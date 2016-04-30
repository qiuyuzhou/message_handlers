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


class message_handlers_unknown_id_error: public std::runtime_error
{
public:
    message_handlers_unknown_id_error(const std::string& msg)
            :std::runtime_error(msg)
    {}
};

class message_handlers_parse_error: public std::runtime_error
{
public:
    message_handlers_parse_error(const std::string& msg)
            :std::runtime_error(msg)
    {}
};


template <typename ContextType, unsigned int MAX_ID_LIMITED>
class message_handlers_array_policy
{
private:
    typedef std::function<void(int id, const void* buf, std::size_t buf_size, ContextType)> inner_handler_t;
    typedef std::array<inner_handler_t, MAX_ID_LIMITED> container_t;
    container_t _handlers;
public:
    void add_handler(int id, inner_handler_t& handler)
    {
        if (id < 0 || id >= MAX_ID_LIMITED)
        {
            std::array<char, 256> buf;
            std::snprintf(buf.data(), buf.size()
                    , "Id %i is out of range. Id range is limited (0 < id < %i)", id, MAX_ID_LIMITED);
            throw std::runtime_error(std::string(buf.data()));
        }
        if (_handlers[id])
        {
            std::array<char, 256> buf;
            std::snprintf(buf.data(), buf.size()
                    , "Duplicated handler for id %i", id);
            throw std::invalid_argument(std::string(buf.data()));
        }
        _handlers[id] = handler;
    }

    inline void process(int id, const void* buf, std::size_t buf_size, ContextType ctx)const
    {
        if (id < 0 || id >= MAX_ID_LIMITED)
        {
            std::array<char, 256> s;
            std::snprintf(s.data(), s.size()
                    , "No handler for id %i", id);
            throw message_handlers_unknown_id_error(std::string(s.data()));
        }
        auto& handler = _handlers[id];
        if (!handler)
        {
            std::array<char, 256> s;
            std::snprintf(s.data(), s.size()
                    , "No handler for id %i", id);
            throw message_handlers_unknown_id_error(std::string(s.data()));
        }
        handler(id, buf, buf_size, ctx);
    }
};

template <typename ContextType>
class message_handlers_map_policy
{
private:
    typedef std::function<void(int id, const void* buf, std::size_t buf_size, ContextType)> inner_handler_t;
    typedef std::map<int, inner_handler_t> container_t;
    container_t _handlers;
public:
    void add_handler(int id, inner_handler_t& handler)
    {
        if ( _handlers.end() != _handlers.find(id) )
        {
            std::array<char, 256> buf;
            std::snprintf(buf.data(), buf.size()
                    , "Duplicated handler for id %i", id);
            throw std::invalid_argument(std::string(buf.data()));
        }
        _handlers[id] = handler;
    }

    inline void process(int id, const void* buf, std::size_t buf_size, ContextType ctx)const
    {
        auto itr = _handlers.find(id);
        if (itr != _handlers.end() )
        {
            itr->second(id, buf, buf_size, ctx);
        }
        else
        {
            std::array<char, 256> s;
            std::snprintf(s.data(), s.size()
                    , "No handler for id %i", id);
            throw message_handlers_unknown_id_error(std::string(s.data()));
        }
    }
};

template <typename ContextType, typename ContainerTypePolicy=message_handlers_map_policy<ContextType>>
class message_handlers
{
public:
//    typedef google::protobuf::MessageLite PacketBaseType;
    typedef std::function<void(int id, const void* buf, std::size_t buf_size, ContextType)> inner_handler_t;

public:
    message_handlers()
    {
    }

    template <typename PacketType>
    message_handlers& add_handler( int id, std::function<void(int id, PacketType&, ContextType)> handler)
    {
        // TODO static assert to check PacketType and PacketBaseType
        inner_handler_t fn = [&handler](int id, const void* buf, std::size_t buf_size, ContextType context) -> void
        {
            PacketType packet;
            if (!packet.ParseFromArray(buf, buf_size))
            {
                std::array<char, 256> s;
                std::snprintf(s.data(), s.size()
                        , "Parsing message error for id %i", id);
                throw message_handlers_parse_error(std::string(s.data()));
            }
            handler(id, packet, context);
        };
        _handlers.add_handler(id, fn);

        return *this;
    }
    message_handlers& add_raw_handler(
            int id
            , std::function<void(int id, const void* buf, std::size_t buf_size, ContextType)> handler )
    {
        _handlers.add_handler(id, handler);

        return *this;
    }

    inline void process(int id, const void* buf, std::size_t buf_size, ContextType context)const
    {
        _handlers.process(id, buf, buf_size, context);
    }

private:
    ContainerTypePolicy _handlers;
};


#endif //MESSAGE_HANDLERS_MESSAGE_HANDLERS_HPP_H
