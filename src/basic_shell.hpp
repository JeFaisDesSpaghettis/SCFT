#ifndef BASIC_SHELL_HPP
#define BASIC_SHELL_HPP

/**
 * @file src/basic_shell.hpp
 * @brief Defines basic_shell, to be derived to create a CLI interface
*/

#include "Prisma.h"

#include "scrolling_log.hpp"

#include <cctype>
#include <cstdio>

#include <functional>
#include <iostream>
#include <mutex>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef __WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
#endif

namespace smgp
{
    /**
     * @brief Base CLI interface
    */
    namespace basic_shell
    {
        /**
         * @brief Log line width limit
        */
        constexpr std::size_t SCROLL_LOG_WIDTH = 40;

        /**
         * @brief Limit of maximum lines logged
        */
        constexpr std::size_t SCROLL_LOG_HEIGHT = 25;

        /**
         * @brief Basic CLI interface
        */
        class basic_shell
        {
            /**
             * @brief SCROLL_LOG_WIDTHxSCROLL_LOG_HEIGHT scrolling_log
            */
            public: basic_shell();

            /**
             * @brief Default destructor
            */
            public: ~basic_shell();

            /**
             * @brief Check if string is an int
             * @param str String to check
             * @return True if valid integer
            */
            protected: bool is_int(const std::string& str);

            /**
             * @brief Check if string is IPV4 address
             * @param str String to check
             * @return True if valid ipv4 address
            */
            protected: bool is_ipv4(const std::string& str);

            /**
             * @brief Split string by spaces, except everything inquotes
             * @param command String to split
             * @return Split args
            */
            protected: std::vector<std::string> split_args(const std::string& command);

            /**
             * @brief Call command
             * @param args Arguments
             * @return True if it could find the command, false if unavailable
            */
            protected: bool process_cmd(const std::vector<std::string>& args);

            /**
             * @brief Set quit to true
             * @return Returns always true for (m_commands)
            */
            protected: bool set_quit();

            /**
             * @brief Clears SCROLL_LOG_HEIGHT lines
            */
            protected: void clear_log();

            /**
             * @brief Show log
            */
            protected: void show_log();

            /**
             * @brief Get command and process it
            */
            public: virtual void run();

            /**
             * @brief Exits run on true
            */
            protected: bool quit;

            /**
             * @brief Command list
            */
            protected: std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> m_commands;

            /**
             * @brief Log
            */
            protected: scrolling_log m_log;

            /**
             * @brief cout synchronization
            */
            protected: std::mutex m_iostream_mutex;
        };
    }
}

#endif /* BASIC_SHELL_HPP */