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

#ifndef SIMSLIDES_IGNITION_SIMSLIDESIGN_HH_
#define SIMSLIDES_IGNITION_SIMSLIDESIGN_HH_

#include <ignition/gui/qt.h>
#include <ignition/msgs/int64.pb.h>
#include <ignition/gui/Plugin.hh>
#include <ignition/rendering/Camera.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/transport/Node.hh>

namespace simslides
{
class SimSlidesIgn : public ignition::gui::Plugin
{
  Q_OBJECT

  /// \brief Constructor
  public: SimSlidesIgn();

  /// \brief Destructor
  public: virtual ~SimSlidesIgn();

  // Documentation inherited
  public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

  // Documentation inherited
  protected: bool eventFilter(QObject *_obj, QEvent *_event) override;

  /// \brief Callback trigged when the slide value is changed.
  /// \param[in] _slide Number of slide to change to
  protected slots: void OnSlideChanged(int _slide);

  /// \brief Process pending commands on the rendering thread.
  private slots: void ProcessCommands();

  private: void LoadScene();

  private: void OnKeyPress(const ignition::msgs::Int32 &_msg);

  private: bool pendingCommand;

  /// \brief Node used for communication.
  private: ignition::transport::Node node;

  /// \brief Keep pointer to camera so we can move it.
  private: ignition::rendering::CameraPtr camera;
  private: ignition::rendering::ScenePtr scene;

  private: double eyeOffsetX = 0;
  private: double eyeOffsetY = -3.0;
  private: double eyeOffsetZ = 0;
  private: double eyeOffsetRoll = 0;
  private: double eyeOffsetPitch = 0;
  private: double eyeOffsetYaw = IGN_PI_2;

//  /// \brief Used to start, stop, and step simulation.
//  private: ignition::transport::Publisher logPlaybackControlPub;
};
}

#endif
