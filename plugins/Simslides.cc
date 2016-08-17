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

#include "NewDialog.hh"
#include "Simslides.hh"

using namespace simslides;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(Simslides)

/////////////////////////////////////////////////
Simslides::Simslides()
  : gazebo::GUIPlugin()
{
  // New dialog
  auto newSlideDialog = new NewDialog();

  // Menu item
  auto menu = new QMenu("Simslides");
  auto simslidesAct = new QAction(tr("New presentation"), menu);
  this->connect(simslidesAct, SIGNAL(triggered()), newSlideDialog,
      SLOT(open()));
  menu->addAction(simslidesAct);

  auto mainWindow = gazebo::gui::get_main_window();
  mainWindow->AddMenu(menu);

  ////// OVERLAY ////////

  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the layout that sits inside the frame
  QVBoxLayout *frameLayout = new QVBoxLayout();
  frameLayout->setContentsMargins(0, 0, 0, 0);

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
void Simslides::Load(sdf::ElementPtr _sdf)
{
}
