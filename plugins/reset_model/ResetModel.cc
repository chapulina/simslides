#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <stdio.h>

namespace gazebo
{
  class ResetModel : public ModelPlugin
  {
    /////////////////////////////////////////////////
    public: void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Get input params from SDF
      if (!_sdf->HasElement("key"))
      {
        gzerr << "Missing <key>" << std::endl;
        return;
      }

      this->key = _sdf->Get<int>("key");
      this->model = _model;

      // Initialize transport
      this->node = gazebo::transport::NodePtr(
          new gazebo::transport::Node());
      this->node->Init();

      this->keyboardSub =
          this->node->Subscribe("/gazebo/default/keyboard/keypress",
          &ResetModel::OnKeyPress, this, true);
    }

    /////////////////////////////////////////////////
    void OnKeyPress(ConstAnyPtr &_msg)
    {
      if (_msg->int_value() != this->key)
        return;

      model->Reset();
    }

    private: int key;
    private: gazebo::physics::ModelPtr model;
    private: gazebo::transport::NodePtr node;
    private: gazebo::transport::SubscriberPtr keyboardSub;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(ResetModel)
}

