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
#include <gazebo/rendering/UserCamera.hh>
#include <simslides/common/Common.hh>

#include "ImportDialog.hh"
#include "InsertActorDialog.hh"
#include "LoadDialog.hh"
#include "PresentMode.hh"
#include "SimSlides.hh"

using namespace simslides;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(SimSlides)

/////////////////////////////////////////////////
SimSlides::SimSlides()
  : gazebo::GUIPlugin()
{
  // Menu item
  auto menu = new QMenu("SimSlides");

  // Import dialog
  auto newSlideDialog = new ImportDialog();

  auto newAct = new QAction(tr("Import PDF"), menu);
  newAct->setShortcut(Qt::Key_F2);
  this->connect(newAct, SIGNAL(triggered()), newSlideDialog, SLOT(open()));
  menu->addAction(newAct);

  // Load keyframes
  // TODO(louise) Support loading any keyframes, not just spawning slides
/*
  auto loadDialog = new LoadDialog();

  auto loadAct = new QAction(tr("Load models"), menu);
  loadAct->setShortcut(Qt::Key_F3);
  this->connect(loadAct, SIGNAL(triggered()), loadDialog, SLOT(open()));
  menu->addAction(loadAct);
*/
/*
  // InsertActor keyframes
  auto importActorDialog = new InsertActorDialog();

  auto importActorAct = new QAction(tr("Insert actor"), menu);
  this->connect(importActorAct, SIGNAL(triggered()), importActorDialog, SLOT(open()));
  menu->addAction(importActorAct);
*/

  auto presentAct = new QAction(QIcon(":/images/play.png"),
      tr("Presentation mode"), menu);
  presentAct->setShortcut(Qt::Key_F5);
  this->connect(presentAct, SIGNAL(triggered()), this, SLOT(OnPresent()));
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
       }"\
      "QToolButton {\
         color : rgba(30, 30, 30, 255);\
         font-size : 100 px;\
       }");

  // Current
  this->currentSpin = new QSpinBox(0);
  this->currentSpin->setAlignment(Qt::AlignRight);
  this->currentSpin->setMaximumWidth(60);
  this->connect(this->currentSpin, SIGNAL(editingFinished()),
      this, SLOT(OnCurrentChanged()));

  // Total
  auto totalLabel = new QLabel("0");
  this->connect(this, SIGNAL(SetTotal(const QString)), totalLabel,
      SLOT(setText(const QString)));

  // Present
  auto presentButton = new QToolButton();
  presentButton->setDefaultAction(presentAct);
  presentButton->setIconSize(QSize(100, 100));

  // Text
  this->text = new QTextBrowser();
  this->text->setAcceptRichText(true);
  this->text->setOpenExternalLinks(true);
  this->text->setReadOnly(true);

  auto textLayout = new QVBoxLayout();
  textLayout->addWidget(this->text);

  auto textDialog = new QDialog(this);
  textDialog->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
  textDialog->setWindowTitle("SimSlide Text");
  textDialog->setLayout(textLayout);

  auto textButton = new QToolButton();
  textButton->setText(QString::fromUtf8("\u2197"));
  textButton->setIconSize(QSize(100, 100));
  this->connect(textButton, SIGNAL(clicked()), textDialog, SLOT(show()));

  // Create the layout that sits inside the frame
  auto frameLayout = new QHBoxLayout();
  frameLayout->addWidget(currentSpin);
  frameLayout->addWidget(new QLabel("/"));
  frameLayout->addWidget(totalLabel);
  frameLayout->addWidget(presentButton);
  frameLayout->addWidget(textButton);

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
  this->resize(200, 50);
}

/////////////////////////////////////////////////
void SimSlides::Load(const sdf::ElementPtr _sdf)
{
  Common::Instance()->LoadPluginSDF(_sdf);

  if (!std::isnan(Common::Instance()->farClip) && !std::isnan(Common::Instance()->nearClip))
  {
    auto camera = gazebo::gui::get_active_camera();
    if (nullptr == camera)
    {
      gzwarn << "No user camera, can't set near and far clip distances"
             << std::endl;
    }
    else
    {
      camera->SetClipDist(Common::Instance()->nearClip, Common::Instance()->farClip);
    }
  }
}

/////////////////////////////////////////////////
void SimSlides::OnKeyframeChanged(const int _keyframe, const int _total)
{
  this->currentSpin->blockSignals(true);
  this->currentSpin->setValue(_keyframe);
  this->currentSpin->blockSignals(false);

  this->SetTotal(QString::number(_total));
}

/////////////////////////////////////////////////
void SimSlides::OnTextChanged(QString _text)
{
  // FIXME Setting plain text so it's possible to display XML. Ideally we'd
  // support rich text with links
  this->text->setHtml(_text);
}

/////////////////////////////////////////////////
void SimSlides::OnCurrentChanged()
{
  this->CurrentChanged(this->currentSpin->value());
}

/////////////////////////////////////////////////
void SimSlides::OnPresent()
{
  if (nullptr != this->presentMode)
    delete this->presentMode;

  this->presentMode = new PresentMode();
  this->connect(presentMode, SIGNAL(KeyframeChanged(int, int)), this,
      SLOT(OnKeyframeChanged(int, int)));
  this->connect(presentMode, SIGNAL(TextChanged(QString)), this,
      SLOT(OnTextChanged(QString)));
  this->connect(this, SIGNAL(CurrentChanged(int)), presentMode,
      SLOT(OnKeyframeChanged(int)));

  this->presentMode->InitTransport();
  this->OnKeyframeChanged(Common::Instance()->currentKeyframe,
      Common::Instance()->keyframes.size()-1);
}

