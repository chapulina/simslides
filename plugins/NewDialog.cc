#include <gazebo/common/CommonIface.hh>
#include <gazebo/transport/Node.hh>
#include "NewDialog.hh"

using namespace simslides;

class simslides::NewDialogPrivate
{
  public: QString tmpDir = "/tmp/simslides_tmp";
  public: gazebo::transport::NodePtr node;
  public: gazebo::transport::PublisherPtr factoryPub;
  public: QLabel *pdfLabel;
  public: QLabel *waitLabel;
  public: QLabel *doneLabel;
  public: QLineEdit *dirEdit;
  public: QLineEdit *nameEdit;
  public: QPushButton *generateButton;
};

/////////////////////////////////////////////////
NewDialog::NewDialog(QWidget *_parent)
  : QDialog(_parent), dataPtr(new NewDialogPrivate)
{
  this->setWindowTitle(tr("Create a new presentation"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  // PDF
  this->dataPtr->pdfLabel = new QLabel();

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowsePDF()));

  // Save dir
  this->dataPtr->dirEdit = new QLineEdit();

  auto saveDirButton = new QPushButton(tr("Browse"));
  this->connect(saveDirButton, SIGNAL(clicked()), this, SLOT(OnBrowseDir()));

  // Model names
  this->dataPtr->nameEdit = new QLineEdit();
  this->connect(this->dataPtr->nameEdit, SIGNAL(textChanged(QString)), this,
      SLOT(CheckReady(QString)));

  // Generate
  this->dataPtr->generateButton = new QPushButton(tr("Generate"));
  this->dataPtr->generateButton->setEnabled(false);
  this->connect(this->dataPtr->generateButton, SIGNAL(clicked()), this,
      SLOT(OnGenerate()));

  // Wait
  this->dataPtr->waitLabel = new QLabel("Generating, this may take a while...");
  this->dataPtr->waitLabel->setVisible(false);

  // Done
  this->dataPtr->doneLabel = new QLabel("Done! Press F5 to start presenting!");
  this->dataPtr->doneLabel->setVisible(false);

  auto mainLayout = new QGridLayout();

  // PDF
  mainLayout->addWidget(new QLabel("PDF file:"), 0, 0);
  mainLayout->addWidget(this->dataPtr->pdfLabel, 0, 1);
  mainLayout->addWidget(browseButton, 0, 2);

  // Save dir
  mainLayout->addWidget(new QLabel("Save models folder:"), 1, 0);
  mainLayout->addWidget(this->dataPtr->dirEdit, 1, 1);
  mainLayout->addWidget(saveDirButton, 1, 2);

  // Model names
  mainLayout->addWidget(new QLabel("Model name prefix:"), 2, 0);
  mainLayout->addWidget(this->dataPtr->nameEdit, 2, 1);

  // Generate
  mainLayout->addWidget(this->dataPtr->generateButton, 3, 1);

  // Steps
  mainLayout->addWidget(this->dataPtr->waitLabel, 4, 0, 1, 3);
  mainLayout->addWidget(this->dataPtr->doneLabel, 5, 0, 1, 3);

  this->setLayout(mainLayout);

  this->dataPtr->node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  this->dataPtr->node->Init();
  this->dataPtr->factoryPub =
      this->dataPtr->node->Advertise<gazebo::msgs::Factory>("/gazebo/default/factory");
}

/////////////////////////////////////////////////
NewDialog::~NewDialog()
{
}

/////////////////////////////////////////////////
void NewDialog::OnBrowsePDF()
{
  this->dataPtr->waitLabel->setVisible(false);
  this->dataPtr->doneLabel->setVisible(false);
  this->dataPtr->generateButton->setEnabled(false);
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

  this->CheckReady();
}

