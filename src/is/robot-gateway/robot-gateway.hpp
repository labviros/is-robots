
#pragma once

#include <is/msgs/robot.pb.h>
#include <tl/expected.hpp>
#include "is/robot-drivers/interface/robot-driver.hpp"

namespace is {

using namespace tl;

class RobotGateway {
  RobotDriver* driver;

  std::chrono::system_clock::time_point last_speed_command;

 public:
  RobotGateway(RobotDriver*);

  auto get_configuration() -> expected<robot::RobotConfig, std::exception>;
  auto set_configuration(robot::RobotConfig const&) -> expected<void, std::exception>;

  void enforce_safety(std::chrono::nanoseconds const& hold_time);
};

}  // namespace is