
#include <google/protobuf/empty.pb.h>
#include <chrono>
#include <is/msgs/utils.hpp>
#include <is/wire/core.hpp>
#include <is/wire/rpc.hpp>
#include <is/wire/rpc/log-interceptor.hpp>
#include "conf/options.pb.h"
#include "is/robot-drivers/aria/aria-driver.hpp"
#include "robot-gateway.hpp"

auto load_configuration(int argc, char** argv) -> is::RobotGatewayOptions {
  auto filename = (argc == 2) ? argv[1] : "options.json";
  auto options = is::RobotGatewayOptions{};
  is::load(filename, &options);
  is::validate_message(options);
  return options;
}

int main(int argc, char** argv) {
  auto options = load_configuration(argc, argv);
  auto service = fmt::format("RobotGateway.{}", options.robot_parameters().id());

  auto driver = is::AriaDriver{options.robot_parameters().robot_uri()};
  auto gateway = is::RobotGateway{&driver, options.robot_parameters()};
  is::info("event=RobotInitDone");

  auto channel = is::Channel{options.broker_uri()};
  is::info("event=ChannelInitDone");

  auto server = is::ServiceProvider{channel};
  auto logs = is::LogInterceptor{};
  server.add_interceptor(logs);
  server.delegate<is::robot::RobotConfig, google::protobuf::Empty>(
      service + ".SetConfig", [&](auto* ctx, auto const& config, auto*) {
        gateway.set_configuration(config);
        return is::make_status();
      });

  server.delegate<google::protobuf::Empty, is::robot::RobotConfig>(
      service + ".GetConfig", [&](auto* ctx, auto const&, auto* config) {
        *config = *gateway.get_configuration();
        return is::make_status();
      });

  is::info("event=InitAllDone");
  for (;;) {
    auto message = channel.consume_until(gateway.next_deadline());
    if (message) { server.serve(*message); }
    gateway.enforce_safety();
  }
}