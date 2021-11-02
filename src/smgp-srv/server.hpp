#ifndef SERVER_HPP
#define SERVER_HPP

/**
 * @file src/smgp-srv/server.hpp
 * @brief Defines SMGP server class
*/

#include "smgp-srv_version.hpp"
#include "scrolling_log.hpp"
#include "room.hpp"
#include <boost/asio.hpp>

namespace smgp
{
    /**
     * @brief Server
    */
    namespace server
    {
        /**
         * @brief SMGP Server
        */
        class server
        {
            /**
             * @brief Listen to specified address and port
             * @param io_ctx boost io context
             * @param address IPV4 to listen on
             * @param port to listen on
             * @param _log Log
            */
            public: server(
                boost::asio::io_context& io_ctx,
                const std::string& address,
                std::uint16_t port,
                basic_shell::scrolling_log& _log);

            /**
             * @brief Default destructor
            */
            public: ~server();

            /**
             * @brief Listen
            */
            private: void accepter();

            /**
             * @brief TCP Accept socket
            */
            boost::asio::ip::tcp::acceptor m_acceptor;

            /**
             * @brief Server room
            */
            room m_room;

            /**
             * @brief Log to write to
            */
            basic_shell::scrolling_log& m_log;
        };
    }
}

#endif /* SERVER_HPP */