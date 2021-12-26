#ifndef ROOM_HPP
#define ROOM_HPP

/**
 * @file src/scft-srv/room.hpp
 * @brief Defines room class, used by server
*/

#include "member.hpp"
#include "scrolling_log.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <mutex>

namespace scft
{
    namespace server
    {
        class member;

        /**
         * @brief Server room
        */
        class room
        {
            /**
             * @brief Specify the log
             * @param _log Log
            */
            public: room(basic_shell::scrolling_log& _log);

            /**
             * @brief Default destructor
            */
            public: ~room();

            /**
             * @brief Add client connection
             * @param _socket Connected socket
            */
            public: void add_member(boost::asio::ip::tcp::socket _socket);

            /**
             * @brief Remove client connection
             * @param _member Member to remove
            */
            public: void remove_member(std::shared_ptr<member> _member);

            /**
             * @brief Send message to every member except message origin
             * @param _message Initialized message to broadcast
            */
            public: void broadcast(message::message _message);

            /**
             * @brief Member list
            */
            private: std::vector<std::shared_ptr<member>> m_members;

            /**
             * @brief Member add and remove sync
            */
            private: std::mutex m_members_mutex;

            /**
             * @brief Log
            */
            private: basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* ROOM_HPP */