#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

source $DIR/setup.sh

gazebo --verbose $DIR/worlds/simslides.world
