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

#ifndef ZMQSTREAM_SUBSCRIBER_HPP
#define ZMQSTREAM_SUBSCRIBER_HPP

#include <Eigen/Core>
#include <iostream>
#include <string>
#include <zmq.hpp>

namespace zmq_stream {
    class Subscriber {
    public:
        Subscriber()
        {
            // initialize the zmq context with a single IO thread
            _context = std::make_unique<zmq::context_t>(1);
        }

        virtual ~Subscriber()
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
                std::cout << "[ZMQ] Subscriber already connected" << std::endl;
                return true;
            }

            // construct a PUB socket and bind to interface
            if (_context) {
                _socket = std::make_unique<zmq::socket_t>(*_context, zmq::socket_type::sub);

                _socket->set(zmq::sockopt::conflate, 1);
                _socket->set(zmq::sockopt::subscribe, "");

                _socket->bind("tcp://" + host + ":" + port);

                std::cout << "[ZMQ] Subscriber connected" << std::endl;
                return true;
            }

            return false;
        }

        template <typename EigenData, typename... Args>
        EigenData receive(Args... args)
        {
            if (!_socket || !_socket->handle())
                return EigenData::Zero(args...);

            zmq::message_t message;

            auto request = _socket->recv(message, zmq::recv_flags::none);
            double* data = message.data<typename EigenData::Scalar>();

            return Eigen::Map<EigenData>(data, args...);
        }

    protected:
        // ZMQ context & socket
        std::unique_ptr<zmq::context_t> _context;
        std::unique_ptr<zmq::socket_t> _socket;
    };

} // namespace zmq_stream

#endif // ZMQSTREAM_SUBSCRIBER_HPP