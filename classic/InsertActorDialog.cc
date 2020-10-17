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
#include <gazebo/common/CommonIface.hh>
#include <gazebo/transport/Node.hh>
#include <gazebo/gui/ConfigWidget.hh>
#include "Common.hh"
#include "InsertActorDialog.hh"

using namespace simslides;

class simslides::InsertActorDialogPrivate
{
  public: gazebo::gui::ConfigWidget *configWidget;
  public: QLabel *waitLabel;
  public: QLabel *doneLabel;
  public: QLineEdit *dirEdit;
  public: QLineEdit *nameEdit;
  public: QPushButton *generateButton;
};

/////////////////////////////////////////////////
InsertActorDialog::InsertActorDialog(QWidget *_parent)
  : QDialog(_parent), dataPtr(new InsertActorDialogPrivate)
{
  this->setWindowTitle(tr("Insert a scripted actor"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowseSDF()));

  // ConfigWidget
  this->dataPtr->configWidget = new gazebo::gui::ConfigWidget();

  // Pose widgets
  auto poseWidget =
      this->dataPtr->configWidget->CreatePoseWidget("pose", 0);
  this->dataPtr->configWidget->AddConfigChildWidget("pose",
      poseWidget);

  // Connect pose widgets signal
  connect(this->dataPtr->configWidget,
      SIGNAL(PoseValueChanged(const QString,
      const ignition::math::Pose3d)),
      this, SLOT(OnPoseFromDialog(const QString,
      const ignition::math::Pose3d)));

  // Config Widget layout
  auto configLayout = new QVBoxLayout();
  configLayout->setSpacing(0);
  configLayout->addWidget(poseWidget);

  this->dataPtr->configWidget->setLayout(configLayout);

  // Scroll area
  auto scrollArea = new QScrollArea;
  scrollArea->setWidget(this->dataPtr->configWidget);
  scrollArea->setWidgetResizable(true);

  // General layout
  auto generalLayout = new QVBoxLayout;
  generalLayout->setContentsMargins(0, 0, 0, 0);
  generalLayout->addWidget(scrollArea);

  // Insert button
  auto insertButton = new QPushButton(tr("Insert"));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(OnInsert()));

  // Main layout
  auto mainLayout = new QVBoxLayout;
  mainLayout->addLayout(generalLayout);
  mainLayout->addWidget(insertButton);

  this->setLayout(mainLayout);

  // Setup transport
  auto node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init();
  auto factoryPub =
       node->Advertise<gazebo::msgs::Factory>("/gazebo/default/factory");
/*

  std::string sdfStr(
      "<?xml version=\"1.0\" ?>\
<sdf version=\"1.6\">\
      <actor name=\"actor\">\
    <include>\
      <uri>model://beer</uri>\
    </include>\
         <script>\
            <loop>true</loop>\
            <delay_start>0.000000</delay_start>\
            <auto_start>true</auto_start>\
            <trajectory id=\"0\" type=\"walking\">\
               <waypoint>\
                  <time>0.000000</time>\
                  <pose>1 0 1 0 0 0</pose>\
               </waypoint>\
               <waypoint>\
                  <time>1.000000</time>\
                  <pose>1 1 1 0 0 0</pose>\
               </waypoint>\
               <waypoint>\
                  <time>2.000000</time>\
                  <pose>0 1 1 0 0 0</pose>\
               </waypoint>\
               <waypoint>\
                  <time>3.000000</time>\
                  <pose>0 0 1 0 0 0</pose>\
               </waypoint>\
               <waypoint>\
                  <time>4.000000</time>\
                  <pose>1 0 1 0 0 0</pose>\
               </waypoint>\
            </trajectory>\
         </script>\
      </actor>\
</sdf>\
      ");



  gazebo::msgs::Factory msg;
  msg.set_sdf(sdfStr);

  factoryPub->Publish(msg);
*/
}

/////////////////////////////////////////////////
InsertActorDialog::~InsertActorDialog()
{
}
