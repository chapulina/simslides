#include <iostream>
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  public: unsigned int type;
  public: unsigned int slideNumber;
  public: ignition::math::Pose3d camPose;
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
  else if (type == "campose")
  {
    this->dataPtr->camPose = _sdf->Get<ignition::math::Pose3d>("pose");
    this->AddType(KeyframeType::CAM_POSE);
  }
  else
    gzerr << "Unsupported type [" << type << "]" << std::endl;

  this->dataPtr->slideNumber = _sdf->Get<int>("number");

  gzmsg << "Loading keyframe [" << this->dataPtr->slideNumber << "]"
        << std::endl;
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

