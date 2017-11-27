#include <is/is.hpp>

#include <is/msgs/common.pb.h>
#include <is/msgs/robot.pb.h>
#include <is/msgs/power.pb.h>

using namespace is;
using namespace is::robot;
using namespace is::common;

int main(int, char**) {
  auto uri = "amqp://rmq.is:30000";
  auto channel = rmq::Channel::CreateFromUri(uri);
  auto tag = is::declare_queue(channel);

  RobotConfig config;
  config.mutable_speed()->set_linear(0.1);
  config.mutable_speed()->set_angular(5.0 * std::asin(1) / 90.0);
  config.PrintDebugString();
  
  auto msg = is::pack_proto(config);
  msg->ReplyTo(tag);
  channel->BasicPublish("is", "RobotGateway.0.SetConfig", msg);

  is::subscribe(channel, tag, "RobotGateway.0.Pose");
  while (1) {
    auto envelope = channel->BasicConsumeMessage(tag);
    auto pose = is::unpack<Pose>(envelope);
    if (pose) {
      is::info("Pose: {},{},{}", pose->position().x(), pose->position().y(),
               pose->orientation().roll() * 90.0 / std::asin(1));
    }
  }
}
