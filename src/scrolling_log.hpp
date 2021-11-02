#ifndef SCROLLING_LOG_HPP
#define SCROLLING_LOG_HPP

#include <deque>
#include <string>
#include <mutex>

/**
 * @file src/scrolling_log.hpp
 * @brief Defines scrolling_log, to help having a fixed size log
*/

namespace smgp
{
    namespace basic_shell
    {
        /**
         * std::deque wrapper for scrolling log
        */
        class scrolling_log
        {
            /**
             * @brief Initialize scrolling log
             * @param width Number of character to considers as full line
             * @param height Maximum number of the lines
            */
            public: scrolling_log(std::size_t width, std::size_t height);

            /**
             * @brief Default destructor
            */
            public: ~scrolling_log();

            /**
             * @brief Add entry to log
             * @param str String
            */
            public: void append_log(const std::string& str);

            /**
             * @brief Add line
             * @param str String
            */
            private: void add_line(const std::string& str);

            /**
             * @brief Returns width
             * @return Width
            */
            public: const std::size_t& get_width() { return m_width; }

            /**
             * @brief Returns height
             * @return height
            */
            public: const std::size_t& get_height() { return m_height; }

            /**
             * @brief Returns reference to log
             * @return Internal deque
            */
            public: const std::deque<std::string>& get_log() { return lines; }

            /**
             * @brief Number of total lines added
             * @return Lines recorded
            */
            public: const std::size_t& get_recorded_lines_count() { return recorded_lines_count; }

            /**
             * @brief Internal width
            */
            private: std::size_t m_width;

            /**
             * @brief Internal height
            */
            private: std::size_t m_height;

            /**
             * @brief Current line to write to
            */
            private: std::string cur_line;

            /**
             * @brief Log lines
            */
            private: std::deque<std::string> lines;

            /**
             * @brief Total number of recorded lines
            */
            private: std::size_t recorded_lines_count;

            /**
             * @brief Prevent concurrent append_log() calls
            */
            private: std::mutex lines_mutex;
        };
    }
}
#endif /* SCROLLING_LOG_HPP */