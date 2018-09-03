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
#ifndef SIMSLIDES_KEYFRAME_HH_
#define SIMSLIDES_KEYFRAME_HH_

#include <memory>
#include <sdf/sdf.hh>
#include <ignition/math/Pose3.hh>

#include <gazebo/gui/gui.hh>

namespace simslides
{
  class KeyframePrivate;

  /// \brief Keyframe types
  enum KeyframeType
  {
    /// \brief Move the camera to look at a given slide
    LOOKAT,

    /// \brief The slide is part of a stack. Slides on a stack are located in
    /// the same position.
    STACK,

    /// \brief Seek to a specific time in a log file.
    LOG_SEEK
  };

  class Keyframe
  {
    /// \brief Constructor.
    /// \param[in] _sdf
    public: Keyframe(sdf::ElementPtr _sdf);

    /// \brief Destructor.
    public: ~Keyframe();

    /// \brief Get the full type definition.
    /// \return The full type definition.
    public: KeyframeType GetType() const;

    /// \brief
    /// \return
    public: unsigned int SlideNumber() const;

    /// \brief
    /// \return
    public: ignition::math::Pose3d CamPose() const;

    /// \brief
    /// \return
    public: gazebo::common::Time LogSeek() const;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<KeyframePrivate> dataPtr;
  };
  /// \}
}

#endif
