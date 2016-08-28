#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/rendering/Scene.hh>

#include <gazebo/transport/Node.hh>
#include <gazebo/transport/Subscriber.hh>
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
}

/////////////////////////////////////////////////
PresentMode::~PresentMode()
{
}

/////////////////////////////////////////////////
void PresentMode::Start()
{
  if (!this->dataPtr->node)
  {
    // Initialize transport
    this->dataPtr->node = gazebo::transport::NodePtr(
        new gazebo::transport::Node());
    this->dataPtr->node->Init();

    this->dataPtr->keyboardSub =
        this->dataPtr->node->Subscribe("/gazebo/default/keyboard/keypress",
        &PresentMode::OnKeyPress, this, true);

    // Keep pointer to the user camera
    this->dataPtr->camera = gazebo::gui::get_active_camera();
  }

  this->dataPtr->slideCount = 0;
  while (this->dataPtr->camera->GetScene()->GetVisual("slides-" +
      std::to_string(this->dataPtr->slideCount)))
  {
    this->dataPtr->slideCount++;
  }
}

/////////////////////////////////////////////////
void PresentMode::OnKeyPress(ConstAnyPtr &_msg)
{
  // Next
  if (_msg->int_value() == 16777236 &&
      this->dataPtr->currentIndex + 1 < this->dataPtr->slideCount)
  {
    this->dataPtr->currentIndex++;
  }
  // Previous
  else if (_msg->int_value() == 16777234 && this->dataPtr->currentIndex >= 1)
  {
    this->dataPtr->currentIndex--;
  }
  // First
  else if (_msg->int_value() == 16777268)
  {
    this->dataPtr->currentIndex = 0;
  }
  // Current
  else if (_msg->int_value() == 16777264)
  {
  }
  else
    return;

  auto vis = this->dataPtr->camera->GetScene()->GetVisual("slides-" +
      std::to_string(this->dataPtr->currentIndex));

  auto target_world = ignition::math::Matrix4d(vis->GetWorldPose().Ign());

  auto eye_target = ignition::math::Matrix4d(ignition::math::Pose3d(
      0, -5.66, 2.83, 0, 0.26, IGN_PI_2));

  auto eye_world = target_world * eye_target;

  auto mat = ignition::math::Matrix4d::LookAt(eye_world.Translation(),
      target_world.Translation());

  this->dataPtr->camera->MoveToPosition(mat.Pose(), 1);
}



