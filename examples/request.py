import numpy as np

from zmq_stream.requester import Requester

req = Requester()
req.configure("localhost", "5511")

vec = np.array([1., 2., 3.])

while True:
    print(req.request(vec, np.float64, 6))
