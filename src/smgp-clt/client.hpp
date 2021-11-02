#ifndef CLIENT_HPP
#define CLIENT_HPP

/**
 * @file src/smgp-clt/client.hpp
 * @brief Defines SMGP client class
*/

#include "smgp-clt_version.hpp"
#include "smgp_message.hpp"
#include "scrolling_log.hpp"

#include <cstdlib>
#include <deque>
#include <boost/asio.hpp>

namespace smgp
{
    /**
     * @brief Client
    */
    namespace client
    {
        /**
         * @brief SMGP Client
        */
        class client
        {
            /**
             * @brief Tries to connect to specified server, show message
             * @param io_ctx boost io context
             * @param address IPV4 to listen on
             * @param port to listen on
             * @param _log Log
            */
            public: client(
                boost::asio::io_context& io_ctx,
                const std::string& address,
                std::uint16_t port,
                basic_shell::scrolling_log& _log);

            /**
             * @brief Default constructor
            */
            public: ~client();

            /**
             * @brief Send message
             * @param _message Message to send
            */
            public: void send_message(message::message _message);

            /**
             * @brief Flush queued message
            */
            private: void flush_messages();

            /**
             * @brief Get message header
            */
            private: void header_reader();

            /**
             * @brief Get message data
            */
            private: void data_buffer_reader();

            /**
             * @brief Get local address
             * @return Local address
            */
            public: std::string get_address();

            /**
             * @brief Get local port
             * @return Local port
            */
            public: std::uint16_t get_port();

            /**
             * @brief boost io context
            */
            private: boost::asio::io_context& m_io_ctx;

            /**
             * @brief TCP Socket
            */
            private: boost::asio::ip::tcp::socket m_socket;

            /**
             * @brief Current reading message
            */
            private: message::message m_message;

            /**
             * @brief Output message queue
            */
            private: std::deque<message::message> m_messages;

            /**
             * @brief Log to write to
            */
            private: basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* CLIENT_HPP */