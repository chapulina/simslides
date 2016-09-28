DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export GAZEBO_PLUGIN_PATH=$DIR/plugins/build:/usr/local/lib/gazebo-8/plugins/:/usr/local/lib/x86_64-linux-gnu/gazebo-8/plugins/:$GAZEBO_PLUGIN_PATH

export GAZEBO_MODEL_PATH=$DIR/worlds/models:$DIR/models:$GAZEBO_MODEL_PATH
