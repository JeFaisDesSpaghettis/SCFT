#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "smgp-clt_version.hpp"
#include "smgp_message.hpp"
#include "scrolling_log.hpp"

#include <cstdlib>
#include <deque>
#include <boost/asio.hpp>

namespace smgp
{
    namespace client
    {
        class client
        {
            public: client(
                boost::asio::io_context& io_ctx,
                const std::string& address,
                std::uint16_t port,
                basic_shell::scrolling_log& _log);
            public: ~client();
            public: void send_message(message::message _message);
            private: void flush_messages();
            private: void header_reader();
            private: void data_buffer_reader();
            public: std::string get_address();
            public: std::uint16_t get_port();

            private: boost::asio::io_context& m_io_ctx;
            private: boost::asio::ip::tcp::socket m_socket;
            private: message::message m_message;
            private: std::deque<message::message> m_messages;
            private: basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* CLIENT_HPP */