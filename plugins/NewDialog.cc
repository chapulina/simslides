#include <gazebo/common/CommonIface.hh>
#include "NewDialog.hh"

using namespace simslides;

class simslides::NewDialogPrivate
{
  public: QString tmpDir = "/tmp/simslides_tmp";
  public: std::vector<QLabel *> steps;
};

/////////////////////////////////////////////////
NewDialog::NewDialog(QWidget *_parent)
  : QDialog(_parent), dataPtr(new NewDialogPrivate)
{
  this->setWindowTitle(tr("Create a new presentation"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowse()));

  // Steps
  for (int i = 0; i < 9; ++i)
  {
    auto step = new QLabel("ok");
    step->setVisible(false);
    this->dataPtr->steps.push_back(step);
  }

  auto mainLayout = new QGridLayout();
  mainLayout->addWidget(new QLabel("Choose PDF:"), 0, 0);
  mainLayout->addWidget(browseButton, 0, 1);
  mainLayout->addWidget(this->dataPtr->steps[0], 1, 0);
  mainLayout->addWidget(new QLabel("Create temp folder"), 1, 1);
  mainLayout->addWidget(this->dataPtr->steps[1], 2, 0);
  mainLayout->addWidget(new QLabel("Convert PDF to images"), 2, 1);
  mainLayout->addWidget(this->dataPtr->steps[2], 3, 0);
  mainLayout->addWidget(new QLabel("Find number of images in temp folder"), 3, 1);
  mainLayout->addWidget(this->dataPtr->steps[3], 4, 0);
  mainLayout->addWidget(new QLabel("Create model folder"), 4, 1);
  mainLayout->addWidget(this->dataPtr->steps[4], 5, 0);
  mainLayout->addWidget(new QLabel("Save model.config"), 5, 1);
  mainLayout->addWidget(this->dataPtr->steps[5], 6, 0);
  mainLayout->addWidget(new QLabel("Save model.sdf"), 6, 1);
  mainLayout->addWidget(this->dataPtr->steps[6], 7, 0);
  mainLayout->addWidget(new QLabel("Create materials folders"), 7, 1);
  mainLayout->addWidget(this->dataPtr->steps[7], 8, 0);
  mainLayout->addWidget(new QLabel("Save material script"), 8, 1);
  mainLayout->addWidget(this->dataPtr->steps[8], 9, 0);
  mainLayout->addWidget(new QLabel("Move image to texture folder"), 9, 1);

  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
NewDialog::~NewDialog()
{
}

/////////////////////////////////////////////////
void NewDialog::OnBrowse()
{
  for (int i = 0; i < this->dataPtr->steps.size(); ++i)
    this->dataPtr->steps[i]->setVisible(false);
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

  QStringList selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  // Create temp folder to hold images
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
  this->dataPtr->steps[0]->setVisible(true);
  QCoreApplication::processEvents();

  // Convert PDF to pngs
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("convert", QStringList() <<
        "-density" << "150" <<
        "-quality" << "100" <<
        "-sharpen" << "0x1.0" <<
        selected[0] <<
        QString(this->dataPtr->tmpDir + "/slides.png"));
    p.waitForFinished();
  }
  this->dataPtr->steps[1]->setVisible(true);
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
  this->dataPtr->steps[2]->setVisible(true);
  QCoreApplication::processEvents();

  std::string modelsDir = gazebo::common::cwd() + "/models/";
  auto saveDialog = new gazebo::gui::SaveEntityDialog(
      gazebo::gui::SaveEntityDialog::MODEL);
  for (int i = 0; i < count - 1; ++i)
  {
    std::string modelName("slides-" + std::to_string(i));
    saveDialog->SetModelName(modelName);
    saveDialog->SetSaveLocation(modelsDir + modelName);

    // Create dir
    {
      boost::filesystem::path path;
      path = path / (modelsDir + modelName);

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }

    this->dataPtr->steps[3]->setText(QString::number(i));
    this->dataPtr->steps[3]->setVisible(true);
    QCoreApplication::processEvents();

    // Save model.config
    saveDialog->GenerateConfig();
    saveDialog->SaveToConfig();

    this->dataPtr->steps[4]->setText(QString::number(i));
    this->dataPtr->steps[4]->setVisible(true);
    QCoreApplication::processEvents();

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
                    <name>Slides/slides_" + std::to_string(i) + "</name>\
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

    this->dataPtr->steps[5]->setText(QString::number(i));
    this->dataPtr->steps[5]->setVisible(true);
    QCoreApplication::processEvents();

    // Create materials dirs
    {
      boost::filesystem::path path;
      path = path / (modelsDir + modelName + "/materials/scripts");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }
    {
      boost::filesystem::path path;
      path = path / (modelsDir + modelName + "/materials/textures");

      if (!boost::filesystem::create_directories(path))
        gzerr << "Couldn't create folder [" << path << "]" << std::endl;
    }

    this->dataPtr->steps[6]->setText(QString::number(i));
    this->dataPtr->steps[6]->setVisible(true);
    QCoreApplication::processEvents();

    // Save material script
    std::ofstream materialFile(modelsDir + modelName +
        "/materials/scripts/script.material");
    if (materialFile.is_open())
    {
      materialFile <<
        "material Slides/slides_" << std::to_string(i) << "\n\
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

    this->dataPtr->steps[7]->setText(QString::number(i));
    this->dataPtr->steps[7]->setVisible(true);
    QCoreApplication::processEvents();

    // Move image to dir
    gazebo::common::moveFile(
      this->dataPtr->tmpDir.toStdString() + "/" + modelName + ".png",
      modelsDir + modelName + "/materials/textures/" + modelName + ".png");

    this->dataPtr->steps[8]->setText(QString::number(i));
    this->dataPtr->steps[8]->setVisible(true);
    QCoreApplication::processEvents();
  }
  saveDialog->AddDirToModelPaths(modelsDir + "dummy");

  // Clear temp path
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("rm", QStringList() << "-rf" << this->dataPtr->tmpDir);
    p.waitForFinished();
  }
}



