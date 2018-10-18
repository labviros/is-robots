from is_wire.core import Channel, Subscription, Message 
from is_msgs.robot_pb2 import RobotConfig
import json

options = json.load(open("../etc/conf/options.json"))
channel = Channel(options["broker_uri"])
subscription = Subscription(channel)

rid = options["robot_parameters"]["id"]

get_config_req = Message(reply_to=subscription)
channel.publish(topic="RobotGateway.{}.GetConfig".format(rid), message=get_config_req)

get_config_rep = channel.consume(timeout=1.0)
print(get_config_rep.unpack(RobotConfig))