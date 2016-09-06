#include <sstream>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/rendering/Scene.hh>
#include <gazebo/rendering/Visual.hh>
#include <gazebo/gui/KeyEventHandler.hh>

#include "ImportDialog.hh"
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

  // Presentation mode
  auto presentMode = new PresentMode();

  auto presentAct = new QAction(tr("Presentation mode"), menu);
  presentAct->setShortcut(Qt::Key_F5);
  this->connect(presentAct, SIGNAL(triggered()), presentMode, SLOT(Start()));
  menu->addAction(presentAct);

  // Add to main window
  auto mainWindow = gazebo::gui::get_main_window();
  mainWindow->AddMenu(menu);

  ////// OVERLAY ////////

  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the layout that sits inside the frame
  QVBoxLayout *frameLayout = new QVBoxLayout();
  frameLayout->setContentsMargins(0, 0, 0, 0);

  // Create the frame to hold all the widgets
  QFrame *mainFrame = new QFrame();
  mainFrame->setLayout(frameLayout);

  // Main layout
  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainFrame);
  this->setLayout(mainLayout);

  // Position and resize this widget
  this->move(10, 10);
  this->resize(50, 30);
}

/////////////////////////////////////////////////
void Simslides::Load(sdf::ElementPtr _sdf)
{
}
