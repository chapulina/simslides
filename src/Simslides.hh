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
#ifndef SIMSLIDES_SIMSLIDES_HH_
#define SIMSLIDES_SIMSLIDES_HH_

#include <gazebo/common/Plugin.hh>
#include <gazebo/gui/GuiPlugin.hh>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/gui/gui.hh>
#endif

namespace simslides
{
  class Simslides : public gazebo::GUIPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: Simslides();

    public: void Load(sdf::ElementPtr _sdf);

    private slots: void OnSlideChanged(int _slide, int _total);
    private slots: void OnCountChanged(int _slide);

    Q_SIGNALS: void SetCount(int);
    Q_SIGNALS: void CountChanged(int);
    Q_SIGNALS: void SetTotal(QString);
  };
}
#endif
