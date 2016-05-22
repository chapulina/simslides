#!/bin/bash

echo -e "\e[101mCompiling Camera Poses Plugin\e[49m"
cd plugins/camera_poses
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../../

echo -e "\e[101mCompiling Animate Model Plugin\e[49m"
cd plugins/animate_model
rm -rf build
mkdir build
cd build
cmake ..
make -j4
export GAZEBO_PLUGIN_PATH=`pwd`:$GAZEBO_PLUGIN_PATH
cd ../../../
