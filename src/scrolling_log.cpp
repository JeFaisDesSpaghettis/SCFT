#include "scrolling_log.hpp"
#include <stdexcept>
#include <iostream>

namespace scft
{
    namespace basic_shell
    {
        scrolling_log::scrolling_log(std::size_t width, std::size_t height)
        :
        m_width(width),
        m_height(height),
        recorded_lines_count(0)
        {
            if (m_width < 1 || m_height < 1)
                throw std::logic_error("Width and/or height cannot be lower than 1");
        }

        scrolling_log::~scrolling_log()
        {
        }

        void scrolling_log::append_log(const std::string& str)
        {
            lines_mutex.lock();
            for (const char& ch : str)
            {
                if (cur_line.size() > m_width)
                {
                    add_line(cur_line);
                    cur_line.clear();
                }
                cur_line.push_back(ch);
                if (ch == '\n')
                {
                    add_line(cur_line);
                    cur_line.clear();
                }
            }
            lines_mutex.unlock();
        }

        void scrolling_log::add_line(const std::string& str)
        {
            ++recorded_lines_count;
            if (lines.size() > m_height)
                lines.pop_front();
            lines.push_back(str);
        }
    }
}