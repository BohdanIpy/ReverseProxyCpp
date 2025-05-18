//
// Created by geth on 5/16/25.
//

#include "ListeningStream.h"

models::ListeningStream::ListeningStream(net::io_context& ioc,
            std::optional<fs::path> cert_path,
            std::optional<fs::path> key_path,
            int number,
            net::ip::address host,
            unsigned short port,
            int route) : m_acceptor(ioc, net::ip::tcp::endpoint(host, port)),
                         m_io_context(ioc) {
    this->m_number = number;
    this->m_host = host;
    this->m_port = port;
    this->m_route = route;

    if (cert_path.has_value() && key_path.has_value() ) {
        // ssl definition
        ssl::context sslctx(net::ssl::context::tlsv12_server);
        this->m_context = std::move(sslctx);
        this->m_context->use_certificate_file(cert_path.value().string(), ssl::context_base::pem);
        this->m_context->use_private_key_file(key_path.value(), ssl::context_base::pem);
    }
    else if (!cert_path.has_value() && !key_path.has_value()) {
        // non-ssl definition
        net::ip::tcp::socket socket(this->m_io_context);
        this->m_context = std::nullopt;
    }
    throw std::invalid_argument("Invalid certificate or key");
}

models::ListeningStream::~ListeningStream() {
}
