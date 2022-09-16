/*
 * Copyright (C) 2020 Louise Poubel
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

#include <iostream>
#include <limits>

#include <gz/msgs/gui_camera.pb.h>
#include <gz/msgs/boolean.pb.h>
#include <tinyxml2.h>

#include <gz/common/Console.hh>
#include <gz/gui/Application.hh>
#include <gz/gui/GuiEvents.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/math/Pose3.hh>
#include <gz/plugin/Register.hh>
#include <gz/rendering/RenderEngine.hh>
#include <gz/rendering/RenderingIface.hh>
#include <simslides/common/Common.hh>
#include <sdf/parser.hh>

#include "SimSlidesGz.hh"

using namespace simslides;

/////////////////////////////////////////////////
SimSlidesGz::SimSlidesGz()
  : Plugin()
{
}

/////////////////////////////////////////////////
SimSlidesGz::~SimSlidesGz()
{
}

/////////////////////////////////////////////////
void SimSlidesGz::LoadConfig(const tinyxml2::XMLElement *_pluginXml)
{
  if (this->title.empty())
    this->title = "SimSlides";

  // Load SDF
  tinyxml2::XMLPrinter printer;
  _pluginXml->Accept(&printer);

  std::ostringstream stream;
  stream
    << "<sdf version='" << SDF_VERSION << "'>"
    << printer.CStr()
    << "</sdf>";

  auto sdfParsed = std::make_shared<sdf::SDF>();
  sdf::init(sdfParsed);
  sdf::readString(stream.str(), sdfParsed);

  auto pluginElem = sdfParsed->Root()->GetElement("plugin");
  if (nullptr == pluginElem)
  {
    gzerr << "Error getting plugin element from:" << std::endl << stream.str()
           << std::endl;
    return;
  }

  Common::Instance()->LoadPluginSDF(pluginElem);

  this->node.Subscribe("/keyboard/keypress", &SimSlidesGz::OnKeyPress, this);

//      this->logPlaybackControlPub = this->node->
//          Advertise<gazebo::msgs::LogPlaybackControl>("~/playback_control");

  gz::gui::App()->findChild<gz::gui::MainWindow *>
      ()->installEventFilter(this);

  simslides::Common::Instance()->MoveCamera =
      std::bind(&SimSlidesGz::OnMoveCamera, this, std::placeholders::_1);

  simslides::Common::Instance()->SetVisualVisible =
      std::bind(&SimSlidesGz::OnSetVisualVisible, this, std::placeholders::_1,
      std::placeholders::_2);

  simslides::Common::Instance()->SeekLog =
      std::bind(&SimSlidesGz::OnSeekLog, this, std::placeholders::_1);

  simslides::Common::Instance()->ResetCameraPose =
      std::bind(&SimSlidesGz::OnResetCameraPose, this);

  simslides::Common::Instance()->VisualPose =
      std::bind(&SimSlidesGz::OnVisualPose, this, std::placeholders::_1);

  simslides::Common::Instance()->SetText =
      std::bind(&SimSlidesGz::OnSetText, this, std::placeholders::_1);

  gzmsg << "Start presentation. Total of [" << Common::Instance()->keyframes.size()
        << "] keyframes" << std::endl;

  // Trigger first slide
  Common::Instance()->currentKeyframe = 0;
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
bool SimSlidesGz::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == gz::gui::events::Render::kType)
  {
    this->LoadScene();
    this->ProcessCommands();
  }
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void SimSlidesGz::LoadScene()
{
  if (nullptr != this->scene)
    return;

  this->scene = gz::rendering::sceneFromFirstRenderEngine();
  if (nullptr == this->scene)
    return;

  for (int i = 0; i < this->scene->NodeCount(); ++i)
  {
    auto cam = std::dynamic_pointer_cast<gz::rendering::Camera>(
        this->scene->NodeByIndex(i));
    if (nullptr != cam)
    {
      this->camera = cam;

      if (!std::isnan(Common::Instance()->farClip) && !std::isnan(Common::Instance()->nearClip))
      {
        camera->SetNearClipPlane(Common::Instance()->nearClip);
        camera->SetFarClipPlane(Common::Instance()->farClip);
      }

      // Match Gazebo Classic's user camera FOV so the "zoom" looks the same
      camera->SetHFOV(GZ_DTOR(60));

      gzdbg << "SimSlides attached to camera ["
             << this->camera->Name() << "]" << std::endl;
      break;
    }
  }

  if (!this->camera)
  {
    gzerr << "Camera is not available" << std::endl;
  }
}

/////////////////////////////////////////////////
void SimSlidesGz::OnKeyframeChanged(int _keyframe)
{
  Common::Instance()->ChangeKeyframe(_keyframe);
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
void SimSlidesGz::ProcessCommands()
{
  if (!this->pendingCommand)
    return;

  this->pendingCommand = false;

  gzmsg << "Changing to slide [" << Common::Instance()->currentKeyframe << "]"
         << std::endl;

  simslides::Common::Instance()->Update();

  this->updateGUI(Common::Instance()->currentKeyframe, Common::Instance()->keyframes.size() - 1);
}

/////////////////////////////////////////////////
void SimSlidesGz::OnKeyPress(const gz::msgs::Int32 &_msg)
{
  Common::Instance()->HandleKeyPress(_msg.data());
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
void SimSlidesGz::OnMoveCamera(const gz::math::Pose3d &_pose)
{
  if (nullptr == this->camera)
  {
    gzerr << "No camera, failed to move camera." << std::endl;
    return;
  }

  // Don't bother moving just a mm
  if ((this->camera->WorldPose().Pos() - _pose.Pos()).Length() < 0.001)
    return;

  gz::msgs::GUICamera req;
  gz::msgs::Set(req.mutable_pose(), _pose);

  std::function<void(const gz::msgs::Boolean &, const bool)> cb =
      [](const gz::msgs::Boolean &_res, const bool _result)
  {
    if (!_result)
    {
      gzerr << "Timed out requesting to move camera" << std::endl;
    }
    if (!_res.data())
    {
      gzerr << "Failed to move camera" << std::endl;
    }
  };

  this->node.Request("/gui/move_to/pose", req, cb);
}

/////////////////////////////////////////////////
void SimSlidesGz::OnSetVisualVisible(const std::string &_name, bool _visible)
{
  if (nullptr == this->camera)
  {
    gzerr << "No scene, failed to set visual visibility." << std::endl;
    return;
  }

  auto vis = this->scene->VisualByName(_name);

  if (!vis)
  {
    gzerr << "Couldn't find visual [" << _name << "]" << std::endl;
    return;
  }

  vis->SetVisible(_visible);
}

/////////////////////////////////////////////////
void SimSlidesGz::OnSeekLog(std::chrono::steady_clock::duration _time)
{
  std::cout << "Log seek not supported yet" << std::endl;
  // if (!this->logPlaybackControlPub)
  // {
  //   this->logPlaybackControlPub = this->node->
  //       Advertise<gazebo::msgs::LogPlaybackControl>("~/playback_control");
  // }

  // if (this->logPlaybackControlPub)
  // {
  //   auto s = std::chrono::duration_cast<std::chrono::seconds>(_time);
  //   auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(_time-s);

  //   gazebo::msgs::LogPlaybackControl msg;
  //   gazebo::msgs::Set(msg.mutable_seek(),
  //       gazebo::common::Time(s.count(), ns.count()));
  //   msg.set_pause(false);
  //   this->logPlaybackControlPub->Publish(msg);
  // }
}

/////////////////////////////////////////////////
void SimSlidesGz::OnResetCameraPose()
{
  gz::msgs::Vector3d req;

  std::function<void(const gz::msgs::Boolean &, const bool)> cb =
      [](const gz::msgs::Boolean &_res, const bool _result)
  {
    if (!_result)
    {
      gzerr << "Timed out requesting to reset camera" << std::endl;
    }
    if (!_res.data())
    {
      gzerr << "Failed to reset camera" << std::endl;
    }
  };

  this->node.Request("/gui/view_angle", req, cb);
}

/////////////////////////////////////////////////
gz::math::Pose3d SimSlidesGz::OnVisualPose(const std::string &_name)
{
  if (nullptr == this->camera)
  {
    gzerr << "No camera, failed to get visual pose." << std::endl;
    return {
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN()
    };
  }

  auto vis = this->camera->Scene()->VisualByName(_name);
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

  // Target in world frame
  return vis->WorldPose();
}

/////////////////////////////////////////////////
void SimSlidesGz::OnSetText(const std::string &_name)
{
  // TODO(louise) Support setting text
}

// Register this plugin
GZ_ADD_PLUGIN(simslides::SimSlidesGz,
                    gz::gui::Plugin);
