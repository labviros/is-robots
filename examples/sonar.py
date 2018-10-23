from is_wire.core import Channel, Subscription, Message
from is_msgs.robot_pb2 import RangeScan
import json

options = json.load(open("../etc/conf/options.json"))
channel = Channel(options["broker_uri"])
subscription = Subscription(channel)
rid = options["robot_parameters"]["id"]
subscription.subscribe("RobotGateway.{}.SonarScan".format(rid))

while True:
    message = channel.consume()
    scan = message.unpack(RangeScan)
    for angle, range in zip(scan.angles, scan.ranges):
        print("{} -> {}".format(angle*180/3.14, range))
    print("==========================")
