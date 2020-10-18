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
#include <gazebo/common/Console.hh>
#include <gazebo/common/SystemPaths.hh>
#include <gazebo/gui/SaveEntityDialog.hh>
#include <sdf/Root.hh>
#include <simslides/common/Common.hh>
#include "Helpers.hh"
#include "ImportDialog.hh"

using namespace simslides;

class simslides::ImportDialogPrivate
{
  /// \brief Temp folder to keep slides while models are being generated
  public: QString tmpDir = "/tmp/simslides_tmp";

  /// \brief Holds the path to the PDF file
  public: QLabel * pdfLabel;

  /// \brief Wait message
  public: QLabel * waitLabel;

  /// \brief Directory to save models
  public: QLineEdit * dirEdit;

  /// \brief Model name prefix
  public: QLineEdit * nameEdit;

  /// \brief Next button #1
  public: QPushButton * next1Button;

  /// \brief Button to generate slides
  public: QPushButton * generateButton;

  /// \brief Sping boxes for slide size
  public: QDoubleSpinBox * scaleXSpin;
  public: QDoubleSpinBox * scaleYSpin;
  public: QDoubleSpinBox * scaleZSpin;

  /// \brief Stacked layout to hold steps
  public: QStackedLayout * stackedStepLayout;

  /// \brief Vector holding one button group per slide
  public: std::vector<QButtonGroup *> buttonGroups;

  /// \brief Total number of slides
  public: int count;

  /// \brief Prefix to be used for all generated models
  public: std::string modelPrefix;

  /// \brief External process to convert PDF into images
  public: QProcess * convertProcess{nullptr};
};

