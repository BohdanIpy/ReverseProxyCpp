//
// Created by geth on 5/16/25.
//

#ifndef LISTENINGSTREAM_H
#define LISTENINGSTREAM_H

#include <optional>
#include <filesystem>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace fs = std::filesystem;
using UUID = boost::uuids::uuid;

namespace models
{
    // from outside (server-side like)
    class ListeningStream
    {
    public:
        ListeningStream(net::io_context& ioc,
            std::optional<fs::path> cert_path,
            std::optional<fs::path> key_path,
            int number,
            net::ip::address host,
            unsigned short port,
            int route);

        ~ListeningStream();

        int m_number;
        net::ip::address m_host;
        unsigned short m_port;
        int m_route;
        net::ip::tcp::acceptor m_acceptor;
        // ssl part
        net::io_context& m_io_context;
        std::optional<ssl::context> m_context;
        // callback for deleting the connection
        std::unordered_map<UUID, std::function<void()>> m_contexts;
    };
}

#endif //LISTENINGSTREAM_H
