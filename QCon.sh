#!/bin/bash

gazebo --verbose worlds/2018_QCon.world &

ros2 run follow follow cmd_vel:=/dolly/cmd_vel laser_scan:=/dolly/laser_scan