/////////////////////////////////////////////////
ImportDialog::ImportDialog()
  : dataPtr(new ImportDialogPrivate)
{
  this->setWindowTitle(tr("Create a new presentation"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  ////////////
  // Step 1 //
  ////////////
  auto step1Label = new QLabel(tr("<b>Step 1: Load a PDF file</b>"));
  step1Label->setMaximumHeight(50);

  // PDF
  this->dataPtr->pdfLabel = new QLabel();
  this->dataPtr->pdfLabel->setMinimumWidth(300);

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowsePDF()));

  // Next 1
  this->dataPtr->next1Button = new QPushButton(tr("Next"));
  this->dataPtr->next1Button->setEnabled(false);
  this->connect(this->dataPtr->next1Button, SIGNAL(clicked()), this,
      SLOT(OnLoadPDF()));

  // Step 1 layout
  auto step1Layout = new QGridLayout;
  step1Layout->setSpacing(0);
  step1Layout->addWidget(step1Label, 0, 0, 1, 3);
  step1Layout->addWidget(new QLabel("PDF file:"), 1, 0);
  step1Layout->addWidget(this->dataPtr->pdfLabel, 1, 1);
  step1Layout->addWidget(browseButton, 1, 2);
  step1Layout->addWidget(this->dataPtr->next1Button, 2, 0, 1, 3);

  auto step1Widget = new QWidget();
  step1Widget->setLayout(step1Layout);

  ////////////
  // Step 2 //
  ////////////

  // Step 2 layout (created and inserted later)

  ////////////
  // Step 3 //
  ////////////
  auto step3Label = new QLabel(tr("<b>Step 3: Generate world and models</b>"));

  // Save dir
  this->dataPtr->dirEdit = new QLineEdit();

  auto saveDirButton = new QPushButton(tr("Browse"));
  this->connect(saveDirButton, SIGNAL(clicked()), this, SLOT(OnBrowseDir()));

  // Model names
  this->dataPtr->nameEdit = new QLineEdit();
  this->connect(this->dataPtr->nameEdit, SIGNAL(textChanged(QString)), this,
      SLOT(CheckReady(QString)));

  // Scale
  this->dataPtr->scaleXSpin = new QDoubleSpinBox();
  this->dataPtr->scaleXSpin->setDecimals(4);
  this->dataPtr->scaleXSpin->setValue(1.6);
  this->connect(this->dataPtr->scaleXSpin, SIGNAL(valueChanged(QString)), this,
      SLOT(CheckReady(QString)));

  auto scaleXLayout = new QHBoxLayout();
  scaleXLayout->addWidget(new QLabel("X"));
  scaleXLayout->addWidget(this->dataPtr->scaleXSpin);
  scaleXLayout->addWidget(new QLabel("m"));

  this->dataPtr->scaleYSpin = new QDoubleSpinBox();
  this->dataPtr->scaleYSpin->setDecimals(4);
  this->dataPtr->scaleYSpin->setValue(0.001);
  this->connect(this->dataPtr->scaleYSpin, SIGNAL(valueChanged(QString)), this,
      SLOT(CheckReady(QString)));

  auto scaleYLayout = new QHBoxLayout();
  scaleYLayout->addWidget(new QLabel("Y"));
  scaleYLayout->addWidget(this->dataPtr->scaleYSpin);
  scaleYLayout->addWidget(new QLabel("m"));

  this->dataPtr->scaleZSpin = new QDoubleSpinBox();
  this->dataPtr->scaleZSpin->setDecimals(4);
  this->dataPtr->scaleZSpin->setValue(0.9);
  this->connect(this->dataPtr->scaleZSpin, SIGNAL(valueChanged(QString)), this,
      SLOT(CheckReady(QString)));

  auto scaleZLayout = new QHBoxLayout();
  scaleZLayout->addWidget(new QLabel("Z"));
  scaleZLayout->addWidget(this->dataPtr->scaleZSpin);
  scaleZLayout->addWidget(new QLabel("m"));

  // Generate
  this->dataPtr->generateButton = new QPushButton(tr("Generate"));
  this->dataPtr->generateButton->setEnabled(false);
  this->connect(this->dataPtr->generateButton, SIGNAL(clicked()), this,
      SLOT(OnGenerate()));

  // Step 3 layout
  // TODO(louise): Add tooltips and consider renaming fields to be more user
  // friendly
  // (not clear what's prefix)
  auto step3Layout = new QGridLayout;
  step3Layout->setSpacing(0);
  step3Layout->addWidget(step3Label, 0, 0, 1, 3);
  step3Layout->addWidget(new QLabel("Save folder:"), 1, 0);
  step3Layout->addWidget(this->dataPtr->dirEdit, 1, 1);
  step3Layout->addWidget(saveDirButton, 1, 2);
  step3Layout->addWidget(new QLabel("Prefix:"), 2, 0);
  step3Layout->addWidget(this->dataPtr->nameEdit, 2, 1, 1, 2);
  step3Layout->addWidget(new QLabel("Model scale:"), 3, 0);
  step3Layout->addLayout(scaleXLayout, 3, 1, 1, 2);
  step3Layout->addLayout(scaleYLayout, 4, 1, 1, 2);
  step3Layout->addLayout(scaleZLayout, 5, 1, 1, 2);
  step3Layout->addWidget(this->dataPtr->generateButton, 6, 0, 1, 3);

  auto step3Widget = new QWidget();
  step3Widget->setLayout(step3Layout);

  // Wait
  // TODO(louise): Show progress gif while loading
  this->dataPtr->waitLabel = new QLabel(
      "   Transforming PDF into PNG, this may take a while...\n\
           Believe me, just wait.");

  // TODO(louise): Handle importing a new PDF better.

  //////////
  // Main //
  //////////

  // Stacked layout
  this->dataPtr->stackedStepLayout = new QStackedLayout;
  this->dataPtr->stackedStepLayout->addWidget(step1Widget);
  this->dataPtr->stackedStepLayout->addWidget(this->dataPtr->waitLabel);
  this->dataPtr->stackedStepLayout->addWidget(step3Widget);

  this->setLayout(this->dataPtr->stackedStepLayout);
}

/////////////////////////////////////////////////
ImportDialog::~ImportDialog()
{
}

/////////////////////////////////////////////////
void ImportDialog::OnBrowsePDF()
{
  QCoreApplication::processEvents();

  QFileDialog fileDialog(this, tr("Choose PDF file"), QDir::homePath());
  fileDialog.setFileMode(QFileDialog::ExistingFile);
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  QStringList filters;
  filters << "PDF files (*.pdf)";
  fileDialog.setNameFilters(filters);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  this->dataPtr->pdfLabel->setText(fileDialog.selectedFiles()[0]);

  this->dataPtr->next1Button->setEnabled(true);
}

/////////////////////////////////////////////////
void ImportDialog::OnBrowseDir()
{
  this->dataPtr->generateButton->setEnabled(false);
  QCoreApplication::processEvents();

  QFileDialog fileDialog(this, tr("Choose directory to save models"),
      QDir::homePath());
  fileDialog.setFileMode(QFileDialog::DirectoryOnly);
  fileDialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  this->dataPtr->dirEdit->setText(fileDialog.selectedFiles()[0]);

  this->CheckReady();
}

