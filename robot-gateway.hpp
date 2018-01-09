#ifndef __ROBOT_GATEWAY_HPP__
#define __ROBOT_GATEWAY_HPP__

#include <chrono>
#include <is/is.hpp>
#include "robot-driver.hpp"

namespace is {
namespace robot {

using namespace std::chrono;
using namespace is::common;
using namespace is::robot;

void wait(is::pb::Duration const& duration) {
  std::this_thread::sleep_for(microseconds(is::pb::TimeUtil::DurationToMicroseconds(duration)));
}

struct RobotGateway {
  std::unique_ptr<RobotDriver> driver;

  RobotGateway(std::unique_ptr<RobotDriver> impl) : driver(std::move(impl)) {}

  Status get_configuration(RobotConfig* config) {
    try {
      *config->mutable_speed() = driver->get_speed();
      return make_status(StatusCode::OK);
    } catch (Status const& status) { return status; }
  }

  Status set_configuration(RobotConfig const& config) {
    try {
      driver->set_speed(config.speed());
      return make_status(StatusCode::OK);
    } catch (Status const& status) { return status; }
  }

  void run(std::string const& uri, unsigned int const& id, is::Tracer tracer) {
    is::info("Trying to connect to {}", uri);
    auto channel = rmq::Channel::CreateFromUri(uri);
    is::ServiceProvider provider;
    is::RPCTraceInterceptor interceptor(provider, tracer);

    provider.connect(channel);
    auto queue = provider.declare_queue("RobotGateway", std::to_string(id));

    provider.delegate<RobotConfig, pb::Empty>(
        queue, "SetConfig", [this](RobotConfig const& config, pb::Empty*) -> Status {
          return this->set_configuration(config);
        });
    provider.delegate<pb::Empty, RobotConfig>(
        queue, "GetConfig", [this](pb::Empty const&, RobotConfig* config) -> Status {
          return this->get_configuration(config);
        });

    driver->start();
    auto period = driver->get_sampling_period();
    auto now = is::current_time();
    for (;;) {
      auto pose = driver->get_pose();
      is::publish(channel, fmt::format("RobotGateway.{}.Pose", id), pose);

      now = now + period;
      auto deadline = now - is::pb::TimeUtil::MillisecondsToDuration(1);     
      for (;;) {
        auto envelope = is::consume_until(channel, deadline);
        if (envelope != nullptr)
          provider.serve(envelope);
        else
          break;
      }
      wait(now - is::current_time());
    }
  }

};  // RobotGateway

}  // namespace robot
}  // namespace is

#endif  // __ROBOT_GATEWAY_HPP__