#!/usr/bin/env python
# encoding: utf-8

import zmq
import numpy as np


class Requester:
    def __init__(self):
        self.context = zmq.Context()

    def configure(self, host, port):
        self.socket = self.context.socket(zmq.REQ)
        self.socket.connect("tcp://" + host + ":" + port)

    def request(self, data, type, *args):
        self.socket.send(data)
        return np.frombuffer(self.socket.recv(), dtype=type).reshape(*args)
