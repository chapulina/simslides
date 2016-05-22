/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
#ifndef _GAZEBO_CAMERA_POSES_PLUGIN_HH_
#define _GAZEBO_CAMERA_POSES_PLUGIN_HH_

#include <gazebo/common/Plugin.hh>
#include <gazebo/gui/GuiPlugin.hh>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/transport/transport.hh>
# include <gazebo/gui/gui.hh>
#endif

namespace gazebo
{
  struct Frame
  {
    ignition::math::Pose3d pose;

    /// -1: not stack
    ///  0: start stack
    ///  1: mid stack
    ///  2: end stack
    int stack = -1;
  };

    class GAZEBO_VISIBLE CameraPosesPlugin : public GUIPlugin
    {
      Q_OBJECT

      /// \brief Constructor
      public: CameraPosesPlugin();

      public: void Load(sdf::ElementPtr _sdf);

      /// \brief Key event filter callback when key is pressed.
      /// \param[in] _event The key event.
      /// \return True if the event was handled
      private: bool OnKeyPress(const common::KeyEvent &_event);

      /// \brief Qt signal when the a link has been added.
      Q_SIGNALS: void SetCount(QString);

      /// \brief Node used to establish communication with gzserver.
      private: transport::NodePtr node;

      /// \brief Publisher of factory messages.
      private: transport::PublisherPtr cameraPosesPub;

      private: std::vector<Frame> frames;

      private: int currentIndex;

      private: rendering::UserCameraPtr camera;
    };
}
#endif
