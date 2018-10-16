
#pragma once

#include <Aria/Aria.h>
#include <regex>
#include "is/robot-drivers/interface/robot-driver.hpp"

namespace is {

class AriaDriver : public RobotDriver {
  ArRobot robot;

 public:
  AriaDriver(std::string const& uri);

  auto stop() -> expected<void, std::exception> override;
  auto get_speed() -> expected<common::Speed, std::exception> override;
  auto set_speed(common::Speed const&) -> expected<void, std::exception> override;

  auto get_pose() -> expected<common::Pose, std::exception> override;
};

}  // namespace is