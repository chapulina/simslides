/*
 * Copyright (C) 2020 Louise Poubel
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
 *
*/

#ifndef SIMSLIDES_GAZEBO_SIMSLIDESGZ_HH_
#define SIMSLIDES_GAZEBO_SIMSLIDESGZ_HH_

#include <gz/gui/qt.h>
#include <gz/msgs/int32.pb.h>
#include <gz/msgs/Utility.hh>
#include <gz/gui/Plugin.hh>
#include <gz/rendering/Camera.hh>
#include <gz/rendering/Scene.hh>
#include <gz/transport/Node.hh>

namespace simslides
{
class SimSlidesGz : public gz::gui::Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: SimSlidesGz();

  /// \brief Destructor
  public: virtual ~SimSlidesGz();

  // Documentation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

  // Documentation inherited
  protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \brief Callback trigged when the keyframe value is changed.
  /// \param[in] _keyframe Number of keyframe to change to
  protected slots: void OnKeyframeChanged(int _keyframe);

  /// \brief Process pending commands on the rendering thread.
  private slots: void ProcessCommands();

  /// \brief Notifies that the keyframe index has changed,
  /// \param[in] _currentKeyframe Current keyframe index.
  /// \param[in] _keyframeCount Total number of keyframes.
  signals: void updateGUI(int _currentKeyframe, int _keyframeCount);

  /// \brief Get the scene and user camera
  private: void LoadScene();

  /// \brief Callback when user presses a key.
  /// \param[in] _msg Message containing key.
  private: void OnKeyPress(const gz::msgs::Int32 &_msg);

  /// \brief Callback to move camera.
  /// \param[in] _pose Pose to move to.
  private: void OnMoveCamera(const gz::math::Pose3d &_pose);

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
  private: gz::math::Pose3d OnVisualPose(const std::string &_name);

  /// \brief Callback to set the text on the dialog.
  /// \param[in] _text Text to set.
  private: void OnSetText(const std::string &_text);

  /// \brief True when there's a pending command.
  private: bool pendingCommand;

  /// \brief Node used for communication.
  private: gz::transport::Node node;

  /// \brief Keep pointer to camera so we can move it.
  private: gz::rendering::CameraPtr camera;

  /// \brief Keep pointer to scene so we can get visuals.
  private: gz::rendering::ScenePtr scene;

//  /// \brief Used to start, stop, and step simulation.
//  private: gz::transport::Publisher logPlaybackControlPub;
};
}

#endif