/////////////////////////////////////////////////
void ImportDialog::CheckReady(QString)
{
  this->dataPtr->generateButton->setEnabled(
      !this->dataPtr->pdfLabel->text().isEmpty() &&
      !this->dataPtr->nameEdit->text().isEmpty() &&
      this->dataPtr->scaleXSpin->value() != 0 &&
      this->dataPtr->scaleYSpin->value() != 0 &&
      this->dataPtr->scaleZSpin->value() != 0 &&
      !this->dataPtr->dirEdit->text().isEmpty());

  this->dataPtr->modelPrefix = this->dataPtr->nameEdit->text().toStdString();
}

/////////////////////////////////////////////////
void ImportDialog::OnLoadPDF()
{
  this->dataPtr->stackedStepLayout->setCurrentIndex(1);
  QCoreApplication::processEvents();

  // Create / clear temp folder to hold images
  {
    if (!gazebo::common::exists(this->dataPtr->tmpDir.toStdString()))
    {
      gzmsg << "Creating temp dir [" << this->dataPtr->tmpDir.toStdString()
            << "]" << std::endl;
      QProcess p;
      p.setProcessChannelMode(QProcess::ForwardedChannels);
      p.start("mkdir", QStringList() << this->dataPtr->tmpDir);
      p.waitForFinished();
    }
    else
    {
      gzmsg << "Cleaning temp dir [" << this->dataPtr->tmpDir.toStdString()
            << "]" << std::endl;
      QProcess p;
      p.setProcessChannelMode(QProcess::ForwardedChannels);
      p.start("rm", QStringList() << "-rf" << QString(
          this->dataPtr->tmpDir + "/*"));
      p.waitForFinished();
    }
  }
  QCoreApplication::processEvents();

  // Convert PDF to pngs
  {
    if (!this->dataPtr->convertProcess)
    {
      this->dataPtr->convertProcess = new QProcess();

      this->dataPtr->convertProcess->setProcessChannelMode(
          QProcess::ForwardedChannels);

      this->connect(this->dataPtr->convertProcess,
          SIGNAL(finished(int, QProcess::ExitStatus)),
          this, SLOT(OnConversionFinished(int, QProcess::ExitStatus)));
    }

    gzmsg << "Converting PDF [" << this->dataPtr->pdfLabel->text().toStdString()
          << "] to images" << std::endl;
    this->dataPtr->convertProcess->start("convert", QStringList() <<
        "-density" << "150" <<
        "-quality" << "100" <<
        "-sharpen" << "0x1.0" <<
        this->dataPtr->pdfLabel->text() <<
        QString(this->dataPtr->tmpDir + "/tmpPng.png"));

    this->dataPtr->convertProcess->waitForStarted(1000);

    if (this->dataPtr->convertProcess->error() == QProcess::FailedToStart)
    {
      std::string error{
          "Failed to convert PDF. Have you installed ImageMagick?\n\
           sudo apt-get install imagemagick"};

      this->dataPtr->waitLabel->setText(QString::fromStdString(error));
      gzerr << error << std::endl;

      return;
    }
  }
}

/////////////////////////////////////////////////
void ImportDialog::OnConversionFinished(int _exitCode,
    QProcess::ExitStatus _exitStatus)
{
  if (_exitCode != 0 || _exitStatus != QProcess::NormalExit)
  {
    std::string error = "Failed to convert PDF, exit code: " +
        std::to_string(_exitCode) +
        ", exit status:" + std::to_string(_exitStatus);

    this->dataPtr->waitLabel->setText(QString::fromStdString(error));
    gzerr << error << std::endl;

    return;
  }

  // Generate step 2 widgets

  // Find number of images in temp path
  // TODO(louise): Use QDir
  this->dataPtr->count = QDir(this->dataPtr->tmpDir).entryList(QStringList("*"),
     QDir::Files | QDir::NoSymLinks).size();
  QCoreApplication::processEvents();

  // TODO(louise): Support other keyframes
  auto slidesLayout = new QVBoxLayout();
  for (int i = 0; i < this->dataPtr->count; ++i)
  {
    auto number = new QLabel("Slide " + QVariant(i).toString());

    auto lookat = new QRadioButton("Look at");
    lookat->setChecked(true);
    auto stack = new QRadioButton("Stack");

    auto group = new QButtonGroup();
    group->addButton(lookat, 0);
    group->addButton(stack, 1);
    this->dataPtr->buttonGroups.push_back(group);

    auto slideLayout = new QHBoxLayout();
    slideLayout->addWidget(number);
    slideLayout->addWidget(lookat);
    slideLayout->addWidget(stack);

    slidesLayout->addLayout(slideLayout);
  }

  auto step2Label = new QLabel(tr("<b>Step 2: Keyframes</b>"));

  // Next 2
  auto next2Button = new QPushButton(tr("Next"));
  this->connect(next2Button, SIGNAL(clicked()), this, SLOT(OnNext2()));

  auto step2Layout = new QVBoxLayout;
  step2Layout->setSpacing(0);
  step2Layout->addWidget(step2Label);
  step2Layout->addLayout(slidesLayout);
  step2Layout->addWidget(next2Button);

  auto step2Widget = new QWidget();
  step2Widget->setLayout(step2Layout);

  this->dataPtr->stackedStepLayout->insertWidget(2, step2Widget);
  this->dataPtr->stackedStepLayout->setCurrentIndex(2);
}

