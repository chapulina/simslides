/*
 * Copyright 2016 Louise Poubel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <limits>

#include "include/simslides/common/Common.hh"

simslides::Common *simslides::Common::instance = nullptr;

/////////////////////////////////////////////////
void simslides::Common::LoadPluginSDF(const sdf::ElementPtr _sdf)
{
  if (_sdf->HasElement("far_clip") && _sdf->HasElement("near_clip"))
  {
    this->farClip = _sdf->Get<double>("far_clip");
    this->nearClip = _sdf->Get<double>("near_clip");
  }
  else
  {
    this->farClip = std::nan("");
    this->nearClip = std::nan("");
  }

  if (_sdf->HasElement("keyframe"))
  {
    auto keyframeElem = _sdf->GetElement("keyframe");
    while (keyframeElem)
    {
      this->keyframes.push_back(new Keyframe(keyframeElem));
      keyframeElem = keyframeElem->GetNextElement("keyframe");
    }
  }

  if (this->keyframes.size() == 0)
  {
    std::cerr << "No keyframes were loaded." << std::endl;
  }
}

/////////////////////////////////////////////////
void simslides::Common::HandleKeyPress(int _key)
{
  if (this->keyframes.empty())
    return;

  // Next (right arrow on keyboard or presenter)
  if ((_key == 16777236 || _key == 16777239) &&
      this->currentKeyframe + 1 < this->keyframes.size())
  {
    this->currentKeyframe++;
  }
  // Previous (left arrow on keyboard or presenter)
  else if ((_key == 16777234 || _key == 16777238) &&
      this->currentKeyframe >= 1)
  {
    this->currentKeyframe--;
  }
  // Current (F1)
  else if (_key == 16777264)
  {
  }
  // Home (F6)
  else if (_key == 16777269)
  {
    this->currentKeyframe = -1;
  }
  else
    return;
}

/////////////////////////////////////////////////
void simslides::Common::ChangeKeyframe(int _keyframe)
{
  if (this->keyframes.empty())
    return;

  if (_keyframe > this->keyframes.size())
    this->currentKeyframe = this->keyframes.size() - 1;
  else
    this->currentKeyframe = _keyframe;
}

/////////////////////////////////////////////////
void simslides::Common::Update()
{
  // Reset presentation
  if (this->currentKeyframe < 0)
  {
    this->Common::Instance()->ResetCameraPose();
    return;
  }

  auto keyframe = this->keyframes[this->currentKeyframe];

  // Set text
  this->Common::Instance()->SetText(keyframe->Text());

  // Log seek
  if (keyframe->GetType() == KeyframeType::LOG_SEEK)
  {
    this->Common::Instance()->MoveCamera(keyframe->CamPose());
    this->Common::Instance()->SeekLog(keyframe->LogSeek());
    return;
  }

  // Cam pose
  if (keyframe->GetType() == KeyframeType::CAM_POSE)
  {
    this->Common::Instance()->MoveCamera(keyframe->CamPose());
    return;
  }

  // Look at
  if (keyframe->GetType() == KeyframeType::LOOKAT ||
      keyframe->GetType() == KeyframeType::STACK)
  {
    // Target in world frame
    auto origin = this->Common::Instance()->VisualPose(keyframe->Visual());

    auto bbPos = origin.Pos() + ignition::math::Vector3d(0, 0, 0.5);
    auto targetWorld = ignition::math::Matrix4d(ignition::math::Pose3d(
        bbPos, origin.Rot()));

    // Eye in target frame
    auto offset = keyframe->EyeOffset();
    if (offset == ignition::math::Pose3d::Zero)
    {
      offset = this->kEyeOffset;
    }
    ignition::math::Matrix4d eyeTarget(offset);

    // Eye in world frame
    auto eyeWorld = targetWorld * eyeTarget;

    // Look At
    auto mat = ignition::math::Matrix4d::LookAt(eyeWorld.Translation(),
        targetWorld.Translation());

    this->MoveCamera(mat.Pose());
  }

  // Set stack visibility
  if (keyframe->GetType() == KeyframeType::STACK)
  {
    auto frontKeyframe = this->currentKeyframe;
    while (frontKeyframe > 0 &&
        this->keyframes[frontKeyframe-1]->GetType() == KeyframeType::STACK)
    {
      frontKeyframe--;
    }

    auto backKeyframe = this->currentKeyframe;
    while (backKeyframe + 1 < this->keyframes.size() &&
        this->keyframes[backKeyframe+1]->GetType() == KeyframeType::STACK)
    {
      backKeyframe++;
    }

    std::cout << "Stack front [" << frontKeyframe << "], back [" << backKeyframe
              << "]" << std::endl;

    for (int i = frontKeyframe; i <= backKeyframe; ++i)
    {
      auto name = this->keyframes[i]->Visual();
      this->SetVisualVisible(name, name == keyframe->Visual());
    }
  }

}
