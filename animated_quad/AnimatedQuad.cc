/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
 *
*/
#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <ignition/math.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <stdio.h>

namespace gazebo
{
  class AnimatedQuad : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      // Get input params from SDF
      if (!_sdf->HasElement("pose"))
	return;

      // create the animation
      gazebo::common::PoseAnimationPtr anim(
	    // name the animation "test",
	    // make it last 20 seconds,
	    // and set it on a repeat loop
	    new gazebo::common::PoseAnimation("test", 20.0, true));

      gazebo::common::PoseKeyFrame *key;

      int time = 0;
      sdf::ElementPtr poseElem = _sdf->GetElement("pose");
      while (poseElem)
      {
	ignition::math::Pose3d pose = poseElem->Get<ignition::math::Pose3d>();

        // Location after "time"
	key = anim->CreateKeyFrame(time);
	key->Translation(pose.Pos());
	key->Rotation(pose.Rot());

        time += 5;
	poseElem = poseElem->GetNextElement("pose");
      }

      // set the animation
      _model->SetAnimation(anim);
    }
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(AnimatedQuad)
}
