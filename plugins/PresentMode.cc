#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/rendering/Scene.hh>

#include <gazebo/transport/Node.hh>
#include <gazebo/transport/Subscriber.hh>

#include "Common.hh"
#include "PresentMode.hh"

using namespace simslides;

class simslides::PresentModePrivate
{
  public: gazebo::transport::NodePtr node;
  public: gazebo::transport::SubscriberPtr keyboardSub;
  public: gazebo::rendering::UserCameraPtr camera;
  public: int currentIndex = -1;
  public: int slideCount = 0;
};

/////////////////////////////////////////////////
PresentMode::PresentMode(QObject *_parent)
  : QObject(_parent), dataPtr(new PresentModePrivate)
{
  // Keep pointer to the user camera
  this->dataPtr->camera = gazebo::gui::get_active_camera();
}

/////////////////////////////////////////////////
PresentMode::~PresentMode()
{
}

/////////////////////////////////////////////////
void PresentMode::OnToggled(bool _checked)
{
  if (_checked)
    this->Start();
  else
    this->Stop();
}

/////////////////////////////////////////////////
void PresentMode::Start()
{
  if (!this->dataPtr->camera->GetScene()->GetVisual(simslides::slidePrefix + "-0"))
  {
    gzerr << "No slide models named [" << simslides::slidePrefix <<
        "] to present." << std::endl;
    return;
  }

  if (!this->dataPtr->node)
  {
    // Initialize transport
    this->dataPtr->node = gazebo::transport::NodePtr(
        new gazebo::transport::Node());
    this->dataPtr->node->Init();

    this->dataPtr->keyboardSub =
        this->dataPtr->node->Subscribe("/gazebo/default/keyboard/keypress",
        &PresentMode::OnKeyPress, this, true);
  }

  this->dataPtr->slideCount = 0;
  while (this->dataPtr->camera->GetScene()->GetVisual(simslides::slidePrefix +
      "-" + std::to_string(this->dataPtr->slideCount)))
  {
    this->dataPtr->slideCount++;
  }

  // Trigger first slide
  this->ChangeSlide(-1);
}

/////////////////////////////////////////////////
void PresentMode::Stop()
{
  this->dataPtr->slideCount = -1;
}

/////////////////////////////////////////////////
void PresentMode::OnKeyPress(ConstAnyPtr &_msg)
{
  if (this->dataPtr->slideCount < 0)
    return;

  this->ChangeSlide(_msg->int_value());
}

/////////////////////////////////////////////////
void PresentMode::ChangeSlide(const int _key)
{
  // Next
  if (_key == 16777236 &&
      this->dataPtr->currentIndex + 1 < this->dataPtr->slideCount)
  {
    this->dataPtr->currentIndex++;
  }
  // Previous
  else if (_key == 16777234 && this->dataPtr->currentIndex >= 1)
  {
    this->dataPtr->currentIndex--;
  }
  // First
  else if (_key == -1)
  {
    this->dataPtr->currentIndex = 0;
  }
  // Current
  else if (_key == 16777264)
  {
  }
  else
    return;

  auto vis = this->dataPtr->camera->GetScene()->GetVisual(
      simslides::slidePrefix + "-" +
      std::to_string(this->dataPtr->currentIndex));

  // Target in world frame
  auto origin = vis->GetWorldPose().Ign();

  auto bb_pos = origin.Pos() +
                vis->GetBoundingBox().Ign().Center();
  auto target_world = ignition::math::Matrix4d(ignition::math::Pose3d(
      bb_pos, origin.Rot()));

  // Eye in world frame
  auto eye_target = ignition::math::Matrix4d(ignition::math::Pose3d(
      0, -6.3, 1.8, 0, 0.04, IGN_PI_2));

  auto eye_world = target_world * eye_target;

  // Up in world frame
/*
  auto rot = ignition::math::Matrix4d(ignition::math::Pose3d(
      ignition::math::Vector3d::Zero, origin.Rot()));

  auto up = rot * ignition::math::Vector3d::UnitZ;

  up += eye_world.Translation();

std::cout << "---------" << std::endl;
std::cout << "Eye: " << eye_world.Translation() << std::endl;
std::cout << "target: " << target_world.Translation() << std::endl;
std::cout << "up: " << up << std::endl;
*/
  // Look At
  auto mat = ignition::math::Matrix4d::LookAt(eye_world.Translation(),
      target_world.Translation());

  this->dataPtr->camera->MoveToPosition(mat.Pose(), 1);
  this->SlideChanged(this->dataPtr->currentIndex);
}



