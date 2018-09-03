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

#include "Common.hh"
#include "ImportDialog.hh"
#include "InsertActorDialog.hh"
#include "Keyframe.hh"
#include "LoadDialog.hh"
#include "PresentMode.hh"
#include "Simslides.hh"

using namespace simslides;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(Simslides)

/////////////////////////////////////////////////
Simslides::Simslides()
  : gazebo::GUIPlugin()
{
  // Menu item
  auto menu = new QMenu("Simslides");

  // Import dialog
  auto newSlideDialog = new ImportDialog();

  auto newAct = new QAction(tr("Import PDF"), menu);
  newAct->setShortcut(Qt::Key_F2);
  this->connect(newAct, SIGNAL(triggered()), newSlideDialog, SLOT(open()));
  menu->addAction(newAct);

  // Load slides
  auto loadDialog = new LoadDialog();

  auto loadAct = new QAction(tr("Load models"), menu);
  loadAct->setShortcut(Qt::Key_F3);
  this->connect(loadAct, SIGNAL(triggered()), loadDialog, SLOT(open()));
  menu->addAction(loadAct);
/*
  // InsertActor slides
  auto importActorDialog = new InsertActorDialog();

  auto importActorAct = new QAction(tr("Insert actor"), menu);
  this->connect(importActorAct, SIGNAL(triggered()), importActorDialog, SLOT(open()));
  menu->addAction(importActorAct);
*/
  // Presentation mode
  auto presentMode = new PresentMode();
  this->connect(presentMode, SIGNAL(SlideChanged(int, int)), this,
      SLOT(OnSlideChanged(int, int)));
  this->connect(this, SIGNAL(CurrentChanged(int)), presentMode,
      SLOT(OnSlideChanged(int)));

  auto presentAct = new QAction(QIcon(":/images/play.png"),
      tr("Presentation mode"), menu);
  presentAct->setShortcut(Qt::Key_F5);
  presentAct->setCheckable(true);
  this->connect(presentAct, SIGNAL(toggled(bool)), presentMode,
      SLOT(OnToggled(bool)));
  menu->addAction(presentAct);

  // Add to main window
  auto mainWindow = gazebo::gui::get_main_window();
  mainWindow->AddMenu(menu);

  ////// OVERLAY ////////

  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame {\
         background-color : rgba(230, 230, 230, 255);\
         color : rgba(30, 30, 30, 255);\
         font-size : 50 px;\
       }"\
      "QSpinBox {\
         background-color : rgba(230, 230, 230, 255);\
         color : rgba(30, 30, 30, 255);\
         font-size : 50 px;\
       }");

  // Current
  auto currentSpin = new QSpinBox(0);
  currentSpin->setAlignment(Qt::AlignRight);
  currentSpin->setMaximumWidth(60);
  this->connect(this, SIGNAL(SetCurrent(const int)), currentSpin,
      SLOT(setValue(const int)));
  this->connect(currentSpin, SIGNAL(valueChanged(const int)), this,
      SLOT(OnCurrentChanged(const int)));

  // Total
  auto totalLabel = new QLabel("0");
  this->connect(this, SIGNAL(SetTotal(const QString)), totalLabel,
      SLOT(setText(const QString)));

  // Present
  auto presentButton = new QToolButton();
  presentButton->setDefaultAction(presentAct);
  presentButton->setIconSize(QSize(100, 100));

  // Create the layout that sits inside the frame
  auto frameLayout = new QHBoxLayout();
  frameLayout->addWidget(currentSpin);
  frameLayout->addWidget(new QLabel("/"));
  frameLayout->addWidget(totalLabel);
  frameLayout->addWidget(presentButton);

  // Create the frame to hold all the widgets
  auto mainFrame = new QFrame();
  mainFrame->setLayout(frameLayout);

  // Main layout
  auto mainLayout = new QHBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainFrame);
  this->setLayout(mainLayout);

  // Start with left pane closed
  gazebo::gui::Events::leftPaneVisibility(false);

  // Hide toolbars
  gazebo::gui::Events::showToolbars(false);

  // Position and resize this widget
  this->move(10, 10);
  this->resize(150, 50);
}

/////////////////////////////////////////////////
void Simslides::Load(const sdf::ElementPtr _sdf)
{
  if (_sdf->HasElement("slide_prefix"))
  {
    simslides::slidePrefix = _sdf->Get<std::string>("slide_prefix");
  }

  if (_sdf->HasElement("keyframe"))
  {
    auto keyframeElem = _sdf->GetElement("keyframe");
    while (keyframeElem)
    {
      simslides::keyframes.push_back(new Keyframe(keyframeElem));
      keyframeElem = keyframeElem->GetNextElement("keyframe");
    }
  }
}

/////////////////////////////////////////////////
void Simslides::OnSlideChanged(const int _slide, const int _total)
{
  this->SetCurrent(_slide);
  this->SetTotal(QString::number(_total));
}

/////////////////////////////////////////////////
void Simslides::OnCurrentChanged(const int _slide)
{
  this->CurrentChanged(_slide);
}

