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
#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/rendering/Scene.hh>

#include <gazebo/gui/GuiEvents.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/Node.hh>
#include <gazebo/transport/Subscriber.hh>

#include <simslides/common/Common.hh>
#include "PresentMode.hh"

using namespace simslides;

class simslides::PresentModePrivate
{
  /// \brief Node used for communication.
  public: gazebo::transport::NodePtr node;

  /// \brief Subscribe to key click messages.
  public: gazebo::transport::SubscriberPtr keyboardSub;

  /// \brief Keep pointer to camera so we can move it.
  public: gazebo::rendering::UserCameraPtr camera;

  /// \brief Used to start, stop, and step simulation.
  public: gazebo::transport::PublisherPtr logPlaybackControlPub;

  /// \brief Event based connections.
  public: std::vector<gazebo::event::ConnectionPtr> connections;

  /// \brief Window mode, usually "simulation" or "LogPlayback"
  public: std::string windowMode = "simulation";
};

/////////////////////////////////////////////////
PresentMode::PresentMode() : dataPtr(new PresentModePrivate)
{
  // Keep pointer to the user camera
  this->dataPtr->camera = gazebo::gui::get_active_camera();

  // Initialize transport
  this->dataPtr->node = gazebo::transport::NodePtr(
      new gazebo::transport::Node());
  this->dataPtr->node->Init();

  this->dataPtr->keyboardSub =
      this->dataPtr->node->Subscribe("~/keyboard/keypress",
      &PresentMode::OnKeyPress, this, true);

  // Connections
  this->dataPtr->connections.push_back(
      gazebo::gui::Events::ConnectWindowMode(
      std::bind(&PresentMode::OnWindowMode, this, std::placeholders::_1)));

  simslides::Common::Instance()->moveCamera =
      std::bind(&PresentMode::OnMoveCamera, this, std::placeholders::_1);

  simslides::Common::Instance()->setVisualVisible =
      std::bind(&PresentMode::OnSetVisualVisible, this, std::placeholders::_1,
      std::placeholders::_2);

  simslides::Common::Instance()->seekLog =
      std::bind(&PresentMode::OnSeekLog, this, std::placeholders::_1);

  simslides::Common::Instance()->initialCameraPose =
      std::bind(&PresentMode::OnInitialCameraPose, this);

  simslides::Common::Instance()->visualPose =
      std::bind(&PresentMode::OnVisualPose, this, std::placeholders::_1);

  gzmsg << "Start presentation. Total of [" << simslides::keyframes.size()
        << "] slides" << std::endl;

  // Trigger first slide
  simslides::currentKeyframe = 0;
  this->ChangeSlide();
}

/////////////////////////////////////////////////
PresentMode::~PresentMode()
{
}

/////////////////////////////////////////////////
void PresentMode::OnWindowMode(const std::string &_mode)
{
  this->dataPtr->windowMode = _mode;
}

/////////////////////////////////////////////////
void PresentMode::OnKeyPress(ConstAnyPtr &_msg)
{
  simslides::HandleKeyPress(_msg->int_value());
  this->ChangeSlide();
}

/////////////////////////////////////////////////
void PresentMode::OnSlideChanged(int _keyframe)
{
  simslides::ChangeKeyframe(_keyframe);
  this->ChangeSlide();
}

