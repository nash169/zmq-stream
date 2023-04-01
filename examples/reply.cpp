#include <Eigen/Core>
#include <iostream>
#include <zmq_stream/Replier.hpp>

using namespace zmq_stream;

int main(int, char**)
{
    Replier replier;
    replier.configure("0.0.0.0", "5511");

    Eigen::Matrix<double, 3, 1> vec(4., 5., 6.);

    auto task = [&](const Eigen::Vector3d& data) {
        return (Eigen::Matrix<double, 6, 1>() << data, vec).finished();
    };

    while (true)
        replier.reply<Eigen::VectorXd>(task, 3);

    return 0;
}