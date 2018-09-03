DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export GAZEBO_PLUGIN_PATH=$DIR/build/src:$DIR/build/src:/usr/local/lib/gazebo-9/plugins/:/usr/local/lib/x86_64-linux-gnu/gazebo-9/plugins/:$GAZEBO_PLUGIN_PATH

export GAZEBO_MODEL_PATH=$DIR/worlds/models:$DIR/models:$GAZEBO_MODEL_PATH
