#ifndef SIMSLIDES_COMMON_HH_
#define SIMSLIDES_COMMON_HH_

#include <string>

#include "Keyframe.hh"

namespace simslides
{
  /// \brief Prefix for slide models' names, used for creating model files
  /// and to find slides during presentation.
  extern std::string slidePrefix;

  /// \brief Path where to save / find slide models
  extern std::string slidePath;

  /// \brief Vector of keyframes loaded for presentation
  extern std::vector<Keyframe *> keyframes;

  /// \brief Load slide models into the world based on the path. This is used
  /// after slides are generated and if the option to load slides is chosen,
  /// but not if slides are loaded from a world.
  void LoadSlides();
}

#endif