/////////////////////////////////////////////////
void ImportDialog::OnNext2()
{
  this->dataPtr->stackedStepLayout->setCurrentIndex(3);
}

/////////////////////////////////////////////////
void ImportDialog::OnGenerate()
{
  // Generate and save world and models, load Common::Instance()->keyframes
  this->GenerateWorld();

  // Insert models
  simslides::SpawnSlides();

  // Close dialog
  this->dataPtr->stackedStepLayout->removeItem(
      this->dataPtr->stackedStepLayout->itemAt(2));
  this->close();
}

/////////////////////////////////////////////////
void ImportDialog::AddGUI(std::string & _worldSdf)
{
  // TODO(louise): Use libsdformat

  // <plugin>
  std::string pluginStr = "\
      <plugin name='simslides' filename='libSimSlidesClassic.so'>\n";

  if (this->dataPtr->buttonGroups.size() != this->dataPtr->count)
  {
    gzerr << "Number of slides [" << this->dataPtr->count <<
         "] doesn't match number of button groups [" <<
         this->dataPtr->buttonGroups.size() << "]" << std::endl;
    return;
  }

  for (int i = 0; i < this->dataPtr->count; ++i)
  {
    auto visualName = this->dataPtr->modelPrefix + "-" + std::to_string(i);
    if (this->dataPtr->buttonGroups[i]->checkedId() == 0)
    {
      pluginStr +=
        "        <keyframe type='lookat' visual='" + visualName + "'/>\n";
    }
    else if (this->dataPtr->buttonGroups[i]->checkedId() == 1)
    {
      pluginStr +=
          "        <keyframe type='stack' visual='" + visualName + "'/>\n";
    }
    else
      gzerr << "Invalid button [" << i << "]" << std::endl;
  }
  pluginStr +="\
    </plugin>\n\
      <plugin name='keyboard' filename='libKeyboardGUIPlugin.so'>\n\
      </plugin>\n";

  // Load plugin so keyframes are generated
  // Hack: put it inside <world> because that can be a root SDF
  // TODO(louise): Instead, create Keyframe obejcts above.
  std::string sdfStr = "\
    <sdf version ='1.6'>\n\
      <world name='dummy'>\n" +
        pluginStr +
      "</world>\n\
    </sdf>\n";

  sdf::SDFPtr pluginSdf(new sdf::SDF);
  pluginSdf->SetFromString(sdfStr);
  auto pluginElem = pluginSdf->Root()->GetElement("world")->GetElement("plugin");
  Common::Instance()->LoadPluginSDF(pluginElem);

  // <gui>
  std::string guiStr = "\
    <gui>\n" +
      pluginStr +
    "</gui>\n";

  // Add to world
  _worldSdf += guiStr;
}

