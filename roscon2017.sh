#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

source $DIR/setup.sh

gazebo --verbose -u -p $DIR/worlds/roscon2017.log
