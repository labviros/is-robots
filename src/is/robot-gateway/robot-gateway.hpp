
#pragma once

#include <is/msgs/robot.pb.h>
#include <is/wire/core.hpp>
#include <tl/expected.hpp>
#include "conf/options.pb.h"
#include "is/robot-drivers/interface/robot-driver.hpp"

namespace is {

using namespace tl;

class RobotGateway {
  Channel channel;
  RobotDriver* driver;
  RobotParameters params;
  std::chrono::system_clock::time_point last_speed_command;
  std::chrono::system_clock::time_point last_sampling;

 public:
  RobotGateway(Channel const&, RobotDriver*, RobotParameters const&);

  auto get_configuration() -> expected<robot::RobotConfig, std::exception>;
  auto set_configuration(robot::RobotConfig const&) -> expected<void, std::exception>;

  auto next_deadline() const -> std::chrono::system_clock::time_point;
  void run();

 private:
  void enforce_safety();
  void publish_sensors();

  auto safety_deadline() const -> std::chrono::system_clock::time_point;
  auto sampling_deadline() const -> std::chrono::system_clock::time_point;
};

}  // namespace is