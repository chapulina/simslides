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
#ifndef SIMSLIDES_PRESENTMODE_HH_
#define SIMSLIDES_PRESENTMODE_HH_

#include <memory>

#include <gazebo/gui/gui.hh>
#include <gazebo/msgs/any.pb.h>

namespace simslides
{
  class PresentModePrivate;

  /// \brief Handles keyframes while presenting.
  class PresentMode : public QObject
  {
    Q_OBJECT

    /// \brief Constructor.
    public: PresentMode();

    /// \brief Destructor.
    public: ~PresentMode();

    /// \brief Subscribing to key presses from the constructor makes the
    /// Node::Subscribe function crash with
    ///
    /// terminate called after throwing an instance of 'std::bad_function_call'                                                               ///  what():  bad_function_call
    ///
    /// So we initialize it later
    public: void InitTransport();

    /// \brief Callback when user presses a key.
    /// \param[in] _msg Message containing key.
    private: void OnKeyPress(ConstAnyPtr &_msg);

    /// \brief Callback to move camera.
    /// \param[in] _pose Pose to move to.
    private: void OnMoveCamera(const ignition::math::Pose3d &_pose);

    /// \brief Callback to show / hide a visual
    /// \param[in] _name Visual's scoped name
    /// \param[in] _visible True to show
    private: void OnSetVisualVisible(const std::string &_name, bool _visible);

    /// \brief Callback to seek a log file.
    /// \param[in] _time Time to seek to.
    private: void OnSeekLog(std::chrono::steady_clock::duration _time);

    /// \brief Callback to reset the camera pose.
    private: void OnResetCameraPose();

    /// \brief Callback to get a visual's pose
    /// \param[in] _name Visual's scoped name
    /// \return Visual's pose in world frame
    private: ignition::math::Pose3d OnVisualPose(const std::string &_name);

    /// \brief Callback to set the text on the dialog.
    /// \param[in] _text Text to set.
    private: void OnSetText(const std::string &_text);

    /// \brief Callback when Gazebo says the window mode has changed.
    /// \param[in] _mode New mode, usually "simulation" or "LogPlayback".
    private: void OnWindowMode(const std::string &_mode);

    /// \brief Performs the slide change, based on the current index which was
    /// previously set.
    private: void ChangeKeyframe();

    /// \brief Callback when the user requested a new keyframe.
    /// \oaram[in] _slide New slide index.
    private slots: void OnKeyframeChanged(int _slide);

    /// \brief Notifies that the slide index has changed,
    /// \param[in] _currentIndex Current keyframe index.
    /// \param[in] _slideCount Total number of keyframes.
    signals: void KeyframeChanged(int _currentIndex, int _slideCount);

    /// \brief Notifies that the slide text has changed,
    /// \param[in] _text Slide text.
    signals: void TextChanged(QString _text);

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<PresentModePrivate> dataPtr;
  };
  /// \}
}

#endif
