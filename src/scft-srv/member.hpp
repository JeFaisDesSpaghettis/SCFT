#ifndef MEMBER_HPP
#define MEMBER_HPP

/**
 * @file src/scft-srv/member.hpp
 * @brief Defines member class, contained in the room
*/

#include "scft_message.hpp"
#include "room.hpp"

#include <boost/asio.hpp>
#include <deque>

namespace scft
{
    namespace server
    {
        class room;

        /**
         * @brief Room member
        */
        class member : public std::enable_shared_from_this<member>
        {
            /**
             * @brief Starts checking for message
             * @param _socket Member socket
             * @param group Room in which the member belongs
            */
            public: member(boost::asio::ip::tcp::socket _socket, room& group);

            /**
             * @brief Default destructor
            */
            public: ~member();

            /**
             * @brief Check for message header from client
            */
            private: void header_reader();

            /**
             * @brief Read message reader from client
            */
            private: void data_buffer_reader();

            /**
             * @brief Send message to client
             * @param _message Initialized message to send
            */
            public: void send_message(message::message _message);

            /**
             * @brief Flush message to client
            */
            private: void flush_messages();

            /**
             * @brief Get remote address
             * @return Remote IPV4 String
            */
            public: std::string get_address();

            /**
             * @brief Get remote port
             * @return Remote port
            */
            public: std::uint16_t get_port();

            /**
             * @brief boost tcp socket
            */
            boost::asio::ip::tcp::socket m_socket;

            /**
             * @brief Current reading message
            */
            message::message m_message;

            /**
             * @brief Message queue
            */
            std::deque<message::message> m_messages;

            /**
             * @brief Room in which it is contained
            */
            room& m_group;
        };
    }
}

#endif /* MEMBER_HPP */