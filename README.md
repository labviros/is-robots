Robot Gateways
==========

This repository contains the base implementation to build drivers and gateways for robots.

Dependencies:
-----
This service does not depend on any other service.

Events:
--------
<img width=600/> ⇒ Triggered By | <img width=600/> Triggers ⇒ | <img width=200/> Description  
:------------ | :-------- | :----------
:incoming_envelope: **topic:** `RobotGateway.{id}.SetConfig` <br> :gem: **schema:** [RobotConfig] | :incoming_envelope: **topic:** `{request.reply_to}` <br> :gem: **schema:** Empty | `Configure robot parameters, like current speed.`
:incoming_envelope: **topic:** `RobotGateway.{id}.GetConfig` <br> :gem: **schema:** Empty | :incoming_envelope: **topic:** `{request.reply_to}` <br> :gem: **schema:** [RobotConfig] | `Get the current robot configuration.`
:clock5: **interval:** `{config.sampling_rate}` | :incoming_envelope: **topic:** `RobotGateway.{id}.SonarScan` <br> :gem: **schema:** [RangeScan] | `Periodically publishes the readings from the robot sonars.`


[RobotConfig]: https://github.com/labviros/is-msgs/tree/master/docs#is.robot.RobotConfig
[RangeScan]: https://github.com/labviros/is-msgs/tree/master/docs#is.robot.RangeScan


Configuration:
----------------
The behavior of the service can be customized by passing a JSON configuration file as the first argument, e.g: `./service config.json`. The schema and documentation for this file can be found in [`src/is/robot-gateway/conf/options.proto`](src/is/robot-gateway/conf/options.proto). An example configuration file can be found in [`etc/conf/options.json`](etc/conf/options.json).


Examples:
------------
Examples on how to configure the robot speed and read sensors are provided in [`examples/`](examples/) directory.