/////////////////////////////////////////////////
void NewDialog::OnBrowseDir()
{
  this->dataPtr->waitLabel->setVisible(false);
  this->dataPtr->doneLabel->setVisible(false);
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
void NewDialog::CheckReady(QString)
{
  this->dataPtr->generateButton->setEnabled(
      !this->dataPtr->pdfLabel->text().isEmpty() &&
      !this->dataPtr->nameEdit->text().isEmpty() &&
      !this->dataPtr->dirEdit->text().isEmpty());
}

/////////////////////////////////////////////////
void NewDialog::OnGenerate()
{
  this->dataPtr->waitLabel->setVisible(true);
  this->dataPtr->doneLabel->setVisible(false);
  QCoreApplication::processEvents();

  // Create / clear temp folder to hold images
  {
    if (!gazebo::common::exists(this->dataPtr->tmpDir.toStdString()))
    {
      QProcess p;
      p.setProcessChannelMode(QProcess::ForwardedChannels);
      p.start("mkdir", QStringList() << this->dataPtr->tmpDir);
      p.waitForFinished();
    }
    else
    {
      QProcess p;
      p.setProcessChannelMode(QProcess::ForwardedChannels);
      p.start("rm", QStringList() << "-rf" << QString(this->dataPtr->tmpDir + "/*"));
      p.waitForFinished();
    }
  }
  QCoreApplication::processEvents();

  // Convert PDF to pngs
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("convert", QStringList() <<
        "-density" << "150" <<
        "-quality" << "100" <<
        "-sharpen" << "0x1.0" <<
        this->dataPtr->pdfLabel->text() <<
        QString(this->dataPtr->tmpDir + "/" + this->dataPtr->nameEdit->text() +
            ".png"));
    p.waitForFinished();
  }
  QCoreApplication::processEvents();

  // Save models

  // Find number of images in temp path
  boost::filesystem::path tmpPath(this->dataPtr->tmpDir.toStdString());
  int count = std::count_if(
      boost::filesystem::directory_iterator(tmpPath),
      boost::filesystem::directory_iterator(),
      boost::bind( static_cast<bool(*)(const boost::filesystem::path&)>(
      boost::filesystem::is_regular_file),
        boost::bind( &boost::filesystem::directory_entry::path, _1 ) ) );
  QCoreApplication::processEvents();

  std::string modelsDir = this->dataPtr->dirEdit->text().toStdString();
  auto saveDialog = new gazebo::gui::SaveEntityDialog(
      gazebo::gui::SaveEntityDialog::MODEL);
  for (int i = 0; i < count - 1; ++i)
  {
    std::string modelName(this->dataPtr->nameEdit->text().toStdString() + "-" +
        std::to_string(i));
    saveDialog->SetModelName(modelName);
    saveDialog->SetSaveLocation(modelsDir + "/" + modelName);

    // Create dir
    {
      boost::filesystem::path path;
      path = path / (modelsDir + "/" + modelName);

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
              <pose>0 0 1.5 0 0 0</pose>\
              <collision name='collision'>\
                <geometry>\
                  <box>\
                    <size>4.0 1.0 3.0</size>\
                  </box>\
                </geometry>\
              </collision>\
              <visual name='visual'>\
                <geometry>\
                  <box>\
                    <size>4.0 1.0 3.0</size>\
                  </box>\
                </geometry>\
                <material>\
                  <script>\
                    <uri>model://" + modelName + "/materials/scripts</uri>\
                    <uri>model://" + modelName + "/materials/textures</uri>\
                    <name>Slides/" + this->dataPtr->nameEdit->text().toStdString() + "_" + std::to_string(i) + "</name>\
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
      path = path / (modelsDir + "/" + modelName + "/materials/scripts");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }
    {
      boost::filesystem::path path;
      path = path / (modelsDir + "/" + modelName + "/materials/textures");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }

    // Save material script
    std::ofstream materialFile(modelsDir + "/" + modelName +
        "/materials/scripts/script.material");
    if (materialFile.is_open())
    {
      materialFile <<
        "material Slides/" + this->dataPtr->nameEdit->text().toStdString() + "_" << std::to_string(i) << "\n\
        {\n\
          receive_shadows off\n\
          technique\n\
          {\n\
            pass\n\
            {\n\
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
      this->dataPtr->tmpDir.toStdString() + "/" + modelName + ".png",
      modelsDir + "/" + modelName + "/materials/textures/" + modelName + ".png");

  }

  // Add to path and wait to be added
  saveDialog->AddDirToModelPaths(modelsDir + "/" + "dummy");
  bool found = false;
  while (!found)
  {
    auto modelPaths = gazebo::common::SystemPaths::Instance()->GetModelPaths();
    for (auto it : modelPaths)
    {
      if (it.compare(modelsDir) == 0)
      {
        found = true;
        break;
      }
    }
    gazebo::common::Time::MSleep(100);
  }

  // Clear temp path
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("rm", QStringList() << "-rf" << this->dataPtr->tmpDir);
    p.waitForFinished();
  }

  // Insert models
  int countX= 0;
  int countY= 0;
  for (int i = 0; i < count - 1; ++i)
  {
    gazebo::msgs::Factory msg;

    std::string filename("file://" + modelsDir + "/" +
        this->dataPtr->nameEdit->text().toStdString() + "-" + std::to_string(i));
    msg.set_sdf_filename(filename);

    gazebo::msgs::Set(msg.mutable_pose(),
        ignition::math::Pose3d(countX, countY, 0, 0, 0, 0));

    this->dataPtr->factoryPub->Publish(msg);

    if (countX > 30)
    {
      countX = 0;
      countY = countY + 10;
    }
    else
    {
      countX = countX + 10;
    }
  }
  this->dataPtr->waitLabel->setVisible(false);
  this->dataPtr->doneLabel->setVisible(true);
}



