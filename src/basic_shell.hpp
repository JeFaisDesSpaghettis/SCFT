#ifndef BASIC_SHELL_HPP
#define BASIC_SHELL_HPP

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
    namespace basic_shell
    {
        class basic_shell
        {
            public: basic_shell();

            public: ~basic_shell();

            protected: bool is_int(const std::string& str);

            protected: bool is_ipv4(const std::string& str);

            protected: std::vector<std::string> split_args(const std::string& command);

            protected: bool process_cmd(const std::vector<std::string>& args);

            protected: bool set_quit();

            protected: void clear_log();

            protected: void show_log();

            public: virtual void run();

            protected: bool quit;

            protected: std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> m_commands;

            protected: scrolling_log m_log;

            protected: std::mutex m_iostream_mutex;
        };
    }
}

#endif /* BASIC_SHELL_HPP */