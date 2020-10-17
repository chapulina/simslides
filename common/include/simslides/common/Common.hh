/*
 * Copyright 2016 Louise Poubel
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

  /// \brief Load <gui><plugin> tag for libsimslides.
  /// \param[in] _sdf SDF element.
  void LoadPluginSDF(const sdf::ElementPtr _sdf);
}

#endif
