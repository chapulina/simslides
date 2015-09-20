#!/bin/bash

echo "Compiling Camera Poses Plugin"
cd camera_poses
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../

echo "Compiling Animated Quad Plugin"
cd animated_quad
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../

echo "Generating slide world..."
sh generate_models.sh
