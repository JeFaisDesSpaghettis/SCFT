#ifndef SCROLLING_LOG_HPP
#define SCROLLING_LOG_HPP

#include <deque>
#include <string>
#include <mutex>

namespace smgp
{
    namespace basic_shell
    {
        class scrolling_log
        {
            public: scrolling_log(std::size_t width, std::size_t height);
            public: ~scrolling_log();

            public: void append_log(const std::string& str);
            private: void add_line(const std::string& str);

            public: const std::size_t& get_width() { return m_width; }
            public: const std::size_t& get_height() { return m_height; }

            public: const std::deque<std::string>& get_log() { return lines; }
            public: const std::size_t& get_recorded_lines_count() { return recorded_lines_count; }

            private: std::size_t m_width;
            private: std::size_t m_height;
            private: std::string cur_line;
            private: std::deque<std::string> lines;
            private: std::size_t recorded_lines_count;
            private: std::mutex lines_mutex;
        };
    }
}
#endif /* SCROLLING_LOG_HPP */