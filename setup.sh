DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export GAZEBO_PLUGIN_PATH=$DIR/plugins/reset_model/build:$DIR/plugins/build:/usr/local/lib/gazebo-8/plugins/:$GAZEBO_PLUGIN_PATH

export GAZEBO_MODEL_PATH=$DIR/world/models:$DIR/models:$GAZEBO_MODEL_PATH
