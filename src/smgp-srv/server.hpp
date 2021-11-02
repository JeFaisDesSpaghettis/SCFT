#ifndef SERVER_HPP
#define SERVER_HPP

#include "smgp-srv_version.hpp"
#include "scrolling_log.hpp"
#include "room.hpp"
#include <boost/asio.hpp>

namespace smgp
{
    namespace server
    {
        class server
        {
            public: server(
                boost::asio::io_context& io_ctx,
                const std::string& address,
                std::uint16_t port,
                basic_shell::scrolling_log& _log);
            public: ~server();

            private: void accepter();

            boost::asio::ip::tcp::acceptor m_acceptor;
            room m_room;
            basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* SERVER_HPP */