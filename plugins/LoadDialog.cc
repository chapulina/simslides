#include "Common.hh"
#include "LoadDialog.hh"

using namespace simslides;

class simslides::LoadDialogPrivate
{
  public: QLabel *pathLabel;
  public: QLineEdit *nameEdit;
  public: QPushButton *loadButton;
};

/////////////////////////////////////////////////
LoadDialog::LoadDialog(QWidget *_parent)
  : QDialog(_parent), dataPtr(new LoadDialogPrivate)
{
  this->setWindowTitle(tr("Load an existing presentation"));
  this->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint |
      Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);

  // Path
  this->dataPtr->pathLabel = new QLabel();

  auto browseButton = new QPushButton(tr("Browse"));
  this->connect(browseButton, SIGNAL(clicked()), this, SLOT(OnBrowsePath()));

  // Model names
  this->dataPtr->nameEdit = new QLineEdit();
  this->connect(this->dataPtr->nameEdit, SIGNAL(textChanged(QString)), this,
      SLOT(CheckReady(QString)));

  // Load
  this->dataPtr->loadButton = new QPushButton(tr("Load"));
  this->dataPtr->loadButton->setEnabled(false);
  this->connect(this->dataPtr->loadButton, SIGNAL(clicked()), this,
      SLOT(Load()));

  auto mainLayout = new QGridLayout();

  // Path
  mainLayout->addWidget(new QLabel("Path:"), 0, 0);
  mainLayout->addWidget(this->dataPtr->pathLabel, 0, 1);
  mainLayout->addWidget(browseButton, 0, 2);

  // Model names
  mainLayout->addWidget(new QLabel("Model name prefix:"), 1, 0);
  mainLayout->addWidget(this->dataPtr->nameEdit, 1, 1);

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
      !this->dataPtr->pathLabel->text().isEmpty() &&
      !this->dataPtr->nameEdit->text().isEmpty());

  simslides::slidePrefix = this->dataPtr->nameEdit->text().toStdString();
  simslides::slidePath = this->dataPtr->pathLabel->text().toStdString();
}

/////////////////////////////////////////////////
void LoadDialog::Load()
{
  simslides::slidePrefix = this->dataPtr->nameEdit->text().toStdString();
  simslides::slidePath = this->dataPtr->pathLabel->text().toStdString();

  simslides::LoadSlides();
}



