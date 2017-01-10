#!/bin/bash

# GUI plugin
cd plugins
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../

# Reset model plugin
cd plugins/reset_model
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../../

# Continuous velocity plugin
cd plugins/continuous_velocity
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../../