/////////////////////////////////////////////////
void ImportDialog::AddSlides(std::string & _worldSdf)
{
  // Scale
  auto scaleX = std::to_string(this->dataPtr->scaleXSpin->value());
  auto scaleY = std::to_string(this->dataPtr->scaleYSpin->value());
  auto scaleZ = std::to_string(this->dataPtr->scaleZSpin->value());
  auto height = std::to_string(this->dataPtr->scaleZSpin->value() * 0.5);

  // Save each model and add it to the world
  Common::Instance()->slidePath = this->dataPtr->dirEdit->text().toStdString();
  auto saveDialog = new gazebo::gui::SaveEntityDialog(
      gazebo::gui::SaveEntityDialog::MODEL);

  for (int i = 0; i < this->dataPtr->count; ++i)
  {
    std::string modelName(this->dataPtr->modelPrefix + "-" + std::to_string(i));
    saveDialog->SetModelName(modelName);
    saveDialog->SetSaveLocation(Common::Instance()->slidePath + "/" + modelName);

    // TODO(louise) Use QDir
    // Create dir
    {
      boost::filesystem::path path;
      path = path / (Common::Instance()->slidePath + "/" + modelName);

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }

    // Save model.config
    saveDialog->GenerateConfig();
    saveDialog->SaveToConfig();

    // Save model.sdf
    sdf::ElementPtr sdf(new sdf::Element());
    sdf::initFile("model.sdf", sdf);

    sdf::readString(
        "<?xml version='1.0' ?>\
        <sdf version='" SDF_VERSION "'>\
          <model name='" + modelName + "'>\
            <static>true</static>\
            <link name='link'>\
              <pose>0 0 " + height + " 0 0 0</pose>\
              <visual name='visual'>\
                <cast_shadows>false</cast_shadows>\
                <transparency>1</transparency>\
                <geometry>\
                  <box>\
                    <size>" + scaleX + " " + scaleY + " " + scaleZ + "</size>\
                  </box>\
                </geometry>\
                <material>\
                  <script>\
                    <uri>model://" + modelName + "/materials/scripts</uri>\
                    <uri>model://" + modelName + "/materials/textures</uri>\
                    <name>Slides/" + this->dataPtr->modelPrefix + "_" + std::to_string(i) + "</name>\
                  </script>\
                </material>\
              </visual>\
            </link>\
          </model>\
        </sdf>", sdf);

    sdf::SDFPtr modelSDF;
    modelSDF.reset(new sdf::SDF);
    modelSDF->Root(sdf);

    saveDialog->SaveToSDF(modelSDF);

    // Create materials dirs
    {
      boost::filesystem::path path;
      path = path / (Common::Instance()->slidePath + "/" + modelName + "/materials/scripts");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }
    {
      boost::filesystem::path path;
      path = path / (Common::Instance()->slidePath + "/" + modelName + "/materials/textures");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }

    // Save material script
    std::ofstream materialFile(Common::Instance()->slidePath + "/" + modelName +
        "/materials/scripts/script.material");
    if (materialFile.is_open())
    {
      materialFile <<
        "material Slides/" + this->dataPtr->modelPrefix + "_" << std::to_string(i) << "\n\
        {\n\
          receive_shadows off\n\
          technique\n\
          {\n\
            pass\n\
            {\n\
              lighting off\n\
              scene_blend alpha_blend\n\
              depth_check on\n\
              texture_unit\n\
              {\n\
                texture " + modelName + ".png\n\
                filtering anistropic\n\
                max_anisotropy 16\n\
              }\n\
            }\n\
          }\n\
        }";
      materialFile.close();
    }
    else
      gzerr << "Unable to open file" << std::endl;

    // Move image to dir
    gazebo::common::moveFile(
      this->dataPtr->tmpDir.toStdString() +
      "/tmpPng-" +
      std::to_string(i) +
      ".png",
      Common::Instance()->slidePath +
      "/" + modelName + "/materials/textures/" + modelName + ".png");

    // Add model to world
    _worldSdf+=
      "<include>\n\
        <name>" + modelName + "</name>\n\
        <pose>" + std::to_string(i) + "0 0 0 0 0 0</pose>\n\
        <uri>model://" + modelName + "</uri>\n\
      </include>";
  }

  // Add to path and wait to be added
  saveDialog->AddDirToModelPaths(Common::Instance()->slidePath + "/" + "dummy");
  bool found = false;
  while (!found)
  {
    auto modelPaths = gazebo::common::SystemPaths::Instance()->GetModelPaths();
    for (auto it : modelPaths)
    {
      if (it.compare(Common::Instance()->slidePath) == 0)
      {
        found = true;
        break;
      }
    }
    gazebo::common::Time::MSleep(100);
  }
}

/////////////////////////////////////////////////
void ImportDialog::GenerateWorld()
{
  // Start world
  std::string worldSdf = "<?xml version='1.0' ?>\n\
    <sdf version='1.5'>\n\
    <world name='default'>\n";

  this->AddGUI(worldSdf);

  // Continue world
  worldSdf += "\
    <include>\n\
      <uri>model://sun</uri>\n\
    </include>\n\
    <include>\n\
      <uri>model://ground_plane</uri>\n\
    </include>";

  this->AddSlides(worldSdf);

  // Save world
  worldSdf+= "</world>\n\
    </sdf>";
  std::string worldFile =
      Common::Instance()->slidePath + "/" + this->dataPtr->modelPrefix + ".world";
  std::ofstream saveWorld(worldFile, std::ios::out);
  if (!saveWorld)
  {
    QMessageBox msgBox;
    std::string str = "Unable to open file: " + worldFile;
    str += ".\nCheck file permissions.";
    msgBox.setText(str.c_str());
    msgBox.exec();
  }
  else
    saveWorld << worldSdf;
  saveWorld.close();

  gzdbg << "Saved world file to " << worldFile << std::endl;

  // Clear temp path
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("rm", QStringList() << "-rf" << this->dataPtr->tmpDir);
    p.waitForFinished();
  }
}

