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
  this->dataPtr->currentIndex = 0;
  this->ChangeSlide(true);
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

  bool next = false;

  // Next (right arrow)
  if (_msg->int_value() == 16777236 &&
      this->dataPtr->currentIndex + 1 < this->dataPtr->slideCount)
  {
    this->dataPtr->currentIndex++;
    next = true;
  }
  // Previous (left arrow)
  else if (_msg->int_value() == 16777234 && this->dataPtr->currentIndex >= 1)
  {
    this->dataPtr->currentIndex--;
  }
  // Current (F1)
  else if (_msg->int_value() == 16777264)
  {
  }
  // Home (F6)
  else if (_msg->int_value() == 16777269)
  {
    this->dataPtr->currentIndex = -1;
  }
  else
    return;

  this->ChangeSlide(next);
}

/////////////////////////////////////////////////
void PresentMode::OnSlideChanged(int _slide)
{
/*
  if (this->dataPtr->slideCount < 0)
    return;

  if (_slide > this->dataPtr->slideCount)
    this->dataPtr->currentIndex = this->dataPtr->slideCount - 1;

  // TODO: skipping slides doesn't work well with stacks
  bool next = _slide > this->dataPtr->currentIndex;

  this->dataPtr->currentIndex = _slide;

  this->ChangeSlide(next);
*/
}

/////////////////////////////////////////////////
void PresentMode::ChangeSlide(bool _next)
{
  ignition::math::Pose3d camPose;
  std::string toLookAt;

  // Fixed keyframe (home)
  if (this->dataPtr->currentIndex == -1)
  {
    camPose.Set(-9.13, 264.0, 136.73, 0, 0.466, -1.14);
  }
  // "Look at" Slides
  else if (this->dataPtr->currentIndex == 0 ||
        this->dataPtr->currentIndex == 1 ||
        this->dataPtr->currentIndex == 8 ||
        this->dataPtr->currentIndex == 9 ||
        this->dataPtr->currentIndex == 10)
  {
    toLookAt = simslides::slidePrefix + "-" +
        std::to_string(this->dataPtr->currentIndex);
  }
  // "stack" slides
  else if (this->dataPtr->currentIndex > 1 &&
        this->dataPtr->currentIndex < 8)
  {
    // Go through all slides in the stack, and move all to the back except the
    // one we want to show
    for (int i = 2; i < 8; ++i)
    {
      auto name = simslides::slidePrefix + "-" + std::to_string(i);

      auto vis = this->dataPtr->camera->GetScene()->GetVisual(name);

      if (!vis)
      {
        gzerr << "Couldn't find visual [" << name << "]" << std::endl;
        continue;
      }

      // show
      if (i == this->dataPtr->currentIndex)
        vis->SetPosition(ignition::math::Vector3d(-2.07, -3.869, 2.61));
      else
        vis->SetPosition(ignition::math::Vector3d(-2.07, -3.969, 2.61));
    }

    camPose.Set(1.6, 3.922, 1.621, 0, -0.14, -1.88);
  }

  if (!toLookAt.empty())
  {
    auto vis = this->dataPtr->camera->GetScene()->GetVisual(toLookAt);
    if (!vis)
    {
      gzerr << "Visual [" << toLookAt << "] not found" << std::endl;
      return;
    }

    // Target in world frame
    auto origin = vis->GetWorldPose().Ign();

    auto bb_pos = origin.Pos() +
                  vis->GetBoundingBox().Ign().Center();
    auto target_world = ignition::math::Matrix4d(ignition::math::Pose3d(
        bb_pos, origin.Rot()));

    // Eye in world frame
    ignition::math::Matrix4d eye_target;

    if (vis->GetName() != simslides::slidePrefix + "-11")
    {
      eye_target = ignition::math::Matrix4d(ignition::math::Pose3d(
          0, -6.3, 1.8, 0, 0.04, IGN_PI_2));
    }
    else
    {
      eye_target = ignition::math::Matrix4d(ignition::math::Pose3d(
          0, -3, 1.1, 0, 0.13, IGN_PI_2));
    }

    auto eye_world = target_world * eye_target;

    // Look At
    auto mat = ignition::math::Matrix4d::LookAt(eye_world.Translation(),
        target_world.Translation());

    camPose = mat.Pose();
  }

  this->dataPtr->camera->MoveToPosition(camPose, 1);
  this->SlideChanged(this->dataPtr->currentIndex, this->dataPtr->slideCount-1);
}



