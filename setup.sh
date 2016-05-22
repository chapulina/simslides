DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export GAZEBO_PLUGIN_PATH=$DIR/plugins/camera_poses/build:$DIR/plugins/animate_model/build:$GAZEBO_PLUGIN_PATH

export GAZEBO_MODEL_PATH=$DIR/world/models:$GAZEBO_MODEL_PATH
