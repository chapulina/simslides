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

#include "include/simslides/common/Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  /// \brief Print keyframe info
  public: void Print() const;

  /// \brief Get type as string
  public: std::string TypeToStr(KeyframeType _type) const;

  /// \brief Get type as enum
  public: KeyframeType StrToType(const std::string &_type) const;

  /// \brief Type of this keyframe
  public: KeyframeType type;

  /// \brief Number of slide model
  public: int slideNumber = -1;

  /// \brief Name of visual that this keyframe is attached to
  public: std::string visual;

  /// \brief Camera offset in LOOKAT slide frame
  public: gz::math::Pose3d eyeOffset;

  /// \brief Camera pose in world frame
  public: gz::math::Pose3d camPose;

  /// \brief Log time to seek to
  public: std::chrono::steady_clock::duration logSeek;

  /// \brief Text to display on dialog
  public: std::string text;
};

/////////////////////////////////////////////////
Keyframe::Keyframe(sdf::ElementPtr _sdf) : dataPtr(new KeyframePrivate)
{
  if (!_sdf)
    return;

  auto type = _sdf->Get<std::string>("type");
  this->dataPtr->type = this->dataPtr->StrToType(type);

  if (_sdf->HasAttribute("eye_offset"))
  {
    this->dataPtr->eyeOffset = _sdf->Get<gz::math::Pose3d>("eye_offset");
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
    this->dataPtr->slideNumber = _sdf->Get<int>("number");
    this->dataPtr->visual = _sdf->Get<std::string>("visual");
  }
  else if (type == "lookat")
  {
    this->dataPtr->slideNumber = _sdf->Get<int>("number");
    this->dataPtr->visual = _sdf->Get<std::string>("visual");
  }
  else if (type == "log_seek")
  {
    this->dataPtr->camPose = _sdf->Get<gz::math::Pose3d>("cam_pose");
    auto logSeek = _sdf->Get<sdf::Time>("time");
    this->dataPtr->logSeek = std::chrono::seconds(logSeek.sec) +
       std::chrono::nanoseconds(logSeek.nsec);
  }
  else if (type == "cam_pose")
  {
    this->dataPtr->camPose = _sdf->Get<gz::math::Pose3d>("pose");
  }
  else
  {
    std::cerr << "Unsupported type [" << type << "]" << std::endl;
  }

  this->dataPtr->Print();
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
std::string Keyframe::Visual() const
{
  return this->dataPtr->visual;
}

//////////////////////////////////////////////////
gz::math::Pose3d Keyframe::CamPose() const
{
  return this->dataPtr->camPose;
}

//////////////////////////////////////////////////
gz::math::Pose3d Keyframe::EyeOffset() const
{
  return this->dataPtr->eyeOffset;
}

//////////////////////////////////////////////////
std::chrono::steady_clock::duration Keyframe::LogSeek() const
{
  return this->dataPtr->logSeek;
}

//////////////////////////////////////////////////
std::string Keyframe::Text() const
{
  return this->dataPtr->text;
}

/////////////////////////////////////////////////
void KeyframePrivate::Print() const
{
  std::cout << "- Keyframe " << std::endl;
  std::cout << "    Type : " << this->TypeToStr(this->type) << std::endl;
  std::cout << "    Visual : " << this->visual << std::endl;
  std::cout << "    Eye offset : " << this->eyeOffset << std::endl;
  std::cout << "    Cam pose : " << this->camPose << std::endl;
  std::cout << "    Log seek : " << this->logSeek.count() << std::endl;
  std::cout << "    Text : " << this->text << std::endl;
}

/////////////////////////////////////////////////
std::string KeyframePrivate::TypeToStr(KeyframeType _type) const
{
  if (_type == KeyframeType::LOOKAT)
    return "lookat";

  if (_type == KeyframeType::STACK)
    return "stack";

  if (_type == KeyframeType::LOG_SEEK)
    return "log_seek";

  if (_type == KeyframeType::CAM_POSE)
    return "cam_pose";

  return std::string();
}

/////////////////////////////////////////////////
KeyframeType KeyframePrivate::StrToType(const std::string &_type) const
{
  if (_type == "lookat")
    return KeyframeType::LOOKAT;

  if (_type == "stack")
    return KeyframeType::STACK;

  if (_type == "log_seek")
    return KeyframeType::LOG_SEEK;

  if (_type == "cam_pose")
    return KeyframeType::CAM_POSE;

  return KeyframeType::NONE;
}
