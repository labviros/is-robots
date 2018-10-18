
#include "robot-gateway.hpp"
#include <is/msgs/utils.hpp>
#include <is/wire/core/logger.hpp>

namespace is {

RobotGateway::RobotGateway(RobotDriver* d, RobotParameters const& p)
    : driver(d), params(p), last_speed_command(std::chrono::system_clock::now()) {}

auto RobotGateway::get_configuration() -> expected<robot::RobotConfig, std::exception> {
  auto config = robot::RobotConfig{};
  *config.mutable_speed() = *(driver->get_speed());
  return config;
}

auto RobotGateway::set_configuration(robot::RobotConfig const& config)
    -> expected<void, std::exception> {
  if (config.has_speed()) {
    driver->set_speed(config.speed());
    last_speed_command = std::chrono::system_clock::now();
  }
  return {};
}

void RobotGateway::enforce_safety() {
  if (std::chrono::system_clock::now() > next_deadline()) {
    is::warn("event=EnforceSafety.HoldLimit");
    driver->stop();
    // quick way to avoid re-warning if no command was sent to the robot
    last_speed_command += std::chrono::seconds(3600);
  }
}

auto RobotGateway::next_deadline() const -> std::chrono::system_clock::time_point {
  return last_speed_command + is::to_nanoseconds(params.speed_hold_limit());
}

}  // namespace is
