#include "server.hpp"
#include "basic_shell.hpp"

#include <cctype>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <chrono>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

class server_shell : scft::basic_shell::basic_shell
{
    public: server_shell()
    {
        m_commands.insert(std::make_pair("help", std::bind(&server_shell::cmd_help, this)));
        m_commands.insert(std::make_pair("start", std::bind(&server_shell::cmd_start, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("stop", std::bind(&server_shell::cmd_stop, this, std::placeholders::_1)));
    }

    public: ~server_shell() {}

    private: bool cmd_help()
    {
        m_log.append_log(
            "SCFT-SERVER v" +
            std::to_string(SCFT_SRV_VERSION_MAJOR) + '.' +
            std::to_string(SCFT_SRV_VERSION_MINOR) + '.' +
            std::to_string(SCFT_SRV_VERSION_PATCH) + '\n');
        m_log.append_log("Available commands: \n");
        m_log.append_log("\thelp: Prints this: \n");
        m_log.append_log("\tstart [IP] [PORT]: Start server\n");
        m_log.append_log("\tstop: Stops server\n");
        m_log.append_log("\tquit: Exits\n");
        return true;
    }

    private: bool cmd_start(const std::vector<std::string>& args)
    {
        if (args.size() < 3)
            return false;
        if (!is_ipv4(args.at(1)) || !is_int(args.at(2)))
            return false;
        if (!m_server)
        {
            m_server = std::make_unique<scft::server::server>(m_io_ctx, args.at(1), boost::lexical_cast<std::uint16_t>(args.at(2)), m_log);
            io_ctx_run_thread = std::thread([&](){ m_io_ctx.run(); });
            m_log.append_log("Started server\n");
            return true;
        }
        return false;
    }

    private: bool cmd_stop(const std::vector<std::string>& args)
    {
        if (m_server)
        {
            m_io_ctx.stop();
            io_ctx_run_thread.join();
            m_server.reset();
            m_io_ctx.restart();
            m_log.append_log("Stopped server\n");
            return true;
        }
        return false;
    }

    private: void update_log(const std::size_t& cursor_y)
    {
        std::size_t rec_line_count = m_log.get_recorded_lines_count();
        while (!quit)
        {
            if (rec_line_count != m_log.get_recorded_lines_count())
            {
                rec_line_count = m_log.get_recorded_lines_count();
                std::cout << PRSM_CURS_RESET;
                basic_shell::clear_log();
                std::cout << PRSM_CURS_RESET;
                basic_shell::show_log();
                std::cout << "\x1B[" << cursor_y << "C";
                // 5 Hz
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            // 240 Hz
            else std::this_thread::sleep_for(std::chrono::milliseconds(4));
        }
    }

    public: void run() override
    {
        prsm_enable_ansi_codes();
        prsm_enable_utf8();
        std::cout << PRSM_SCR_CLEAR_FULL;
        std::size_t input_len = 0;
        std::thread update_log_thread(&server_shell::update_log, this, std::ref(input_len));
        std::string cur_command;
        while (!quit)
        {
            char ch = prsm_getch();
            while (true)
            {
            #if __linux__
                if (ch == 0x7F)
                {
                    if (input_len > 0)
                    {
                        cur_command.pop_back();
                        --input_len;
                    }
                }
                else if (ch == 0x1B)
                {
                    ch = prsm_getch();
                    if (ch == '[')
                    {
                        ch = prsm_getch();
                        //if (ch == 'H')
                        //{
                        //
                        //}
                        //else
                        //{
                            ch = prsm_getch();
                            continue;
                        //}
                    }
                    else
                    {
                        ch = prsm_getch();
                        ch = prsm_getch();
                        continue;
                    }
                }
                else if (ch == 0xA) { break; }
                else if (std::isprint(ch)) { cur_command.push_back(ch); ++input_len; }
            #elif _WIN32
                if (ch == 0x8)
                {
                    if (input_len > 0)
                    {
                        cur_command.pop_back();
                        --input_len;
                    }
                }
                else if (ch == 0xD) { break; }
                else if (std::isprint(ch))
                {
                    cur_command.push_back(ch); ++input_len;
                }
            #endif
                std::cout << PRSM_LINE_CLEAR << '\r' << cur_command;
                ch = prsm_getch();
            }
            std::vector<std::string> split_cmd = split_args(cur_command);
            if (basic_shell::process_cmd(split_cmd) == false)
                m_log.append_log("Invalid command or invalid arguments\n");
            cur_command.clear();
            input_len = 0;
        }
        cmd_stop({""});
        update_log_thread.join();
    }
    private: std::unique_ptr<scft::server::server> m_server;
    private: boost::asio::io_context m_io_ctx;
    private: std::thread io_ctx_run_thread;
};


int main(int argc, char* argv[])
{
    server_shell _shell;
    _shell.run();
    return 0;
}