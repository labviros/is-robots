from is_wire.core import Channel, Subscription, Message 
from is_msgs.robot_pb2 import RobotConfig
import json
import time

options = json.load(open("../etc/conf/options.json"))
channel = Channel(options["broker_uri"])
subscription = Subscription(channel)

rid = options["robot_parameters"]["id"]

for i in range(10):
    config = RobotConfig()
    config.speed.linear = -0.2
    set_req = Message(content=config, reply_to=subscription)
    channel.publish(topic="RobotGateway.{}.SetConfig".format(rid), message=set_req)
    set_rep = channel.consume(timeout=0.05)
    print("set:", set_rep.status)

    get_req = Message(reply_to=subscription)
    channel.publish(topic="RobotGateway.{}.GetConfig".format(rid), message=get_req)
    get_rep = channel.consume(timeout=0.05)
    print("get:", get_rep.status, get_rep.unpack(RobotConfig))

    time.sleep(0.1)

config = RobotConfig()
config.speed.linear = 0.0
set_config_req = Message(content=config, reply_to=subscription)
channel.publish(topic="RobotGateway.{}.SetConfig".format(rid), message=set_config_req)
channel.consume(timeout=0.1)
