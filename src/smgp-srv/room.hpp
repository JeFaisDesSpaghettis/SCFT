#ifndef ROOM_HPP
#define ROOM_HPP

#include "member.hpp"
#include "scrolling_log.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <mutex>

namespace smgp
{
    namespace server
    {
        class member;

        class room
        {
            public: room(basic_shell::scrolling_log& _log);

            public: ~room();

            public: void add_member(boost::asio::ip::tcp::socket _socket);

            public: void remove_member(std::shared_ptr<member> _member);

            public: void broadcast(message::message _message);

            private: std::vector<std::shared_ptr<member>> m_members;
            private: std::mutex m_members_mutex;
            private: basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* ROOM_HPP */