#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>
#include <utility>
#include <regex>
#include <memory>
#include <functional>
#include <sstream>

#include "nlohmann/json.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "openssl/sha.h"

#include "ListeningStream.h"
#include "NetworkService.h"

using json = nlohmann::json;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;

struct LStream {
    int number;
    net::ip::address host;
    unsigned short port;
    bool ssl;
    std::optional<fs::path> ssl_cert;
    std::optional<fs::path> ssl_key;
    int route;
};

struct NBackend {
    net::ip::address url;
    int port;
    std::optional<int> max_client;
};

struct NService {
    int number;
    std::string path_prefix;
    std::string change_path;
    std::vector<NBackend> backends;
};

struct Config {
    std::vector<LStream> listen;
    std::vector<NService> routes;
};

const std::regex path_regex(R"(^(/[^/\s]+)+/?$)");
inline const EVP_MD* hash_algo = EVP_sha256();
constexpr size_t CHUNK_SIZE_HASHING = 65536;

namespace validation {

    inline std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> load_key(fs::path path_to_key) {
        std::unique_ptr<BIO, std::function<void(BIO*)>> bio(BIO_new_file(path_to_key.string().c_str(),"r"), BIO_free);

        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(nullptr, EVP_PKEY_free);
        EVP_PKEY* k = key.get();

        k = PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr);
        key.reset(k);
        return std::move(key);
    }

    inline std::unique_ptr<X509, decltype(&X509_free)> load_cert(fs::path path_to_cert) {
        std::unique_ptr<BIO, std::function<void(BIO*)>> bio(BIO_new_file(path_to_cert.string().c_str(),"r"), BIO_free);

        std::unique_ptr<X509, decltype(&X509_free)> x509(nullptr, X509_free);
        auto x = x509.get();

        x = PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr);
        x509.reset(x);
        return std::move(x509);
    }

    inline bool validate_x509_cert(fs::path path_to_cert) {
        auto x509 = load_cert(path_to_cert);
        if(x509 == nullptr) {
            std::cerr <<  "The certificate not valid\n";
            return false;
        }
        else {
            return true;
        }
    }

    inline bool validate_key_matches_x509(fs::path key, fs::path cert) {
        return X509_check_private_key(load_cert(cert).get(),  load_key(key).get()) == 1;
    }

    inline bool validate_private_key(fs::path path_to_key) {
        auto k = load_key(path_to_key);
        if(k == nullptr)
        {
            std::cout << "Error with private key\n";
            return false;
        }
        else
        {
            return true;
        }
    }
}

inline std::vector<unsigned char> hash_document_by_path(fs::path pathToDocument) {
    if(!fs::exists(pathToDocument))
        throw std::invalid_argument("Path to document does not exist");

    std::ifstream fileForHashing(pathToDocument);

    if(!fileForHashing.is_open()) {
        throw std::invalid_argument("Cannot open the file");
    }

    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdCtx(EVP_MD_CTX_new(), EVP_MD_CTX_free);

    if(mdCtx == nullptr) {
        std::cerr << "Error with EVP_MD_CTX_new\n";
        throw std::runtime_error("Error with EVP_MD_CTX_new\n");
    }

    if(EVP_DigestInit_ex(mdCtx.get(), hash_algo, nullptr) != 1) {
        throw std::runtime_error("Cannot init the context\n");
    }

    std::vector<unsigned char> buffer(CHUNK_SIZE_HASHING);
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    unsigned int hash_size = 0;
    while(true) {
        fileForHashing.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE_HASHING);
        auto extracted_bytes = fileForHashing.gcount();
        if(extracted_bytes == 0) {
            if(EVP_DigestFinal_ex(mdCtx.get(), hash.data(), &hash_size) != 1) {
                throw std::runtime_error("Error with EVP_DigestFinal_ex, cannot get the hash\n");
            }
            break;
        }
        if(EVP_DigestUpdate(mdCtx.get(), buffer.data(), extracted_bytes) != 1) {
            throw std::runtime_error("Error with EVP_DigestUpdate\n");
        }
    }
    return hash;
}

