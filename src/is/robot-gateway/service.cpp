#include <google/protobuf/empty.pb.h>
#include <zipkin/opentracing.h>
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

auto create_tracer(std::string const& name, std::string const& uri)
    -> std::shared_ptr<opentracing::Tracer> {
  std::smatch match;
  auto ok = std::regex_match(uri, match, std::regex("http:\\/\\/([a-zA-Z0-9\\.]+)(:(\\d+))?"));
  if (!ok) is::critical("Invalid zipkin uri \"{}\", expected http://<hostname>:<port>", uri);
  auto tracer_options = zipkin::ZipkinOtTracerOptions{};
  tracer_options.service_name = name;
  tracer_options.collector_host = match[1];
  tracer_options.collector_port = match[3].length() ? std::stoi(match[3]) : 9411;
  return zipkin::makeZipkinOtTracer(tracer_options);
}

int main(int argc, char** argv) {
  auto options = load_configuration(argc, argv);
  auto service = fmt::format("RobotGateway.{}", options.robot_parameters().id());

  auto channel = is::Channel{options.broker_uri()};
  auto tracer = create_tracer(service, options.zipkin_uri());
  channel.set_tracer(tracer);
  is::info("event=ChannelInitDone");

  auto driver = is::AriaDriver{options.robot_parameters().robot_uri()};
  is::info("event=RobotInitDone");

  auto gateway = is::RobotGateway{channel, &driver, options.robot_parameters()};

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
    gateway.run();
  }
}
