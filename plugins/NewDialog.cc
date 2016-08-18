#include "NewDialog.hh"

using namespace simslides;

class simslides::NewDialogPrivate
{
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
  delete this->dataPtr;
  this->dataPtr = NULL;
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

  if (fileDialog.exec() == QDialog::Accepted)
  {
    QStringList selected = fileDialog.selectedFiles();
    if (selected.empty())
      return;

    gzdbg << selected[0].toStdString() << std::endl;
  }
}

