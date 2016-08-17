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

  auto mainLayout = new QVBoxLayout();
  mainLayout->addWidget(new QLabel("Create new"));

  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
NewDialog::~NewDialog()
{
  delete this->dataPtr;
  this->dataPtr = NULL;
}

