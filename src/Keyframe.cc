/*
 * Copyright 2017 Louise Poubel
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
#include <iostream>
#include <regex>
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  /// \brief Type of this keyframe
  public: KeyframeType type;

  /// \brief Number of slide model
  public: int slideNumber = -1;

  /// \brief Camera offset in LOOKAT slide frame
  public: ignition::math::Pose3d eyeOffset;

  /// \brief Camera pose in world frame
  public: ignition::math::Pose3d camPose;

  /// \brief Log time to seek to
  public: gazebo::common::Time logSeek;

  /// \brief Text to display on dialog
  public: std::string text;
};

/////////////////////////////////////////////////
Keyframe::Keyframe(sdf::ElementPtr _sdf) : dataPtr(new KeyframePrivate)
{
  if (!_sdf)
    return;

  auto type = _sdf->Get<std::string>("type");
  if (_sdf->HasAttribute("eye_offset"))
  {
    this->dataPtr->eyeOffset = _sdf->Get<ignition::math::Pose3d>("eye_offset");
  }
  if (_sdf->HasAttribute("text"))
  {
    this->dataPtr->text = _sdf->Get<std::string>("text");

    // When the user wants special characters like <> to be printed in
    // QTextBrowser, we need these characters to be encoded. But SDF / TinyXml
    // decodes them. Thus the need for an intermediate encoding :]
    this->dataPtr->text = std::regex_replace(this->dataPtr->text,
        std::regex("<<"), "&lt;");
    this->dataPtr->text = std::regex_replace(this->dataPtr->text,
        std::regex(">>"), "&gt;");
  }
  if (type == "stack")
  {
    this->dataPtr->type = KeyframeType::STACK;
    this->dataPtr->slideNumber = _sdf->Get<int>("number");
  }
  else if (type == "lookat")
  {
    this->dataPtr->type = KeyframeType::LOOKAT;
    this->dataPtr->slideNumber = _sdf->Get<int>("number");
  }
  else if (type == "log_seek")
  {
    this->dataPtr->camPose = _sdf->Get<ignition::math::Pose3d>("cam_pose");
    auto logSeek = _sdf->Get<sdf::Time>("time");
    this->dataPtr->logSeek.sec = logSeek.sec;
    this->dataPtr->logSeek.nsec = logSeek.nsec;
    this->dataPtr->type = KeyframeType::LOG_SEEK;
  }
  else if (type == "cam_pose")
  {
    this->dataPtr->type = KeyframeType::CAM_POSE;
    this->dataPtr->camPose = _sdf->Get<ignition::math::Pose3d>("pose");
  }
  else
  {
    gzerr << "Unsupported type [" << type << "]" << std::endl;
  }

  if (this->dataPtr->slideNumber >= 0)
  {
    gzmsg << "Loading [" << type << "] keyframe tied to slide ["
          << this->dataPtr->slideNumber << "]" << std::endl;
  }
  else
  {
    gzmsg << "Loading [" << type << "] keyframe" << std::endl;
  }
}

/////////////////////////////////////////////////
Keyframe::~Keyframe()
{
}

//////////////////////////////////////////////////
KeyframeType Keyframe::GetType() const
{
  return this->dataPtr->type;
}

//////////////////////////////////////////////////
unsigned int Keyframe::SlideNumber() const
{
  return this->dataPtr->slideNumber;
}

//////////////////////////////////////////////////
ignition::math::Pose3d Keyframe::CamPose() const
{
  return this->dataPtr->camPose;
}

//////////////////////////////////////////////////
ignition::math::Pose3d Keyframe::EyeOffset() const
{
  return this->dataPtr->eyeOffset;
}

//////////////////////////////////////////////////
gazebo::common::Time Keyframe::LogSeek() const
{
  return this->dataPtr->logSeek;
}

//////////////////////////////////////////////////
std::string Keyframe::Text() const
{
  return this->dataPtr->text;
}

