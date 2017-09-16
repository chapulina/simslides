#include <iostream>
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  public: unsigned int type;
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
    this->AddType(KeyframeType::LOOKAT);
    this->AddType(KeyframeType::STACK);
  }
  else if (type == "lookat")
  {
    this->AddType(KeyframeType::LOOKAT);
  }
  else if (type == "log_seek")
  {
    this->dataPtr->camPose = _sdf->Get<ignition::math::Pose3d>("cam_pose");
    this->dataPtr->logSeek = _sdf->Get<gazebo::common::Time>("time");
    this->AddType(KeyframeType::LOG_SEEK);
  }
  else
    gzerr << "Unsupported type [" << type << "]" << std::endl;

  if (_sdf->HasAttribute("number"))
    this->dataPtr->slideNumber = _sdf->Get<int>("number");

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
void Keyframe::AddType(KeyframeType _t)
{
  this->dataPtr->type = this->dataPtr->type | (unsigned int)_t;
}

//////////////////////////////////////////////////
bool Keyframe::HasType(const KeyframeType &_t) const
{
  return ((unsigned int)(_t & this->dataPtr->type) == (unsigned int)_t);
}

//////////////////////////////////////////////////
unsigned int Keyframe::GetType() const
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

