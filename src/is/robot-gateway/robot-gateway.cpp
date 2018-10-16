
#include "robot-gateway.hpp"
#include <is/wire/core/logger.hpp>

namespace is {

RobotGateway::RobotGateway(RobotDriver*) {}

auto RobotGateway::get_configuration() -> expected<robot::RobotConfig, std::exception> {
  auto config = robot::RobotConfig{};
  *config.mutable_speed() = *(driver->get_speed());
}

auto RobotGateway::set_configuration(robot::RobotConfig const& config)
    -> expected<void, std::exception> {
  if (config.has_speed()) {
    driver->set_speed(config.speed());
    last_speed_command = std::chrono::system_clock::now();
  }
  return {};
}

void RobotGateway::enforce_safety(std::chrono::nanoseconds const& hold_limit) {
  if (std::chrono::system_clock::now() - last_speed_command > hold_limit) {
    is::warn("event=EnforceSafety.HoldLimit");
    driver->stop();
    // quick way to avoid re-warning if no command was sent to the robot
    last_speed_command += std::chrono::seconds(3600);
  }
}

}  // namespace is
