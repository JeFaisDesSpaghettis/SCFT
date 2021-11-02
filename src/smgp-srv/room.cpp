#include "room.hpp"

using boost::asio::ip::tcp;

namespace smgp
{
    namespace server
    {
    room::room(basic_shell::scrolling_log& _log)
    :
    m_log(_log)
    {
    }

    room::~room()
    {
    }

    void room::add_member(tcp::socket _socket)
    {
        m_log.append_log(
            "Adding: " + _socket.remote_endpoint().address().to_string() + ':' +
            std::to_string(_socket.remote_endpoint().port()) +  '\n');
        broadcast(
            message::message(
                message::MESSAGE_TYPE::TEXT, _socket.remote_endpoint().address().to_string(), _socket.remote_endpoint().port(), " HAS JOINED"));

        m_members_mutex.lock();
        m_members.push_back(std::make_shared<member>(std::move(_socket), *this));
        m_members_mutex.unlock();
    }

    void room::remove_member(std::shared_ptr<member> _member)
    {
        m_log.append_log("Removing: " + _member->get_address() + ':' + std::to_string(_member->get_port()) +  '\n');
        broadcast(message::message(message::MESSAGE_TYPE::TEXT, _member->get_address(), _member->get_port(), " HAS LEFT"));
        m_members_mutex.lock();
        for (std::size_t index = 0; index < m_members.size(); index++)
        {
            if (m_members[index].get() == _member.get())
            {
                m_members.erase(m_members.begin() + index);
                m_members_mutex.unlock();
                return;
            }
        }
        throw std::logic_error("Double self-destruction");
    }

    void room::broadcast(message::message _message)
    {
        m_log.append_log("Broadcasting: " + std::string(_message.get_string()) + '\n');
        for (std::shared_ptr<member>& _member : m_members)
        {
            std::string origin = _member->get_address() + ":" + std::to_string(_member->get_port());
            if (origin != _message.get_origin())
                _member->send_message(_message);
        }
    }
    }
}
