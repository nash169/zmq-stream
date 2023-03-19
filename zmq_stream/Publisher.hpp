/*
    This file is part of zmq-stream.

    Copyright (c) 2023 Bernardo Fichera <bernardo.fichera@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#ifndef ZMQSTREAM_PUBLISHER_HPP
#define ZMQSTREAM_PUBLISHER_HPP

#include <iostream>
#include <string>
#include <type_traits>
#include <zmq.hpp>

#include "zmq_stream/type_name_rt.hpp"

namespace zmq_stream {
    class Publisher {
    public:
        Publisher()
        {
            // initialize the zmq context with a single IO thread
            _context = std::make_unique<zmq::context_t>(1);
        }

        virtual ~Publisher()
        {
            if (_socket)
                _socket->close();

            _context.reset();
            _socket.reset();
        }

        bool configure(const std::string& host, const std::string& port)
        {
            // check if publisher already connected
            if (_socket) {
                std::cout << "[ZMQ] Publisher already connected" << std::endl;
                return true;
            }

            // construct a PUB socket and bind to interface
            if (_context) {
                _socket = std::make_unique<zmq::socket_t>(*_context, zmq::socket_type::pub);
                _socket->connect("tcp://" + host + ":" + port);
                // _socket->setsockopt(ZMQ_SNDHWM, 1);
                _socket->set(zmq::sockopt::sndhwm, 1);
                _socket->set(zmq::sockopt::conflate, 1);
                // _socket->setsockopt(ZMQ_CONFLATE, 1);
                std::cout << "[ZMQ] Publisher connected" << std::endl;
                return true;
            }

            return false;
        }

        template <typename EigenData>
        void publish(const EigenData& data)
        {
            if (!_socket || !_socket->handle())
                return;

            // std::cout << type_name_str<>() << std::endl;

            zmq::message_t message(data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
            memcpy(message.data(), data.data(), data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
            _socket->send(message, zmq::send_flags::none);
        }

    protected:
        // ZMQ context & socket
        std::unique_ptr<zmq::context_t> _context;
        std::unique_ptr<zmq::socket_t> _socket;
    };

} // namespace zmq_stream

#endif // ZMQSTREAM_PUBLISHER_HPP