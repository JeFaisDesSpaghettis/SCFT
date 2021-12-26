#include "server.hpp"

using boost::asio::ip::tcp;

namespace scft
{
    namespace server
    {
    server::server(
        boost::asio::io_context& io_ctx,
        const std::string& address,
        std::uint16_t port,
        basic_shell::scrolling_log& _log)
    :
    m_acceptor(io_ctx, tcp::endpoint(boost::asio::ip::make_address_v4(address), port)),
    m_room(_log),
    m_log(_log)
    {
        m_log.append_log("Listening on " + std::to_string(m_acceptor.local_endpoint().port()) + '\n');
        accepter();
    }

    server::~server()
    {
        m_log.append_log("Stopped listening on " + std::to_string(m_acceptor.local_endpoint().port()) + '\n');
    }

    void server::accepter()
    {
        m_acceptor.async_accept(
            [&](boost::system::error_code ec, tcp::socket _socket)
            {
                if (!ec)
                {
                    m_room.add_member(std::move(_socket));
                }
                accepter();
            });
    }
    }
}