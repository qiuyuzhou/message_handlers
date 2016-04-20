//
// Created by 邱宇舟 on 16/4/20.
//

#ifndef MESSAGE_HANDLERS_MESSAGE_HANDLERS_H
#define MESSAGE_HANDLERS_MESSAGE_HANDLERS_H


#include <functional>
#include <exception>
#include <map>
#include <array>

#include <google/protobuf/message_lite.h>

namespace policy {
    typedef std::function<void(int id, const void* buf, std::size_t buf_size)> inner_handler_t;

    template <unsigned int MAX_ID_LIMITED>
    class array_policy
    {
    private:
        typedef std::array<inner_handler_t, MAX_ID_LIMITED> container_t;
        container_t _handlers;
    public:
        void add_handler(int id, inner_handler_t& handler)
        {
            if (id < 0 || id >= MAX_ID_LIMITED)
            {
                throw std::runtime_error("id is out of range");// TODO better format message
            }
            if (_handlers[id])
            {
                throw std::invalid_argument("duplate id");
            }
            _handlers[id] = handler;
        }

        inline void process(int id, const void* buf, std::size_t buf_size)const
        {
            if (id < 0 || id >= MAX_ID_LIMITED)
            {
                throw std::runtime_error("id is out of range");// TODO better format message
            }
            auto& handler = _handlers[id];
            if (!handler)
            {
                throw std::runtime_error("No handler for the id");// TODO better format message
            }
            handler(id, buf, buf_size);
        }
    };

    class map_policy
    {
    private:
        typedef std::map<int, inner_handler_t> container_t;
        container_t _handlers;
    public:
        void add_handler(int id, inner_handler_t& handler)
        {
            if ( _handlers.end() != _handlers.find(id) )
            {
                throw std::invalid_argument("duplate id");
            }
            _handlers[id] = handler;
        }

        inline void process(int id, const void* buf, std::size_t buf_size)const
        {
            auto itr = _handlers.find(id);
            if (itr != _handlers.end() )
            {
                itr->second(id, buf, buf_size);
            }
            else
            {
                throw std::runtime_error("No handler for the id");// TODO better format message
            }
        }
    };
}

template <typename ContainerTypePolicy=policy::map_policy>
class message_handlers
{
public:
    typedef google::protobuf::MessageLite PacketBaseType;
    typedef std::function<void(int id, const void* buf, std::size_t buf_size)> inner_handler_t;


public:
    message_handlers()
    {
    }

    template <typename PacketType>
    message_handlers& add_handler( int id, std::function<void(int id, PacketType&)> handler)
    {
        // TODO static assert to check PacketType and PacketBaseType
        inner_handler_t fn = [&handler](int id, const void* buf, std::size_t buf_size) -> void
        {
            PacketType packet;
            if (!packet.ParseFromArray(buf, buf_size))
            {
                throw std::runtime_error("parse failed");// TODO
            }
            handler(id, packet);
        };
        _handlers.add_handler(id, fn);

        return *this;
    }

    inline void process(int id, const void* buf, std::size_t buf_size)const
    {
        _handlers.process(id, buf, buf_size);
    }

private:
    ContainerTypePolicy _handlers;
};


#endif //MESSAGE_HANDLERS_MESSAGE_HANDLERS_HPP_H
