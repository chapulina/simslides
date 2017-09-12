#include <iostream>
#include <string>

#include "Keyframe.hh"

using namespace simslides;

class simslides::KeyframePrivate
{
  public: unsigned int type;
  public: unsigned int slideNumber;

};

/////////////////////////////////////////////////
Keyframe::Keyframe(sdf::ElementPtr _sdf) : dataPtr(new KeyframePrivate)
{
  if (!_sdf)
    return;

  auto type = _sdf->Get<std::string>("type");
  this->AddType(KeyframeType::LOOKAT);
  if (type == "stack")
    this->AddType(KeyframeType::STACK);
  else if (type != "lookat")
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

