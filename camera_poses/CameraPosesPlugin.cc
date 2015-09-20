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
#include <sstream>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/gui/KeyEventHandler.hh>

#include "CameraPosesPlugin.hh"

using namespace gazebo;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(CameraPosesPlugin)

/////////////////////////////////////////////////
CameraPosesPlugin::CameraPosesPlugin()
  : GUIPlugin()
{
  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Count
  QLabel *countLabel = new QLabel("0");
  connect(this, SIGNAL(SetCount(QString)), countLabel, SLOT(setText(QString)));

  // Create the layout that sits inside the frame
  QVBoxLayout *frameLayout = new QVBoxLayout();
  frameLayout->setContentsMargins(0, 0, 0, 0);
  frameLayout->addWidget(countLabel);
  frameLayout->setAlignment(countLabel, Qt::AlignCenter);

  // Create the frame to hold all the widgets
  QFrame *mainFrame = new QFrame();
  mainFrame->setLayout(frameLayout);

  // Main layout
  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainFrame);
  this->setLayout(mainLayout);

  // Position and resize this widget
  this->move(10, 10);
  this->resize(50, 30);
}

/////////////////////////////////////////////////
void CameraPosesPlugin::Load(sdf::ElementPtr _sdf)
{
  // Get input params from SDF
  if (!_sdf->HasElement("pose"))
    return;

  sdf::ElementPtr poseElem = _sdf->GetElement("pose");

  while (poseElem)
  {
    this->poses.push_back(poseElem->Get<ignition::math::Pose3d>());
    poseElem = poseElem->GetNextElement("pose");
  }

  // Keep pointer to the user camera
  this->camera = gui::get_active_camera();

  // Start at pose 0
  this->currentIndex = 0;
  this->camera->MoveToPosition(this->poses[this->currentIndex], 1);

  this->SetCount(QString::number(this->currentIndex) + " / " +
                 QString::number(this->poses.size() - 1));

  // Filter keyboard events
  gui::KeyEventHandler::Instance()->AddPressFilter("camera_poses_plugin",
      boost::bind(&CameraPosesPlugin::OnKeyPress, this, _1));
}

/////////////////////////////////////////////////
bool CameraPosesPlugin::OnKeyPress(const common::KeyEvent &_event)
{
  // Next
  if (_event.key == Qt::Key_Right ||
      _event.key == Qt::Key_Down)
  {
    if (this->currentIndex + 1 < this->poses.size())
    {
      this->currentIndex++;
      this->camera->MoveToPosition(this->poses[this->currentIndex], 1);
    }
  }
  // Previous
  else if (_event.key == Qt::Key_Left ||
      _event.key == Qt::Key_Up)
  {
    if (this->currentIndex - 1 >= 0)
    {
      this->currentIndex--;
      this->camera->MoveToPosition(this->poses[this->currentIndex], 1);
    }
  }
  // First
  else if (_event.key == Qt::Key_F5)
  {
    this->currentIndex = 0;
    this->camera->MoveToPosition(this->poses[this->currentIndex], 1);
  }

  this->SetCount(QString::number(this->currentIndex) + " / " +
                 QString::number(this->poses.size() - 1));
}
