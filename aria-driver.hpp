#ifndef __ARIA_DRIVER_HPP__
#define __ARIA_DRIVER_HPP__

#ifndef isnan
#if __cplusplus <= 199711L  // c++98 or older
#define isnan(x) ::isnan(x)
#else
#define isnan(x) std::isnan(x)
#endif
#endif

#include <Aria/Aria.h>
#include <string>
#include "robot-driver.hpp"

namespace is {
namespace robot {

namespace pb {
using namespace google::protobuf::util;
using namespace google::protobuf;
}

using namespace is::common;
using namespace is::robot;

class AriaDriver : public RobotDriver {
  ArRobot robot;
  PowerInfo power_info;

 public:
  AriaDriver(){};
  ~AriaDriver() {
    robot.stopRunning();
    robot.waitForRunExit();
  }

  enum ConnectionType { SERIAL, TCP };

  void connect(std::string const& uri_connection, ConnectionType type) {
    switch (type) {
    case ConnectionType::SERIAL: {
      auto* c = new ArSerialConnection();
      c->open(uri_connection.c_str());
      set_connection(c);
      break;
    }
    case ConnectionType::TCP: {
      auto* c = new ArTcpConnection();
      auto pos = uri_connection.find_last_of(':');
      if (pos == std::string::npos)
        throw std::runtime_error("Invalid robot URI");
      auto hostname = uri_connection.substr(0, pos);
      auto port = std::stoi(uri_connection.substr(pos + 1));
      c->open(hostname.c_str(), port);
      set_connection(c);
      break;
    }
    }
    start();
  }

  void set_speed(Speed const& speed) override {
    robot.lock();
    robot.setVel(1000.0*speed.linear());
    robot.setRotVel(speed.angular() * 90.0 / std::asin(1));
    robot.unlock();
  }

  void set_pose(Pose const& pose) override {
    ArPose ar_pose(1000.0*pose.position().x(), 1000.0*pose.position().y(),
                   pose.orientation().roll() * 90.0 / std::asin(1));
    robot.lock();
    robot.moveTo(ar_pose);
    robot.unlock();
  }

  void set_power_info(PowerInfo const& power_info) override { this->power_info = power_info; }

  Speed get_speed() override {
    Speed speed;
    robot.lock();
    speed.set_linear(robot.getVel() / 1000.0);
    speed.set_angular(robot.getRotVel() * std::asin(1) / 90.0);
    robot.unlock();
    return speed;
  }

  Pose get_pose() override {
    Pose pose;
    robot.lock();
    pose.mutable_position()->set_x(robot.getX() / 1000.0);
    pose.mutable_position()->set_y(robot.getY() / 1000.0);
    pose.mutable_orientation()->set_roll(robot.getTh() * std::asin(1) / 90.0);
    robot.unlock();
    return pose;
  }

  PowerInfo get_power_info() override {
    robot.lock();
    auto real_voltage = static_cast<float>(robot.getRealBatteryVoltageNow());
    auto normalized_voltage = static_cast<float>(robot.getBatteryVoltageNow());
    robot.unlock();
    power_info.set_voltage(real_voltage);
    power_info.set_charge(normalized_voltage / 12.0);
    return power_info;
  }

  void start() override {
    robot.lock();
    robot.enableMotors();
    robot.unlock();
  }

  void stop() override {
    robot.lock();
    robot.disableMotors();
    robot.unlock();
  }

 private:
  template <typename Connection>
  void set_connection(Connection* c) {
    Aria::init();
    robot.setDeviceConnection(c);
    if (!robot.blockingConnect()) {
      throw std::runtime_error("Could not connect to robot.");
    }
    robot.runAsync(true);
  }

};  // AriaDriver

}  // namespace robot
}  // namespace is

#endif  // __ARIA_DRIVER_HPP__