#ifndef MEMBER_HPP
#define MEMBER_HPP

#include "smgp_message.hpp"
#include "room.hpp"

#include <boost/asio.hpp>
#include <deque>

namespace smgp
{
    namespace server
    {
        class room;

        class member : public std::enable_shared_from_this<member>
        {
            public: member(boost::asio::ip::tcp::socket _socket, room& group);

            public: ~member();

            private: void header_reader();

            private: void data_buffer_reader();

            public: void send_message(message::message _message);

            private: void flush_messages();

            public: std::string get_address();

            public: std::uint16_t get_port();

            boost::asio::ip::tcp::socket m_socket;
            message::message m_message;
            std::deque<message::message> m_messages;
            room& m_group;
        };
    }
}

#endif /* MEMBER_HPP */