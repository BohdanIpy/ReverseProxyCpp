//
// Created by geth on 5/16/25.
//

#ifndef LISTENINGSTREAM_H
#define LISTENINGSTREAM_H

#include <optional>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace fs = std::filesystem;

namespace models
{
    class ListeningStream
    {
      public:
        int number;
        net::ip::address host;
        unsigned short port;
        bool ssl;
        std::optional<fs::path> ssl_cert;
        std::optional<fs::path> ssl_key;
        int route;
    };
}

#endif //LISTENINGSTREAM_H
