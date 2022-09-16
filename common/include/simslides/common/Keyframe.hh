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

#include <chrono>
#include <memory>
#include <string>
#include <sdf/Element.hh>
#include <gz/math/Pose3.hh>

namespace simslides
{
  class KeyframePrivate;

  /// \brief Keyframe types
  enum KeyframeType
  {
    /// \brief No type
    NONE,

    /// \brief Move the camera to look at a given slide
    LOOKAT,

    /// \brief The slide is part of a stack. Slides on a stack are located in
    /// the same position.
    STACK,

    /// \brief Seek to a specific time in a log file.
    LOG_SEEK,

    /// \brief Move camera to a pose in the world.
    CAM_POSE
  };

  class Keyframe
  {
    /// \brief Constructor.
    /// \param[in] _sdf
    public: Keyframe(sdf::ElementPtr _sdf);

    /// \brief Destructor.
    public: ~Keyframe();

    /// \brief Get the keyframe type.
    /// \return The type of this keyframe.
    public: KeyframeType GetType() const;

    /// \brief The number of the slide model that this keyframe is tied to.
    /// \return Slide number.
    public: unsigned int SlideNumber() const;

    /// \brief Name of the visual that this keyframe is attached to.
    /// \return Visual name.
    public: std::string Visual() const;

    /// \brief For LOG_SEEK and CAM_POSE
    /// \return Camera pose in the world
    public: gz::math::Pose3d CamPose() const;

    /// \brief For LOOKAT
    /// \return Offset from target slide origin.
    public: gz::math::Pose3d EyeOffset() const;

    /// \brief Log time to seek to
    /// \return Log time
    public: std::chrono::steady_clock::duration LogSeek() const;

    /// \brief Text to display
    /// \return The text.
    public: std::string Text() const;

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<KeyframePrivate> dataPtr;
  };
  /// \}
}

#endif
