#include <iostream>
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  public: KeyframeType type;
  public: int slideNumber = -1;
  public: ignition::math::Pose3d eyeOffset;
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
    this->dataPtr->eyeOffset = _sdf->Get<ignition::math::Pose3d>("eye_offset");
  }
  else if (type == "log_seek")
  {
    this->dataPtr->camPose = _sdf->Get<ignition::math::Pose3d>("cam_pose");
    this->dataPtr->logSeek = _sdf->Get<gazebo::common::Time>("time");
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
ignition::math::Pose3d Keyframe::EyeOffset() const
{
  return this->dataPtr->eyeOffset;
}

//////////////////////////////////////////////////
gazebo::common::Time Keyframe::LogSeek() const
{
  return this->dataPtr->logSeek;
}

