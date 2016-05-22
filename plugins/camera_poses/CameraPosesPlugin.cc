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
#include <gazebo/rendering/Scene.hh>
#include <gazebo/rendering/Visual.hh>
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
  if (!_sdf->HasElement("frame"))
    return;

  sdf::ElementPtr frameElem = _sdf->GetElement("frame");

  while (frameElem)
  {
    Frame frame;
    if (frameElem->HasElement("pose"))
      frame.pose = frameElem->GetElement("pose")->Get<ignition::math::Pose3d>();
    if (frameElem->HasAttribute("stack"))
    {
      frame.stack = frameElem->Get<int>("stack");
      // Ignore pose and copy pose from slide before
      if (frame.stack == 1 || frame.stack == 2)
      {
        frame.pose = this->frames[this->frames.size()-1].pose;
      }
    }

    this->frames.push_back(frame);
    frameElem = frameElem->GetNextElement("frame");
  }

  // Keep pointer to the user camera
  this->camera = gui::get_active_camera();

  // Start at pose 0
  this->currentIndex = 0;
  this->camera->MoveToPosition(this->frames[this->currentIndex].pose, 1);

  this->SetCount(QString::number(this->currentIndex) + " / " +
                 QString::number(this->frames.size() - 1));

  // Start with left pane closed
  gui::Events::leftPaneVisibility(false);

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
    // If there is a next slide
    if (this->currentIndex + 1 < this->frames.size())
    {
      // Next slide
      this->currentIndex++;
      auto frame = this->frames[this->currentIndex];

      // Middle or end of stack
      if (frame.stack == 1 || frame.stack == 2)
      {
        std::string visPrevName = "slides_" + std::to_string(this->currentIndex - 1);
        std::string visName = "slides_" + std::to_string(this->currentIndex);
        auto visPrev = this->camera->GetScene()->GetVisual(visPrevName);
        auto vis = this->camera->GetScene()->GetVisual(visName);
        if (vis && visPrev)
        {
          visPrev->SetPosition(visPrev->GetPosition() + math::Vector3(0, 0.1, 0));
          vis->SetPosition(visPrev->GetPosition() + math::Vector3(0, -0.1, 0));
        }
        else
          gzerr << "Visual [" << visPrevName << "] or [" << visName << "] not found" << std::endl;
      }
      // Unstacked or start of stack
      else
      {
        this->camera->MoveToPosition(this->frames[this->currentIndex].pose, 1);
      }
    }
  }
  // Previous
  else if (_event.key == Qt::Key_Left ||
      _event.key == Qt::Key_Up)
  {
    // If there is a previous slide
    if (this->currentIndex - 1 >= 0)
    {
      // Previous slide
      this->currentIndex--;
      auto frame = this->frames[this->currentIndex];

      // Beginning or middle of stack
      if (frame.stack == 0 || frame.stack == 1)
      {
        std::string visNextName = "slides_" + std::to_string(this->currentIndex + 1);
        std::string visName = "slides_" + std::to_string(this->currentIndex);
        auto visNext = this->camera->GetScene()->GetVisual(visNextName);
        auto vis = this->camera->GetScene()->GetVisual(visName);
        if (vis && visNext)
        {
          visNext->SetPosition(visNext->GetPosition() + math::Vector3(0, 0.1, 0));
          vis->SetPosition(visNext->GetPosition() + math::Vector3(0, -0.1, 0));
        }
        else
          gzerr << "Visual [" << visNextName << "] or [" << visName << "] not found" << std::endl;
      }

      // Not on stack or end of stack
      else if (frame.stack == -1 || frame.stack == 2)
      {
        this->camera->MoveToPosition(this->frames[this->currentIndex].pose, 1);
      }
    }
  }
  // First
  else if (_event.key == Qt::Key_F5)
  {
    this->currentIndex = 0;
    this->camera->MoveToPosition(this->frames[this->currentIndex].pose, 1);
  }
  // Current
  else if (_event.key == Qt::Key_F1)
  {
    this->camera->MoveToPosition(this->frames[this->currentIndex].pose, 1);
  }

  this->SetCount(QString::number(this->currentIndex) + " / " +
                 QString::number(this->frames.size() - 1));
}
