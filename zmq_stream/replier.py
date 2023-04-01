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

    def reply(self, task, type, *args):
        data = np.frombuffer(self.socket.recv(), dtype=type).reshape(*args)
        self.socket.send(task(data))
