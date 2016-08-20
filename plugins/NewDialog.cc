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

  if (fileDialog.exec() != QDialog::Accepted)
    return;

  QStringList selected = fileDialog.selectedFiles();
  if (selected.empty())
    return;

  // Create temp folder to hold images
  QString tmpDir("/tmp/simslides_tmp");
  {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start("mkdir", QStringList() << tmpDir);
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
        QString(tmpDir + "/slides.png"));
    p.waitForFinished();
  }

/*
    # substitute dash with underscore
    cd temp
    for i in `ls *-*`; do
      NEW=`echo $i|tr '-' '_'`
      mv $i $NEW
    done");
*/

}



