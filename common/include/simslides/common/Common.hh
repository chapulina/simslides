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

#include <memory>
#include <string>

#include "Keyframe.hh"

namespace simslides
{
  class Common
  {
     /// \brief Private constructor
     private: Common() = default;

     /// \brief Get the instance
     /// \return Singleton
     public: static Common *Instance()
     {
       if (nullptr == instance)
         instance = new Common;
       return instance;
     }

     /// \brief Update the state according to current keyframe.
     public: void Update();

     /// \brief Load <gui><plugin> tag for libsimslides.
     /// \param[in] _sdf SDF element.
     public: void LoadPluginSDF(const sdf::ElementPtr _sdf);

     /// \brief Handle an incoming key press from the user
     /// \param[in] _key Key as an integer
     public: void HandleKeyPress(int _key);

     /// \brief Change to the given keyframe.
     /// If -1, go back to initial pose.
     /// If larger than total of keyframes, go to last keyframe.
     /// \param[in] _keyframe Index of keyframe to go to.
     public: void ChangeKeyframe(int _keyframe);

     /// \brief Function called to move camera, containing the target pose.
     public: std::function<void(const ignition::math::Pose3d &)> MoveCamera;

     /// \brief Function called to move camera, containing the target pose.
     public: std::function<void(const std::string &, bool)> SetVisualVisible;

     /// \brief Function called to seek a log, containing the target time.
     public: std::function<void(std::chrono::steady_clock::duration)> SeekLog;

     /// \brief Function called to set the camera back to initial pose.
     public: std::function<void()> ResetCameraPose;

     /// \brief Function called to get a visual's pose according to its scoped
     /// name.
     public: std::function<ignition::math::Pose3d(const std::string &)>
         VisualPose;

     /// \brief Function called to set text, containing the the text.
     public: std::function<void(const std::string &)> SetText;

     /// \brief Path where to save / find slide models
     public: std::string slidePath;

     /// \brief Vector of keyframes loaded for presentation
     public: std::vector<Keyframe *> keyframes;

     /// \brief User camera far clip as set by the user.
     public: double farClip{std::numeric_limits<double>::quiet_NaN()};

     /// \brief User camera near clip as set by the user.
     public: double nearClip{std::numeric_limits<double>::quiet_NaN()};

     /// \brief Keep track of current keyframe index.
     /// -1 means the "home" camera pose.
     /// 0 is the first keyframe.
     public: int currentKeyframe{-1};

     /// \brief Total number of keyframes.
     /// -1 means not presenting.
     public: int slideCount{-1};

     public: const ignition::math::Pose3d kEyeOffset
         {0.0, -3.0, 0.0, 0.0, 0.0, IGN_PI_2};

     /// \brief Static instance
     private: static Common *instance;
  };
}

#endif
