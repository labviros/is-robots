#include "robot-gateway.hpp"
#include <boost/filesystem.hpp>
#include <string>
#include "aria-driver.hpp"

namespace fs = boost::filesystem;
using namespace is::robot;

is::pb::Status parse_json_file(std::string const& file, is::pb::Message* message) {
  std::ifstream in(file);
  std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return is::pb::JsonStringToMessage(s, message);
};

int main(int argc, char** argv) {
  std::string uri;
  std::string robot_uri;
  std::string serial_port;
  std::string power_info_file;
  unsigned int id;

  is::po::options_description opts("Options");
  opts.add_options()("uri,u", is::po::value<std::string>(&uri)->required(), "amqp broker uri");
  opts.add_options()("serial-port,s", is::po::value<std::string>(&serial_port),
                     "robot serial port");
  opts.add_options()("robot-uri,r",
                     is::po::value<std::string>(&robot_uri)->default_value("localhost:8101"),
                     "robot uri");
  opts.add_options()("power-info,p",
                     is::po::value<std::string>(&power_info_file)->default_value("power_info.json"),
                     "power info file");
  opts.add_options()("id,i", is::po::value<unsigned int>(&id)->default_value(0), "robot id");
  auto vm = is::parse_program_options(argc, argv, opts);

  std::unique_ptr<is::robot::AriaDriver> driver(new is::robot::AriaDriver());

  if (!fs::is_regular_file(fs::path(power_info_file))) {
    is::warn("\"{}\" isn't a regular file.", power_info_file);
  } else {
    PowerInfo power_info;
    if (parse_json_file(power_info_file, &power_info).ok()) {
			is::info("Power information loaded.");
			power_info.PrintDebugString();
      driver->set_power_info(power_info);
    } else {
      is::warn("Can't load power information from file \"{}\"", power_info_file);
    }
  }

  driver->set_sampling_period(is::pb::TimeUtil::MillisecondsToDuration(100));
	is::info("Connecting to robot");
	if (vm.count("serial-port"))
    driver->connect(serial_port, AriaDriver::ConnectionType::SERIAL);
  else
    driver->connect(robot_uri, AriaDriver::ConnectionType::TCP);

  is::robot::RobotGateway gateway(std::move(driver));
  gateway.run(uri, id);
}