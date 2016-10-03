#!/bin/bash

export GAZEBO_MASTER_URI=http://localhost:11346

export IGN_PARTITION=playback

gazebo --verbose -u -p assets/state.log
