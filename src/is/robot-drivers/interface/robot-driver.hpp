
#pragma once

#include <is/msgs/common.pb.h>
#include <tl/expected.hpp>

namespace is {

using namespace tl;

struct RobotDriver {
  virtual ~RobotDriver() = default;

  virtual auto stop() -> expected<void, std::exception> = 0;
  virtual auto get_speed() -> expected<common::Speed, std::exception> = 0;
  virtual auto set_speed(common::Speed const&) -> expected<void, std::exception> = 0;

  virtual auto get_pose() -> expected<common::Pose, std::exception> = 0;
};

}  // namespace is