inline std::string to_hex(const std::vector<unsigned char>& hash) {
    std::ostringstream oss;
    for (auto byte : hash)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    return oss.str();
}

inline
void from_json(const json &j, Config &config) {
    // listen block in Json, check example
    auto listenBlock = j.at("listen");
    for(auto it = listenBlock.begin(); it != listenBlock.end(); ++it) {
        auto number = it.value().at("number").get<int>();
        auto hostStr = it.value().at("host").get<std::string>();
        auto port = it.value().at("port").get<unsigned short>();
        auto ssl = it.value().at("ssl").get<bool>();
        auto route = it.value().at("route").get<int>();
        std::string ssl_cert_path;
        std::string ssl_key_path;
        if(ssl) {
            ssl_cert_path = it.value().at("ssl_cert").get<std::string>();
            ssl_key_path = it.value().at("ssl_key").get<std::string>();
        }
        // assign fields and validate
        LStream stream;
        stream.number = number;
        try {
            stream.host = boost::asio::ip::make_address_v4(hostStr);
        }
        catch(const boost::system::error_code& ec) {
            std::cerr << ec.message() << std::endl;
            throw std::runtime_error("Invalid IP address");
        }
        stream.port = port;
        if(ssl) {
            stream.ssl = ssl;
            fs::path ssl_cert = ssl_cert_path;
            fs::path ssl_key = ssl_key_path;
            if( !fs::exists(ssl_cert) || !fs::exists(ssl_key) ) {
                throw std::runtime_error("Files not found on disk");
            }

            // validate the private key and certificate
            if(!(validation::validate_private_key(ssl_key) && validation::validate_x509_cert(ssl_cert))) {
                throw std::runtime_error("Invalid private key or certificate");
            }
            if(!validation::validate_key_matches_x509(ssl_key, ssl_cert)) {
                throw std::runtime_error("The certificate and key does not match");
            }
            stream.ssl_cert = ssl_cert;
            stream.ssl_key = ssl_key;
        }
        else {
            stream.ssl = ssl;
            stream.ssl_cert = std::nullopt;
            stream.ssl_key = std::nullopt;
        }
        stream.route = route;
        // adding to Config
        config.listen.push_back(stream);
    }
    //route block
    auto routeBlock = j.at("route");
    for(auto it = routeBlock.begin(); it != routeBlock.end(); ++it) {
        NService service;
        auto number = it.value().at("number").get<int>();
        auto pathPrefix = it.value().at("path_prefix").get<std::string>();
        auto changePath = it.value().at("change_path").get<std::string>();
        auto backends = it.value().at("backend");
        for(auto innerIt = backends.begin(); innerIt != backends.end(); ++innerIt) {
            NBackend backend;
            auto urlStr = innerIt.value().at("url").get<std::string>();
            auto port = innerIt.value().at("port").get<unsigned short>();
            if(!(innerIt->contains("max_client"))) {
               backend.max_client = std::nullopt;
            }
            else {
                backend.max_client = innerIt.value().at("max_client").get<int>();
            }
            // validate and assign the data
            try {
                backend.url = boost::asio::ip::make_address_v4(urlStr);
            }
            catch(const boost::system::error_code& ec) {
                std::cerr << ec.message() << std::endl;
                throw std::runtime_error("Invalid IP address");
            }
            backend.port = port;
            service.backends.push_back(backend);
        }
        // assign fields and validate the NService
        if( !std::regex_match(pathPrefix, path_regex) || !std::regex_match(changePath, path_regex) ) {
            throw std::invalid_argument("Invalid path");
        }
        service.path_prefix = pathPrefix;
        service.change_path = changePath;
        service.number = number;
        // Adding to the Config
        config.routes.push_back(service);
    }
    return;
}

#endif // JSONPARSER_H