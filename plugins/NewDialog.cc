#include "NewDialog.hh"

using namespace simslides;

class simslides::NewDialogPrivate
{
  public: QString tmpDir = "/tmp/simslides_tmp";
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

  auto mainLayout = new QGridLayout();
  mainLayout->addWidget(new QLabel("Create new"), 0, 0);
  mainLayout->addWidget(browseButton, 1, 0);

  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
NewDialog::~NewDialog()
{
}

/////////////////////////////////////////////////
void NewDialog::OnBrowse()
{
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
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("mkdir", QStringList() << this->dataPtr->tmpDir);
    p.waitForFinished();
  }

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

  // Save models

  // Find number of images in temp path
  boost::filesystem::path tmpPath(this->dataPtr->tmpDir.toStdString());
  int count = std::count_if(
      boost::filesystem::directory_iterator(tmpPath),
      boost::filesystem::directory_iterator(),
      boost::bind( static_cast<bool(*)(const boost::filesystem::path&)>(boost::filesystem::is_regular_file),
        boost::bind( &boost::filesystem::directory_entry::path, _1 ) ) );

gzdbg << count << std::endl;
/*
  std::string modelName("slides-0");
  std::string modelsDir("/home/louise/code/simslides/models/");
  auto saveDialog = new gazebo::gui::SaveEntityDialog(
      gazebo::gui::SaveEntityDialog::MODEL);
  saveDialog->SetModelName(modelName);
  saveDialog->SetSaveLocation(modelsDir + modelName);
  saveDialog->AddDirToModelPaths(modelsDir + modelName);

  // Create dir
  {
    boost::filesystem::path path;
    path = path / (modelsDir + modelName);

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
                  <name>Slides/slides_0</name>\
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

  // Save material script
  std::ofstream materialFile(modelsDir + modelName + "/materials/scripts/script.material");
  if (materialFile.is_open())
  {
    materialFile <<
      "material Slides/slides_0\n\
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
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("mv", QStringList() <<
        QString(this->dataPtr->tmpDir + "/slides-0.png") <<
        QString::fromStdString(modelsDir + modelName +
            "/materials/textures/slides-0.png")
    );
    p.waitForFinished();
  }
*/

}



