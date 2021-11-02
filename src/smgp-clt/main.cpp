#include "client.hpp"
#include "basic_shell.hpp"

#include <cctype>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <chrono>
#include <deque>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

class client_shell : smgp::basic_shell::basic_shell
{
    public: client_shell()
    {
        m_commands.insert(std::make_pair("help", std::bind(&client_shell::cmd_help, this)));
        m_commands.insert(std::make_pair("connect", std::bind(&client_shell::cmd_connect, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("disconnect", std::bind(&client_shell::cmd_disconnect, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("sendtext", std::bind(&client_shell::cmd_sendtext, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("sendfile", std::bind(&client_shell::cmd_sendfile, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("st", std::bind(&client_shell::cmd_sendtext, this, std::placeholders::_1)));
        m_commands.insert(std::make_pair("sf", std::bind(&client_shell::cmd_sendfile, this, std::placeholders::_1)));
    }

    public: ~client_shell() {}

    private: bool cmd_help()
    {
        m_log.append_log(
            "SMGP-CLIENT v" +
            std::to_string(SMGP_CLT_VERSION_MAJOR) + '.' +
            std::to_string(SMGP_CLT_VERSION_MINOR) + '.' +
            std::to_string(SMGP_CLT_VERSION_PATCH) + '\n');
        m_log.append_log("Available commands: \n");
        m_log.append_log("\thelp: Prints this: \n");
        m_log.append_log("\tconnect [IP] [PORT]: Connect to server\n");
        m_log.append_log("\tdisconnect: Disconnect\n");
        m_log.append_log("\tsendtext [MESSAGE...]: Send message\n");
        m_log.append_log("\tsendfile [FILEPATH]: Send file\n");
        m_log.append_log("\tst: Alias of sendtext\n");
        m_log.append_log("\tsf: Alias of sendfile\n");
        m_log.append_log("\tquit: Exits\n");
        return true;
    }

    private: bool cmd_connect(const std::vector<std::string>& args)
    {
        if (args.size() < 3)
            return false;
        if (!is_ipv4(args.at(1)) || !is_int(args.at(2)))
            return false;
        if (!m_client)
        {
            m_client = std::make_unique<smgp::client::client>(m_io_ctx, args.at(1), boost::lexical_cast<std::uint16_t>(args.at(2)), m_log);
            io_ctx_run_thread = std::thread([&](){ m_io_ctx.run(); });
            m_log.append_log("Started client\n");
            return true;
        }
        return false;
    }

    private: bool cmd_disconnect(const std::vector<std::string>& args)
    {
        if (m_client)
        {
            m_io_ctx.stop();
            io_ctx_run_thread.join();
            m_client.reset();
            m_io_ctx.restart();
            m_log.append_log("Stopped client\n");
            return true;
        }
        return false;
    }

    private: bool cmd_sendtext(const std::vector<std::string>& args)
    {
        if (args.size() < 2)
            return false;
        if (m_client)
        {
            std::string message_string;
            for (std::size_t index = 1; index < args.size(); index++)
            {
                message_string.append(args.at(index));
                message_string.push_back(' ');
            }
            smgp::message::message _message{smgp::message::MESSAGE_TYPE::TEXT, m_client->get_address(), m_client->get_port(), message_string};
            m_client->send_message(_message);
            m_log.append_log(
                "[" + m_client->get_address() + ":" + std::to_string(m_client->get_port()) + "]: " +
                message_string + "\n");
            return true;
        }
        return false;
    }

    private: bool cmd_sendfile(const std::vector<std::string>& args)
    {
        if (args.size() != 2)
            return false;
        if(!std::filesystem::is_regular_file(args.at(1)))
            return false;
        if (std::filesystem::file_size(args.at(1)) > smgp::message::MAX_DATA_LENGTH - smgp::message::HEADER_SIZE)
            return false;
        if (m_client)
        {
            smgp::message::message _message{smgp::message::MESSAGE_TYPE::WRITE_FILE, m_client->get_address(), m_client->get_port(), args.at(1)};
            m_client->send_message(_message);
            m_log.append_log(
                '[' + m_client->get_address() + ':' + std::to_string(m_client->get_port()) + "]: " +
                "[FILE]: " + args.at(1) + '\n');
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
        std::cout << "\x1B[" << m_log.get_height() << "B";
        std::size_t input_len = 0;
        std::thread update_log_thread(&client_shell::update_log, this, std::ref(input_len));
        std::string cur_command;
        while (!quit)
        {
            char ch = prsm_getch();
            while (true)
            {
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
                std::cout << PRSM_LINE_CLEAR << '\r' << cur_command;
                ch = prsm_getch();
            }
            std::vector<std::string> split_cmd = split_args(cur_command);
            if (basic_shell::process_cmd(split_cmd) == false)
                m_log.append_log("Invalid command or invalid arguments\n");
            cur_command.clear();
            input_len = 0;
        }
        cmd_disconnect({""});
        update_log_thread.join();
    }
    private: std::unique_ptr<smgp::client::client> m_client;
    private: boost::asio::io_context m_io_ctx;
    private: std::thread io_ctx_run_thread;
};


int main(int argc, char* argv[])
{
    client_shell _shell;
    _shell.run();
    return 0;
}