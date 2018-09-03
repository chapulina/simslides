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
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  public: KeyframeType type;
  public: int slideNumber = -1;
  public: ignition::math::Pose3d camPose;
  public: gazebo::common::Time logSeek;
};

/////////////////////////////////////////////////
Keyframe::Keyframe(sdf::ElementPtr _sdf) : dataPtr(new KeyframePrivate)
{
  if (!_sdf)
    return;

  auto type = _sdf->Get<std::string>("type");
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
  else
    gzerr << "Unsupported type [" << type << "]" << std::endl;

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
gazebo::common::Time Keyframe::LogSeek() const
{
  return this->dataPtr->logSeek;
}

