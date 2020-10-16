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

  ignmsg << "Start presentation. Total of [" << simslides::keyframes.size()
        << "] keyframes" << std::endl;

  // Trigger first slide
  simslides::currentKeyframe = 0;
  this->pendingCommand = true;

  ignition::gui::App()->findChild<ignition::gui::MainWindow *>
      ()->installEventFilter(this);
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

  ignition::math::Pose3d camPose;
  ignition::math::Pose3d eyeOff;
  std::string toLookAt;
  std::string text;

  // Reset presentation
  if (simslides::currentKeyframe == -1)
  {
//    camPose = this->camera->InitialPose();
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
      // TODO(louise) Support stacks with non-sequential slide suffixes
      // Find stack front
      auto frontKeyframe = simslides::currentKeyframe;
      while (frontKeyframe > 0 &&
          simslides::keyframes[frontKeyframe-1]->GetType() == KeyframeType::STACK)
      {
        frontKeyframe--;
      }

      // Find stack back
      auto backKeyframe = simslides::currentKeyframe;
      while (backKeyframe+1 < simslides::keyframes.size() &&
          simslides::keyframes[backKeyframe+1]->GetType() == KeyframeType::STACK)
      {
        backKeyframe++;
      }

      if (backKeyframe >= simslides::keyframes.size())
      {
        ignerr << "Dafuq! Back keyframe: " << backKeyframe << std::endl;
        return;
      }

      ignmsg << "Stack front: " << frontKeyframe << ", back: " << backKeyframe << std::endl;

      // Scale down the other slides in the stack
      for (int i = frontKeyframe; i <= backKeyframe; ++i)
      {
        auto name = simslides::keyframes[i]->Visual();

        auto vis = this->scene->VisualByName(name);

        if (!vis)
        {
          ignerr << "Couldn't find visual [" << name << "]" << std::endl;
          continue;
        }

        vis->SetVisible(name == keyframe->Visual());
      }
    }

//    if (keyframe->GetType() == KeyframeType::LOG_SEEK)
//    {
//      camPose = keyframe->CamPose();
//
//      // New chance to advertise
//      if (!this->logPlaybackControlPub &&
//          this->windowMode == "LogPlayback")
//      {
//        this->logPlaybackControlPub = this->node->
//            Advertise<gazebo::msgs::LogPlaybackControl>("~/playback_control");
//      }
//
//      if (this->logPlaybackControlPub)
//      {
//        auto logSeek = keyframe->LogSeek();
//        auto s = std::chrono::duration_cast<std::chrono::seconds>(logSeek);
//        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(logSeek-s);
//
//        gazebo::msgs::LogPlaybackControl msg;
//        gazebo::msgs::Set(msg.mutable_seek(),
//            gazebo::common::Time(s.count(), ns.count()));
//        msg.set_pause(false);
//        this->logPlaybackControlPub->Publish(msg);
//      }
//    }

    if (keyframe->GetType() == KeyframeType::CAM_POSE)
    {
      camPose = keyframe->CamPose();
    }
  }

  if (!toLookAt.empty())
  {
    std::string visName(toLookAt + "::link::visual");

    auto vis = this->camera->Scene()->VisualByName(visName);
    if (!vis)
    {
      ignerr << "Failed to find visual [" << visName << "]" << std::endl;
      return;
    }

    auto size = ignition::math::Vector3d(1, 1, 1); // vis->GeometrySize();

    // Target in world frame
    auto origin = vis->Parent()->Parent()->WorldPose();

    auto bb_pos = origin.Pos() + ignition::math::Vector3d(0, 0, size.Z()*0.5);
    auto target_world = ignition::math::Matrix4d(ignition::math::Pose3d(
        bb_pos, origin.Rot()));

    // Eye in target frame
    if (eyeOff == ignition::math::Pose3d::Zero)
    {
      eyeOff = ignition::math::Pose3d(
              simslides::kEyeOffsetX,
              -size.Z()*2,
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

  if ((this->camera->WorldPose().Pos() - camPose.Pos()).Length() > 0.001)
  {
    ignition::msgs::GUICamera req;
    ignition::msgs::Set(req.mutable_pose(), camPose);

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
  this->updateGUI(simslides::currentKeyframe, simslides::keyframes.size() - 1,
      QString::fromStdString(text));
}

/////////////////////////////////////////////////
void SimSlidesIgn::OnKeyPress(const ignition::msgs::Int32 &_msg)
{
  simslides::HandleKeyPress(_msg.data());
  this->pendingCommand = true;
}

// Register this plugin
IGNITION_ADD_PLUGIN(simslides::SimSlidesIgn,
                    ignition::gui::Plugin);
