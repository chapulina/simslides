#include <sstream>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/rendering/Scene.hh>
#include <gazebo/rendering/Visual.hh>
#include <gazebo/gui/KeyEventHandler.hh>

#include "ImportDialog.hh"
#include "InsertActorDialog.hh"
#include "LoadDialog.hh"
#include "PresentMode.hh"
#include "Simslides.hh"

using namespace simslides;

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(Simslides)

/////////////////////////////////////////////////
Simslides::Simslides()
  : gazebo::GUIPlugin()
{
  // Menu item
  auto menu = new QMenu("Simslides");

  // Import dialog
  auto newSlideDialog = new ImportDialog();

  auto newAct = new QAction(tr("Import PDF"), menu);
  newAct->setShortcut(Qt::Key_F2);
  this->connect(newAct, SIGNAL(triggered()), newSlideDialog, SLOT(open()));
  menu->addAction(newAct);

  // Load slides
  auto loadDialog = new LoadDialog();

  auto loadAct = new QAction(tr("Load models"), menu);
  loadAct->setShortcut(Qt::Key_F3);
  this->connect(loadAct, SIGNAL(triggered()), loadDialog, SLOT(open()));
  menu->addAction(loadAct);
/*
  // InsertActor slides
  auto importActorDialog = new InsertActorDialog();

  auto importActorAct = new QAction(tr("Insert actor"), menu);
  this->connect(importActorAct, SIGNAL(triggered()), importActorDialog, SLOT(open()));
  menu->addAction(importActorAct);
*/
  // Presentation mode
  auto presentMode = new PresentMode();
  this->connect(presentMode, SIGNAL(SlideChanged(int, int)), this,
      SLOT(OnSlideChanged(int, int)));
  this->connect(this, SIGNAL(CountChanged(int)), presentMode,
      SLOT(OnSlideChanged(int)));

  auto presentAct = new QAction(QIcon(":/images/play.png"),
      tr("Presentation mode"), menu);
  presentAct->setShortcut(Qt::Key_F5);
  presentAct->setCheckable(true);
  this->connect(presentAct, SIGNAL(toggled(bool)), presentMode,
      SLOT(OnToggled(bool)));
  menu->addAction(presentAct);

  // Add to main window
  auto mainWindow = gazebo::gui::get_main_window();
  mainWindow->AddMenu(menu);

  ////// OVERLAY ////////

  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Count
  auto countSpin = new QSpinBox(0);
  this->connect(this, SIGNAL(SetCount(int)), countSpin,
      SLOT(setValue(int)));
  this->connect(countSpin, SIGNAL(valueChanged(int)), this,
      SLOT(OnCountChanged(int)));

  // Total
  auto totalLabel = new QLabel("0");
  this->connect(this, SIGNAL(SetTotal(QString)), totalLabel,
      SLOT(setText(QString)));

  // Present
  auto presentButton = new QToolButton();
  presentButton->setDefaultAction(presentAct);
  presentButton->setIconSize(QSize(100, 100));

  // Create the layout that sits inside the frame
  auto frameLayout = new QHBoxLayout();
  frameLayout->addWidget(countSpin);
  frameLayout->addWidget(new QLabel("/"));
  frameLayout->addWidget(totalLabel);
  frameLayout->addWidget(presentButton);

  // Create the frame to hold all the widgets
  QFrame *mainFrame = new QFrame();
  mainFrame->setLayout(frameLayout);

  // Main layout
  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainFrame);
  this->setLayout(mainLayout);

  // Start with left pane closed
  gazebo::gui::Events::leftPaneVisibility(false);

  // Position and resize this widget
  this->move(10, 10);
  this->resize(150, 50);
}

/////////////////////////////////////////////////
void Simslides::Load(sdf::ElementPtr _sdf)
{
  if (_sdf->HasElement("slide_prefix"))
  {
    simslides::slidePrefix = _sdf->Get<std::string>("slide_prefix");
  }
}

/////////////////////////////////////////////////
void Simslides::OnSlideChanged(int _slide, int _total)
{
  this->SetCount(_slide);
  this->SetTotal(QString::number(_total));
}

/////////////////////////////////////////////////
void Simslides::OnCountChanged(int _slide)
{
  this->CountChanged(_slide);
}
