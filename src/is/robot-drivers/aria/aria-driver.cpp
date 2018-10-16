
#include "aria-driver.hpp"
#include <fmt/format.h>

namespace is {

AriaDriver::AriaDriver(std::string const& uri) {
  Aria::init();

  std::smatch match;
  ArDeviceConnection* device;
  if (std::regex_match(uri, match, std::regex("serial:\\/\\/(\\w+)"))) {
    device = new ArSerialConnection();
    auto port = fmt::format("/dev/{}", match[1].str());
    fmt::print("AriaDriver serial={}\n", port);
    static_cast<ArSerialConnection*>(device)->open(port.c_str());
  } else if (std::regex_match(uri, match, std::regex("tcp:\\/\\/(\\w|.+):([0-9]+)"))) {
    device = new ArTcpConnection();
    auto hostname = match[1].str();
    auto port = std::stoi(match[2].str());
    fmt::print("AriaDriver hostname={} port={}\n", hostname, port);
    static_cast<ArTcpConnection*>(device)->open(hostname.c_str(), port);
  } else {
    throw std::runtime_error(
        "Invalid uri format, valid formats: serial://tty20 or tcp://10.0.0.1:2000");
  }

  robot.setDeviceConnection(device);
  if (!robot.blockingConnect()) {
    throw std::runtime_error(fmt::format("Could not connect to robot using {}", uri));
  }

  robot.run();
  robot.enableMotors();
  // robot.runAsync(true);
}

auto AriaDriver::stop() -> expected<void, std::exception> {
  return set_speed(common::Speed{});
}

auto AriaDriver::get_speed() -> expected<common::Speed, std::exception> {
  auto speed = common::Speed{};
  // robot.lock();
  speed.set_linear(robot.getVel() / 1000.0);
  speed.set_angular(robot.getRotVel() * std::asin(1) / 90.0);
  // robot.unlock();
  return speed;
}

auto AriaDriver::set_speed(common::Speed const& speed) -> expected<void, std::exception> {
  // robot.lock();
  robot.setVel(1000.0 * speed.linear());                   // aria linear vel is mm/s
  robot.setRotVel(speed.angular() * 90.0 / std::asin(1));  // aria angular vel is degree/s
  // robot.unlock();
  return {};
}

auto AriaDriver::get_pose() -> expected<common::Pose, std::exception> {
  auto pose = common::Pose{};
  // robot.lock();
  pose.mutable_position()->set_x(robot.getX() / 1000.0);
  pose.mutable_position()->set_y(robot.getY() / 1000.0);
  pose.mutable_orientation()->set_roll(robot.getTh() * std::asin(1) / 90.0);
  // robot.unlock();
  return pose;
}

}  // namespace is