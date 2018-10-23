
#include "robot-gateway.hpp"
#include <algorithm>
#include <is/msgs/utils.hpp>
#include <is/wire/core/logger.hpp>

namespace is {

RobotGateway::RobotGateway(Channel const& c, RobotDriver* d, RobotParameters const& p)
    : channel(c), driver(d), params(p) {
  auto now = std::chrono::system_clock::now();
  last_speed_command = now;
  last_sampling = now;
}

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
  if (std::chrono::system_clock::now() > safety_deadline()) {
    is::warn("event=EnforceSafety.HoldLimit");
    driver->stop();
    // quick way to avoid re-warning if no command was sent to the robot
    last_speed_command += std::chrono::seconds(3600);
  }
}

void RobotGateway::publish_sensors() {
  if (std::chrono::system_clock::now() > sampling_deadline()) {
    auto sonar_scan = driver->get_sonar_scan();
    if (sonar_scan) {
      channel.publish(fmt::format("RobotGateway.{}.SonarScan", params.id()), Message{*sonar_scan});
    }
    last_sampling += std::chrono::nanoseconds(static_cast<int>(1e9 / params.sampling_rate()));
  }
}

auto RobotGateway::safety_deadline() const -> std::chrono::system_clock::time_point {
  return last_speed_command + is::to_nanoseconds(params.speed_hold_limit());
}

auto RobotGateway::sampling_deadline() const -> std::chrono::system_clock::time_point {
  return last_sampling + std::chrono::nanoseconds(static_cast<int>(1e9 / params.sampling_rate()));
}

auto RobotGateway::next_deadline() const -> std::chrono::system_clock::time_point {
  return std::min(safety_deadline(), sampling_deadline());
}

void RobotGateway::run() {
  enforce_safety();
  publish_sensors();
}

}  // namespace is
