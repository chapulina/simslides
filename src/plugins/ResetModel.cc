/*
 * Copyright 2016 Louise Poubel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
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

