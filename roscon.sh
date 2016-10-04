#!/bin/bash

source setup.sh

export IGN_PARTITION=roscon

gazebo --verbose worlds/roscon.world
