#include "basic_shell.hpp"
#include <algorithm>

namespace scft
{
    namespace basic_shell
    {
        basic_shell::basic_shell()
        :
        quit(false),
        m_log(SCROLL_LOG_WIDTH, SCROLL_LOG_HEIGHT)
        {
            m_commands.insert(std::make_pair("quit", std::bind(&basic_shell::set_quit, this)));
        }

        basic_shell::~basic_shell()
        {
        }

        bool basic_shell::is_int(const std::string& str)
        {
            std::size_t index = 0;
            if (str[0] == '-')
                index = 1;
            for (; index < str.size(); index++)
            {
                if (!isdigit(str[index]))
                    return false;
            }
            return true;
        }

        bool basic_shell::is_ipv4(const std::string& str)
        {
            std::string ipv4_parts[4];
            unsigned int part_index = 0;
            unsigned int dot_count = 0;
            if (str.length() > 15 || str.length() < 7)
                return false;
            for (std::size_t index = 0; index < str.length(); index++)
            {
                if (isdigit(str[index]) != 0)
                    ipv4_parts[part_index].push_back(str[index]);

                else if (str[index] == '.')
                {
                    part_index++;
                    dot_count++;
                }
                else
                    return false;
            }
            if (dot_count != 3)
            {
                return false;
            }
            for (std::size_t index = 0; index < 4; index++)
            {
                if (is_int(ipv4_parts[index]) == false)
                    return false;
                if (std::stoi(ipv4_parts[index]) > 255 || 0 > std::stoi(ipv4_parts[index]))
                    return false;
            }
            return true;
        }

        std::vector<std::string> basic_shell::split_args(const std::string& command)
        {
            bool is_inside_quote = false;
            std::vector<std::string> arguments;
            std::string cur_arg;
            for (std::size_t index = 0; index < command.length(); index++)
            {
                switch (command.at(index))
                {
                    case '\"':
                        is_inside_quote = (is_inside_quote == true) ? false : true;
                        break;
                    case ' ':
                        if (is_inside_quote)
                        {
                            if (std::isprint(command.at(index)))
                            {
                                if (std::isalpha(command.at(index)))
                                    cur_arg.push_back(command.at(index));
                                else
                                    cur_arg.push_back(command.at(index));
                            }
                        }
                        else if (command.at(index - 1) != ' ')
                        {
                            arguments.push_back(cur_arg);
                            cur_arg.clear();
                        }
                        break;
                    default:
                        if (std::isprint(command.at(index)))
                        {
                            if (std::isalpha(command.at(index)))
                                cur_arg.push_back(command.at(index));
                            else
                                cur_arg.push_back(command.at(index));
                        }
                        break;
                }
            }
            arguments.push_back(cur_arg);
            return arguments;
        }

        bool basic_shell::process_cmd(const std::vector<std::string>& args)
        {
            std::unordered_map<std::string, std::function<bool (const std::vector<std::string> &)>>::iterator command = m_commands.find(args.at(0));
            if (command != m_commands.end())
            {
                command->second(args);
                return true;
            }
            return false;
        }

        bool basic_shell::set_quit()
        {
            return (quit = true);
        }

        void basic_shell::clear_log()
        {
            for (std::size_t index = 0; index < m_log.get_height(); index++)
                std::cout << PRSM_LINE_CLEAR << '\n';
        }

        void basic_shell::show_log()
        {
            std::size_t index = 0;
            for (; index < m_log.get_log().size(); index++)
            {
                if (m_log.get_log().at(index).back() == '\n')
                    std::cout << m_log.get_log().at(index) << PRSM_LINE_CLEAR;
                else
                    std::cout << m_log.get_log().at(index);
            }
            for (; index < m_log.get_height(); index++)
                std::cout << PRSM_LINE_CLEAR << '\n';
        }

        void basic_shell::run()
        {
            prsm_enable_ansi_codes();
            prsm_enable_utf8();
            for (std::size_t index = 0; index < m_log.get_height(); index++)
                std::cout << '\n';
            while (!quit)
            {
                std::string cur_command;
                std::getline(std::cin, cur_command);
                std::vector<std::string> split_cmd = split_args(cur_command);
                process_cmd(split_cmd);
                std::cin.clear();
                std::cout.flush();
            }
        }
    }
}

