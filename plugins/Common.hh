#ifndef SIMSLIDES_COMMON_HH_
#define SIMSLIDES_COMMON_HH_

#include <string>

#include "Keyframe.hh"

namespace simslides
{
  extern std::string slidePrefix;
  extern std::string slidePath;
  extern std::vector<Keyframe *> keyframes;

  void LoadSlides();
}

#endif
