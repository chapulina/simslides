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

#include <simslides/common/Common.hh>

#include "Helpers.hh"
#include "LoadDialog.hh"

using namespace simslides;

class simslides::LoadDialogPrivate
{
  /// \brief Label holding path.
  public: QLabel * pathLabel;

  /// \brief Load button.
  public: QPushButton * loadButton;
};

/////////////////////////////////////////////////
LoadDialog::LoadDialog()
  : dataPtr(new LoadDialogPrivate)
{
  this->setWindowTitle(tr("Load an existing presentation"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  // Path
  this->dataPtr->pathLabel = new QLabel();

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowsePath()));

  // Load
  this->dataPtr->loadButton = new QPushButton(tr("Load"));
  this->dataPtr->loadButton->setEnabled(false);
  this->connect(this->dataPtr->loadButton, SIGNAL(clicked()), this,
      SLOT(Load()));

  // Layout
  auto mainLayout = new QGridLayout();

  // Path
  mainLayout->addWidget(new QLabel("Path:"), 0, 0);
  mainLayout->addWidget(this->dataPtr->pathLabel, 0, 1);
  mainLayout->addWidget(browseButton, 0, 2);

  // Load
  mainLayout->addWidget(this->dataPtr->loadButton, 2, 1);

  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
LoadDialog::~LoadDialog()
{
}

/////////////////////////////////////////////////
void LoadDialog::OnBrowsePath()
{
  this->dataPtr->loadButton->setEnabled(false);
  QCoreApplication::processEvents();

  QFileDialog fileDialog(this, tr("Choose Path file"), QDir::homePath());
  fileDialog.setFileMode(QFileDialog::DirectoryOnly);
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  QStringList filters;
  filters << "Path files (*.pdf)";
  fileDialog.setNameFilters(filters);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  this->dataPtr->pathLabel->setText(fileDialog.selectedFiles()[0]);

  this->CheckReady();
}

/////////////////////////////////////////////////
void LoadDialog::CheckReady(QString)
{
  this->dataPtr->loadButton->setEnabled(
      !this->dataPtr->pathLabel->text().isEmpty());

  Common::Instance()->slidePath = this->dataPtr->pathLabel->text().toStdString();
}

/////////////////////////////////////////////////
void LoadDialog::Load()
{
  Common::Instance()->slidePath = this->dataPtr->pathLabel->text().toStdString();

  simslides::SpawnSlides();

  this->close();
}

