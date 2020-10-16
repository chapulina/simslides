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

#include <ignition/msgs/gui_camera.pb.h>
#include <ignition/msgs/boolean.pb.h>
#include <tinyxml2.h>

#include <ignition/common/Console.hh>
#include <ignition/gazebo/gui/GuiEvents.hh>
#include <ignition/gui/Application.hh>
#include <ignition/gui/MainWindow.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <simslides/common/Common.hh>
#include <sdf/parser.hh>

#include "SimSlidesIgn.hh"

using namespace simslides;

/////////////////////////////////////////////////
SimSlidesIgn::SimSlidesIgn()
  : Plugin()
{
}

/////////////////////////////////////////////////
SimSlidesIgn::~SimSlidesIgn()
{
}

/////////////////////////////////////////////////
void SimSlidesIgn::LoadConfig(const tinyxml2::XMLElement *_pluginXml)
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
    ignerr << "Error getting plugin element from:" << std::endl << stream.str()
           << std::endl;
    return;
  }

  simslides::LoadPluginSDF(pluginElem);

  this->node.Subscribe("/keyboard/keypress", &SimSlidesIgn::OnKeyPress, this);

//      this->logPlaybackControlPub = this->node->
//          Advertise<gazebo::msgs::LogPlaybackControl>("~/playback_control");

  ignition::gui::App()->findChild<ignition::gui::MainWindow *>
      ()->installEventFilter(this);

  simslides::Common::Instance()->moveCamera =
      std::bind(&SimSlidesIgn::OnMoveCamera, this, std::placeholders::_1);

  simslides::Common::Instance()->setVisualVisible =
      std::bind(&SimSlidesIgn::OnSetVisualVisible, this, std::placeholders::_1,
      std::placeholders::_2);

  simslides::Common::Instance()->seekLog =
      std::bind(&SimSlidesIgn::OnSeekLog, this, std::placeholders::_1);

  simslides::Common::Instance()->initialCameraPose =
      std::bind(&SimSlidesIgn::OnInitialCameraPose, this);

  simslides::Common::Instance()->visualPose =
      std::bind(&SimSlidesIgn::OnVisualPose, this, std::placeholders::_1);

  simslides::Common::Instance()->SetText =
      std::bind(&SimSlidesIgn::OnSetText, this, std::placeholders::_1);

  ignmsg << "Start presentation. Total of [" << simslides::keyframes.size()
        << "] keyframes" << std::endl;

  // Trigger first slide
  simslides::currentKeyframe = 0;
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
bool SimSlidesIgn::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == ignition::gazebo::gui::events::Render::kType)
  {
    this->LoadScene();
    this->ProcessCommands();
  }
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void SimSlidesIgn::LoadScene()
{
  if (nullptr != this->scene)
    return;

  auto loadedEngNames = ignition::rendering::loadedEngines();
  if (loadedEngNames.empty())
    return;

  auto engineName = loadedEngNames[0];
  if (loadedEngNames.size() > 1)
  {
    igndbg << "More than one engine is available. "
      << "Using engine [" << engineName << "]" << std::endl;
  }
  auto engine = ignition::rendering::engine(engineName);
  if (!engine)
  {
    ignerr << "Internal error: failed to load engine [" << engineName
      << "]." << std::endl;
    return;
  }

  if (engine->SceneCount() == 0)
    return;

  // assume there is only one scene
  auto scene = engine->SceneByIndex(0);
  if (!scene)
  {
    ignerr << "Internal error: scene is null." << std::endl;
    return;
  }

  if (!scene->IsInitialized() || scene->VisualCount() == 0)
  {
    return;
  }
  this->scene = scene;

  for (int i = 0; i < this->scene->NodeCount(); ++i)
  {
    auto cam = std::dynamic_pointer_cast<ignition::rendering::Camera>(
        this->scene->NodeByIndex(i));
    if (nullptr != cam)
    {
      this->camera = cam;

      if (!std::isnan(simslides::farClip) && !std::isnan(simslides::nearClip))
      {
        camera->SetNearClipPlane(simslides::nearClip);
        camera->SetFarClipPlane(simslides::farClip);
      }
      break;
    }
  }
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnSlideChanged(int _keyframe)
{
  simslides::ChangeKeyframe(_keyframe);
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
void SimSlidesIgn::ProcessCommands()
{
  if (!this->pendingCommand)
    return;

  this->pendingCommand = false;

  if (simslides::currentKeyframe < 0 || simslides::currentKeyframe >= simslides::keyframes.size())
  {
    ignerr << "Keyframe [" << simslides::currentKeyframe << "] not found" << std::endl;
    return;
  }

  ignmsg << "Changing to slide [" << simslides::currentKeyframe << "]"
         << std::endl;

  simslides::Common::Instance()->Update();

  this->updateGUI(simslides::currentKeyframe, simslides::keyframes.size() - 1);
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnKeyPress(const ignition::msgs::Int32 &_msg)
{
  simslides::HandleKeyPress(_msg.data());
  this->pendingCommand = true;
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnMoveCamera(const ignition::math::Pose3d &_pose)
{
  // Don't bother moving just a mm
  if ((this->camera->WorldPose().Pos() - _pose.Pos()).Length() < 0.001)
    return;

  ignition::msgs::GUICamera req;
  ignition::msgs::Set(req.mutable_pose(), _pose);

  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [](const ignition::msgs::Boolean &_res, const bool _result)
  {
    if (!_result)
    {
      ignerr << "Timed out requesting to move camera" << std::endl;
    }
    if (!_res.data())
    {
      ignerr << "Failed to move camera" << std::endl;
    }
  };

  this->node.Request("/gui/move_to/pose", req, cb);
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnSetVisualVisible(const std::string &_name, bool _visible)
{
  auto vis = this->scene->VisualByName(_name);

  if (!vis)
  {
    ignerr << "Couldn't find visual [" << _name << "]" << std::endl;
    return;
  }

  vis->SetVisible(_visible);
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnSeekLog(std::chrono::steady_clock::duration _time)
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
ignition::math::Pose3d SimSlidesIgn::OnInitialCameraPose()
{
  // TODO(louise) Support initial camera pose
  return {
    std::numeric_limits<double>::quiet_NaN(),
    std::numeric_limits<double>::quiet_NaN(),
    std::numeric_limits<double>::quiet_NaN(),
    std::numeric_limits<double>::quiet_NaN(),
    std::numeric_limits<double>::quiet_NaN(),
    std::numeric_limits<double>::quiet_NaN()
  };
}

/////////////////////////////////////////////////
ignition::math::Pose3d SimSlidesIgn::OnVisualPose(const std::string &_name)
{
  auto vis = this->camera->Scene()->VisualByName(_name);
  if (!vis)
  {
    ignerr << "Failed to find visual [" << _name << "]" << std::endl;
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
void SimSlidesIgn::OnSetText(const std::string &_name)
{
  // TODO(louise) Support setting text
}

// Register this plugin
IGNITION_ADD_PLUGIN(simslides::SimSlidesIgn,
                    ignition::gui::Plugin);
