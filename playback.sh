#!/bin/bash

export GAZEBO_MASTER_URI=http://localhost:11346

gazebo --verbose -u -p assets/state.log
