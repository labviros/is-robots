#ifndef __ROBOT_DRIVER_HPP__
#define __ROBOT_DRIVER_HPP__

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/empty.pb.h>
#include <is/msgs/common.pb.h>
#include <is/msgs/robot.pb.h>
#include <is/msgs/power.pb.h>

namespace is {
namespace robot {

namespace pb {
using namespace google::protobuf::util;
using namespace google::protobuf;
}

using namespace is::common;
using namespace is::robot;

struct RobotDriver {
  virtual ~RobotDriver(){};

  virtual void set_speed(Speed const&) = 0;
  virtual void set_pose(Pose const&) = 0;
  virtual void set_power_info(PowerInfo const&) = 0;
  virtual Speed get_speed() = 0;
  virtual Pose get_pose() = 0;
  virtual PowerInfo get_power_info() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  void set_sampling_period(pb::Duration const& period) { sampling_period = period; };
  pb::Duration get_sampling_period() { return sampling_period; };
  pb::Duration sampling_period;
};

}  // namespace robot
}  // namespace is

#endif  // __ROBOT_DRIVER_HPP__