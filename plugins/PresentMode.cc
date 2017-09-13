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

  public: double eyeOffsetX = 0;
  public: double eyeOffsetY = -3.0;
  public: double eyeOffsetZ = 0;
  public: double eyeOffsetRoll = 0;
  public: double eyeOffsetPitch = 0;
  public: double eyeOffsetYaw = IGN_PI_2;
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
  if (!this->dataPtr->camera->GetScene()->GetVisual(
      simslides::slidePrefix + "-0::link::visual"))
  {
    gzerr << "No slide models named [" << simslides::slidePrefix <<
        "] to present." << std::endl;
    return;
  }

  if (simslides::keyframes.size() == 0)
  {
    gzerr << "No keyframes were loaded." << std::endl;
    return;
  }

  if (!this->dataPtr->node)
  {
    // Initialize transport
    this->dataPtr->node = gazebo::transport::NodePtr(
        new gazebo::transport::Node());
    this->dataPtr->node->Init();

    this->dataPtr->keyboardSub =
        this->dataPtr->node->Subscribe("~/keyboard/keypress",
        &PresentMode::OnKeyPress, this, true);
  }

  this->dataPtr->slideCount = simslides::keyframes.size();

  gzmsg << "Start presentation. Total of [" << this->dataPtr->slideCount
        << "] slides" << std::endl;

  // Trigger first slide
  this->dataPtr->currentIndex = 0;
  this->ChangeSlide();
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

  // Next (right arrow on keyboard or presenter)
  if ((_msg->int_value() == 16777236 || _msg->int_value() == 16777239) &&
      this->dataPtr->currentIndex + 1 < this->dataPtr->slideCount)
  {
    this->dataPtr->currentIndex++;
  }
  // Previous (left arrow on keyboard or presenter)
  else if ((_msg->int_value() == 16777234 || _msg->int_value() == 16777238) &&
      this->dataPtr->currentIndex >= 1)
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

  this->ChangeSlide();
}

/////////////////////////////////////////////////
void PresentMode::OnSlideChanged(int _slide)
{
  if (this->dataPtr->slideCount < 0)
    return;

  if (_slide > this->dataPtr->slideCount)
    this->dataPtr->currentIndex = this->dataPtr->slideCount - 1;

  this->dataPtr->currentIndex = _slide;

  this->ChangeSlide();
}

/////////////////////////////////////////////////
void PresentMode::ChangeSlide()
{
  gzmsg << "Change Slide: " << this->dataPtr->currentIndex << std::endl;

  ignition::math::Pose3d camPose;
  std::string toLookAt;

  // Reset presentation
  if (this->dataPtr->currentIndex == -1)
  {
    camPose = this->dataPtr->camera->InitialPose();
  }
  // Slides
  else
  {
    auto keyframe = simslides::keyframes[this->dataPtr->currentIndex];

    if (keyframe->HasType(KeyframeType::LOOKAT))
    {
      toLookAt = simslides::slidePrefix + "-" +
          std::to_string(this->dataPtr->currentIndex);
    }

    if (keyframe->HasType(KeyframeType::STACK))
    {
      // Find stack front
      auto front = this->dataPtr->currentIndex;
      while (front > 0 && simslides::keyframes[front-1]->HasType(KeyframeType::STACK))
        front--;

      // Find stack back
      auto back = this->dataPtr->currentIndex;
      while (back+1 < simslides::keyframes.size() &&
          simslides::keyframes[back+1]->HasType(KeyframeType::STACK))
      {
        back++;
      }

      // Get average position of all slides in stack
      ignition::math::Pose3d avgPose;
      std::vector<gazebo::rendering::VisualPtr> stackVis;
      for (int i = front; i <= back; ++i)
      {
        auto name = simslides::slidePrefix + "-" + std::to_string(i);

        auto vis = this->dataPtr->camera->GetScene()->GetVisual(name);

        if (!vis)
        {
          gzerr << "Couldn't find visual [" << name << "]" << std::endl;
          continue;
        }

        stackVis.push_back(vis);

        avgPose+= vis->WorldPose();
      }
      avgPose.Pos() = avgPose.Pos() / (back-front+1);
      avgPose.Rot() = stackVis[0]->WorldPose().Rot();

      // Make all other slides on the stack thinner
      for (int i = 0; i < stackVis.size(); ++i)
      {
        auto vis = stackVis[i];
        vis->SetPosition(avgPose.Pos());

        if (front + i == this->dataPtr->currentIndex)
          vis->SetScale(ignition::math::Vector3d(1, 1, 1));
        else
          vis->SetScale(ignition::math::Vector3d(0.5, 0.5, 0.5));
      }
    }
  }

  if (!toLookAt.empty())
  {
    std::string visName(toLookAt + "::link::visual");

    auto vis = this->dataPtr->camera->GetScene()->GetVisual(visName);
    if (!vis)
    {
      gzerr << "Failed to find visual [" << visName << "]" << std::endl;
      return;
    }

    auto size = vis->GetGeometrySize();

    // Target in world frame
    auto origin = vis->GetParent()->GetParent()->WorldPose();

    auto bb_pos = origin.Pos() + ignition::math::Vector3d(0, 0, size.Z()*0.5);
    auto target_world = ignition::math::Matrix4d(ignition::math::Pose3d(
        bb_pos, origin.Rot()));

    // Eye in target frame
    ignition::math::Matrix4d eye_target =
        ignition::math::Matrix4d(ignition::math::Pose3d(
            this->dataPtr->eyeOffsetX,
            -size.Z()*2,
            this->dataPtr->eyeOffsetZ,
            this->dataPtr->eyeOffsetRoll,
            this->dataPtr->eyeOffsetPitch,
            this->dataPtr->eyeOffsetYaw));

    // Eye in world frame
    auto eye_world = target_world * eye_target;

    // Look At
    auto mat = ignition::math::Matrix4d::LookAt(eye_world.Translation(),
        target_world.Translation());

    camPose = mat.Pose();
  }

  if ((this->dataPtr->camera->WorldPose().Pos() - camPose.Pos()).Length() > 0.001)
//      && (this->dataPtr->camera->WorldPose().Rot() - camPose.Rot()).Euler().Length() > 0.001)
  {
    this->dataPtr->camera->MoveToPosition(camPose, 1);
  }
  this->SlideChanged(this->dataPtr->currentIndex, this->dataPtr->slideCount-1);
}

