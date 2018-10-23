
#pragma once

#ifndef isnan
  #if __cplusplus <= 199711L  // c++98 or older
    #define isnan(x) ::isnan(x)
  #else
    #define isnan(x) std::isnan(x)
  #endif
#endif

#ifndef isfinite
  #if __cplusplus <= 199711L  // c++98 or older
    #define isfinite(x) ::isfinite(x)
  #else
    #define isfinite(x) std::isfinite(x)
  #endif
#endif

#include <Aria/Aria.h>
#include <regex>
#include "is/robot-drivers/interface/robot-driver.hpp"

namespace is {

class AriaDriver : public RobotDriver {
  ArRobot robot;
  ArSonarDevice sonar;
  ArSick laser;

 public:
  AriaDriver(std::string const& uri);
  virtual ~AriaDriver();

  auto stop() -> expected<void, std::exception> override;
  auto get_speed() -> expected<common::Speed, std::exception> override;
  auto set_speed(common::Speed const&) -> expected<void, std::exception> override;

  auto get_pose() -> expected<common::Pose, std::exception> override;
  auto get_sonar_scan() -> expected<robot::RangeScan, std::exception> override;
};

}  // namespace is