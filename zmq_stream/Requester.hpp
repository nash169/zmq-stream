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
#ifndef ZMQSTREAM_REQUESTER_HPP
#define ZMQSTREAM_REQUESTER_HPP

#include <iostream>
#include <string>
#include <type_traits>
#include <zmq.hpp>

namespace zmq_stream {
    class Requester {
    public:
        Requester()
        {
            // initialize the zmq context with a single IO thread
            _context = std::make_unique<zmq::context_t>(1);
        }

        virtual ~Requester()
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
                std::cout << "[ZMQ] Requester already connected" << std::endl;
                return true;
            }

            // construct a PUB socket and bind to interface
            if (_context) {
                _socket = std::make_unique<zmq::socket_t>(*_context, zmq::socket_type::req);
                _socket->connect("tcp://" + host + ":" + port);
                std::cout << "[ZMQ] Requester connected" << std::endl;
                return true;
            }

            return false;
        }

        template <typename EigenData, typename... Args>
        EigenData request(const EigenData& data, Args... args)
        {
            if (!_socket || !_socket->handle())
                return EigenData::Zero(args...);

            // send
            zmq::message_t out_msg(data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
            memcpy(out_msg.data(), data.data(), data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
            _socket->send(out_msg, zmq::send_flags::none);

            // receive
            zmq::message_t in_msg;
            auto request = _socket->recv(in_msg, zmq::recv_flags::none); // dontwait
            double* input = in_msg.data<typename EigenData::Scalar>();
            return Eigen::Map<EigenData>(input, args...);
        }

        // template <typename EigenData>
        // void send(const EigenData& data)
        // {
        //     if (!_socket || !_socket->handle())
        //         return;

        //     // std::cout << type_name_str<>() << std::endl;

        //     zmq::message_t message(data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
        //     memcpy(message.data(), data.data(), data.size() * sizeof(typename std::remove_const_t<std::remove_reference_t<decltype(data)>>::Scalar));
        //     _socket->send(message, zmq::send_flags::none);
        // }

        // template <typename EigenData, typename... Args>
        // EigenData receive(Args... args)
        // {
        //     if (!_socket || !_socket->handle())
        //         return EigenData::Zero(args...);

        //     zmq::message_t message;

        //     auto request = _socket->recv(message, zmq::recv_flags::none); // dontwait
        //     double* data = message.data<typename EigenData::Scalar>();

        //     return Eigen::Map<EigenData>(data, args...);
        // }

    protected:
        // ZMQ context & socket
        std::unique_ptr<zmq::context_t> _context;
        std::unique_ptr<zmq::socket_t> _socket;
    };

} // namespace zmq_stream

#endif // ZMQSTREAM_REQUESTER_HPP