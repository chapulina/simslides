DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. /usr/local/share/gazebo-8/setup.sh

export GAZEBO_PLUGIN_PATH=$DIR/plugins/build:$DIR/plugins/reset_model/build:/usr/local/lib/gazebo-8/plugins/:/usr/local/lib/x86_64-linux-gnu/gazebo-8/plugins/:$GAZEBO_PLUGIN_PATH

export GAZEBO_MODEL_PATH=$DIR/worlds/models:$DIR/models:$DIR/../servicesim/servicesim_competition/models:$GAZEBO_MODEL_PATH

export GAZEBO_RESOURCE_PATH=$DIR:$DIR/../servicesim/servicesim_competition:$GAZEBO_RESOURCE_PATH
