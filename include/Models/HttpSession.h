//
// Created by geth on 5/18/25.
//

#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include <memory>
#include <variant>

#include "boost/asio/ssl.hpp"
#include "boost/asio.hpp"

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace fs = std::filesystem;

namespace Models {
    class HttpSession : public std::enable_shared_from_this<HttpSession> {

        // callback for the closing connection

    public:
        std::variant<std::monostate, ssl::stream<net::ip::tcp::socket>, net::ip::tcp::socket> m_socket;

    };
}


#endif //HTTPSESSION_H