/////////////////////////////////////////////////
void PresentMode::ChangeSlide()
{
  gzmsg << "Change Slide: " << simslides::currentKeyframe << std::endl;

  ignition::math::Pose3d camPose;
  ignition::math::Pose3d eyeOff;
  std::string toLookAt;
  std::string text;

  // Reset presentation
  if (simslides::currentKeyframe < 0)
  {
    camPose = simslides::Common::Instance()->initialCameraPose();
  }
  // Slides
  else
  {
    auto keyframe = simslides::keyframes[simslides::currentKeyframe];

    text = keyframe->Text();

    if (keyframe->GetType() == KeyframeType::LOOKAT ||
        keyframe->GetType() == KeyframeType::STACK)
    {
      toLookAt = keyframe->Visual();

      eyeOff = keyframe->EyeOffset();
    }

    if (keyframe->GetType() == KeyframeType::STACK)
    {
      // Find stack front
      auto frontKeyframe = simslides::currentKeyframe;
      while (frontKeyframe > 0 &&
          simslides::keyframes[frontKeyframe-1]->GetType() == KeyframeType::STACK)
      {
        frontKeyframe--;
      }

      // Find stack back
      auto backKeyframe = simslides::currentKeyframe;
      while (backKeyframe + 1 < simslides::keyframes.size() &&
          simslides::keyframes[backKeyframe+1]->GetType() == KeyframeType::STACK)
      {
        backKeyframe++;
      }

      gzmsg << "Stack front [" << frontKeyframe << "], back [" << backKeyframe
            << "]" << std::endl;

      // Scale down the other slides in the stack
      for (int i = frontKeyframe; i <= backKeyframe; ++i)
      {
        auto name = simslides::keyframes[i]->Visual();
        simslides::Common::Instance()->setVisualVisible(name,
            name == keyframe->Visual());
      }
    }

    if (keyframe->GetType() == KeyframeType::LOG_SEEK)
    {
      camPose = keyframe->CamPose();
      simslides::Common::Instance()->seekLog(keyframe->LogSeek());
    }

    if (keyframe->GetType() == KeyframeType::CAM_POSE)
    {
      camPose = keyframe->CamPose();
    }
  }

  if (!toLookAt.empty())
  {
    // Target in world frame
    auto origin = simslides::Common::Instance()->visualPose(toLookAt);

    auto bb_pos = origin.Pos() + ignition::math::Vector3d(0, 0, 0.5);
    auto target_world = ignition::math::Matrix4d(ignition::math::Pose3d(
        bb_pos, origin.Rot()));

    // Eye in target frame
    if (eyeOff == ignition::math::Pose3d::Zero)
    {
      eyeOff = ignition::math::Pose3d(
              simslides::kEyeOffsetX,
              simslides::kEyeOffsetY,
              simslides::kEyeOffsetZ,
              simslides::kEyeOffsetRoll,
              simslides::kEyeOffsetPitch,
              simslides::kEyeOffsetYaw);
    }
    ignition::math::Matrix4d eye_target(eyeOff);

    // Eye in world frame
    auto eye_world = target_world * eye_target;

    // Look At
    auto mat = ignition::math::Matrix4d::LookAt(eye_world.Translation(),
        target_world.Translation());

    camPose = mat.Pose();
  }

  simslides::Common::Instance()->moveCamera(camPose);

  this->SlideChanged(simslides::currentKeyframe, simslides::keyframes.size()-1,
      QString::fromStdString(text));
}

/////////////////////////////////////////////////
void PresentMode::OnMoveCamera(const ignition::math::Pose3d &_pose)
{
  // Don't bother moving just a mm
  if ((this->dataPtr->camera->WorldPose().Pos() - _pose.Pos()).Length() < 0.001)
    return;

  this->dataPtr->camera->MoveToPosition(_pose, 1);
}

/////////////////////////////////////////////////
void PresentMode::OnSetVisualVisible(const std::string &_name, bool _visible)
{
  auto vis = this->dataPtr->camera->GetScene()->GetVisual(_name);

  if (!vis)
  {
    gzerr << "Couldn't find visual [" << _name << "]" << std::endl;
    return;
  }

  vis->SetVisible(_visible);
}

/////////////////////////////////////////////////
void PresentMode::OnSeekLog(std::chrono::steady_clock::duration _time)
{
  // Advertise
  if (!this->dataPtr->logPlaybackControlPub &&
      this->dataPtr->windowMode == "LogPlayback")
  {
    this->dataPtr->logPlaybackControlPub = this->dataPtr->node->
        Advertise<gazebo::msgs::LogPlaybackControl>("~/playback_control");
  }

  if (this->dataPtr->logPlaybackControlPub)
  {
    auto s = std::chrono::duration_cast<std::chrono::seconds>(_time);
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(_time-s);

    gazebo::msgs::LogPlaybackControl msg;
    gazebo::msgs::Set(msg.mutable_seek(),
        gazebo::common::Time(s.count(), ns.count()));
    msg.set_pause(false);
    this->dataPtr->logPlaybackControlPub->Publish(msg);
  }
}

/////////////////////////////////////////////////
ignition::math::Pose3d PresentMode::OnInitialCameraPose()
{
  return this->dataPtr->camera->InitialPose();
}

/////////////////////////////////////////////////
ignition::math::Pose3d PresentMode::OnVisualPose(const std::string &_name)
{
  auto vis = this->dataPtr->camera->GetScene()->GetVisual(_name);
  if (!vis)
  {
    gzerr << "Failed to find visual [" << _name << "]" << std::endl;
    return {
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN()
    };
  }

  return vis->WorldPose();
}


