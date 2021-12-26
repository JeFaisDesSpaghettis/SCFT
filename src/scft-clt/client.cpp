#include "client.hpp"
#include <iostream>

using boost::asio::ip::tcp;

namespace scft
{
    namespace client
    {
    client::client(boost::asio::io_context& io_ctx, const std::string& address, std::uint16_t port, basic_shell::scrolling_log& _log)
    :
    m_io_ctx(io_ctx),
    m_socket(io_ctx),
    m_log(_log)
    {
        tcp::resolver resolver(io_ctx);
        auto endpoints = resolver.resolve(address, std::to_string(port));
        boost::asio::async_connect(m_socket, endpoints,
            [this](boost::system::error_code ec, tcp::endpoint)
            {
                if (!ec)
                {
                    m_log.append_log(
                        "Connected to " + m_socket.remote_endpoint().address().to_string() + ':'
                        + std::to_string(m_socket.remote_endpoint().port()) +'\n');
                    header_reader();
                }
            });
    }

    client::~client()
    {
        m_socket.close();
    }

    void client::send_message(message::message _message)
    {
        boost::asio::post(m_io_ctx,
            [this, _message]()
            {
                bool write_in_progress = !m_messages.empty();
                m_messages.push_back(_message);
                if (!write_in_progress)
                {
                    flush_messages();
                }
            });
    }

    void client::flush_messages()
    {
        boost::asio::async_write(m_socket,
            boost::asio::buffer(m_messages.front().get_raw_message(), m_messages.front().get_raw_message().size()),
            [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    m_messages.pop_front();
                    if (!m_messages.empty())
                    {
                        flush_messages();
                    }
                }
                else
                {
                    m_socket.close();
                }
            });
    }

    void client::header_reader()
    {
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_message.get_raw_message(), message::HEADER_SIZE),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec && !m_message.bad_header())
                {
                    m_message.adjust();
                    data_buffer_reader();
                }
                else
                {
                    m_socket.close();
                }
            });
    }

    void client::data_buffer_reader()
    {
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_message.get_data(), m_message.get_data_len()),
            [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    std::uint32_t checksum = scft::crc32::get_crc32(reinterpret_cast<std::uint8_t*>(m_message.get_data()), m_message.get_data_len());
                    if (checksum == m_message.get_checksum())
                        m_log.append_log("[CRC32 OK!]: ");
                    else
                        m_log.append_log("[CRC32 BAD]: ");
                    m_log.append_log('[' + std::string(m_message.get_origin()) + "]: ");

                    if (m_message.get_message_type() == message::MESSAGE_TYPE::WRITE_FILE)
                    {
                        std::ofstream out_file{m_message.get_string(), std::ios::out | std::ios::binary};
                        out_file.write(reinterpret_cast<const char*>(m_message.get_file_buffer()), m_message.get_file_buffer_len());
                        out_file.close();
                        m_log.append_log("[FILE] " +
                            std::string(m_message.get_string()) + ' ' +
                            std::to_string(m_message.get_file_buffer_len()) + " (bytes)" + '\n');
                    }
                    else if (m_message.get_message_type() == message::MESSAGE_TYPE::TEXT)
                    {
                        m_log.append_log(std::string(m_message.get_string()) + '\n');
                    }
                    header_reader();
                }
                else
                {
                    m_socket.close();
                }
            });
    }


    std::string client::get_address()
    {
        return m_socket.local_endpoint().address().to_string();
    }

    std::uint16_t client::get_port()
    {
        return m_socket.local_endpoint().port();
    }
    }
}