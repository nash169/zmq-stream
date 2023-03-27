#!/usr/bin/env python
# encoding: utf-8

import zmq
import numpy as np


class Replier:
    def __init__(self):
        self.context = zmq.Context()

    def configure(self, host, port):
        self.socket = self.context.socket(zmq.REP)
        self.socket.bind("tcp://" + host + ":" + port)

    def send(self, data):
        self.socket.send(data)

    def receive(self, type, *args):
        data = self.socket.recv()
        return np.frombuffer(data, dtype=type).reshape(*args)
