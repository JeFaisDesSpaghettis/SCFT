#include "member.hpp"

using boost::asio::ip::tcp;

namespace scft
{
    namespace server
    {
    member::member(tcp::socket _socket, room& group)
    :
    m_socket(std::move(_socket)),
    m_message(),
    m_group(group)
    {
        header_reader();
    }

    member::~member()
    {
    }

    void member::header_reader()
    {
        boost::asio::async_read(m_socket, boost::asio::buffer(m_message.get_raw_message(), message::HEADER_SIZE),
            [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec && !m_message.bad_header())
                {
                    m_message.adjust();
                    data_buffer_reader();
                }
                else
                {
                    m_group.remove_member(shared_from_this());
                }
            });
    }

    void member::data_buffer_reader()
    {
        boost::asio::async_read(m_socket, boost::asio::buffer(m_message.get_data(), m_message.get_data_len()),
            [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    m_group.broadcast(m_message);
                    header_reader();
                }
                else
                {
                    m_group.remove_member(shared_from_this());
                }
            });
    }

    void member::send_message(message::message _message)
    {
        bool send_in_progress = !m_messages.empty();
        m_messages.push_back(_message);
        if (!send_in_progress)
        {
            flush_messages();
        }
    }

    void member::flush_messages()
    {
        boost::asio::async_write(m_socket,  boost::asio::buffer(m_messages.front().get_raw_message(), m_messages.front().get_raw_message().size()),
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
                    m_group.remove_member(shared_from_this());
                }
            });
    }

    std::string member::get_address()
    {
        return m_socket.remote_endpoint().address().to_string();
    }

    std::uint16_t member::get_port()
    {
        return m_socket.remote_endpoint().port();
    }
    }
}