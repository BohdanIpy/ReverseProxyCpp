//
// Created by geth on 5/3/25.
//

#include "HotReloadConfigurer.h"
#include <openssl/sha.h>

using namespace ht;

bool HotReloadConfigurer::ConfigurerBuilder::setPath(const fs::path &path) {
    if (fs::exists(path) && fs::is_regular_file(path)) {
        this->m_path = path;
        return true;
    }
    return false;
}

std::unique_ptr<HotReloadConfigurer> HotReloadConfigurer::ConfigurerBuilder::build() {
    return std::unique_ptr<HotReloadConfigurer>(new HotReloadConfigurer(this->m_path));
}

HotReloadConfigurer::ConfigurerBuilder HotReloadConfigurer::builder() {
    return {};
}

HotReloadConfigurer::HotReloadConfigurer(const fs::path& pathToConfig) :
    m_pathConfigFile(pathToConfig),
    m_digest(EVP_sha256()),
    m_mdCTX(nullptr, EVP_MD_CTX_free)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create MD ctx");
    }
    this->m_mdCTX.reset(ctx);

    if(EVP_DigestInit_ex(m_mdCTX.get(), m_digest,  nullptr) != 1) {
        throw std::runtime_error("Failed to initialize MD ctx");
    }

    loadConfiguration();
}

void HotReloadConfigurer::loadConfiguration() {
    // here i need to:
    // 1) hash the configuration file
    //    - If nothing changed, do nothing
    //    - Or else parse the json, and compare and modify the listeners and streams
    //    - Update the hash( actualization)


}
