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

std::string simslides::slidePath;
std::vector<simslides::Keyframe *> simslides::keyframes;
double simslides::nearClip{std::numeric_limits<double>::quiet_NaN()};
double simslides::farClip{std::numeric_limits<double>::quiet_NaN()};
int simslides::currentKeyframe{-1};
int simslides::slideCount{-1};
const double simslides::kEyeOffsetX{0.0};
const double simslides::kEyeOffsetY{-3.0};
const double simslides::kEyeOffsetZ{0.0};
const double simslides::kEyeOffsetRoll{0.0};
const double simslides::kEyeOffsetPitch{0.0};
const double simslides::kEyeOffsetYaw{IGN_PI_2};

/////////////////////////////////////////////////
void simslides::LoadPluginSDF(const sdf::ElementPtr _sdf)
{
  if (_sdf->HasElement("far_clip") && _sdf->HasElement("near_clip"))
  {
    simslides::farClip = _sdf->Get<double>("far_clip");
    simslides::nearClip = _sdf->Get<double>("near_clip");
  }
  else
  {
    simslides::farClip = std::nan("");
    simslides::nearClip = std::nan("");
  }

  if (_sdf->HasElement("keyframe"))
  {
    auto keyframeElem = _sdf->GetElement("keyframe");
    while (keyframeElem)
    {
      simslides::keyframes.push_back(new Keyframe(keyframeElem));
      keyframeElem = keyframeElem->GetNextElement("keyframe");
    }
  }

  if (simslides::keyframes.size() == 0)
  {
    std::cerr << "No keyframes were loaded." << std::endl;
  }
}

/////////////////////////////////////////////////
void simslides::HandleKeyPress(int _key)
{
  if (simslides::keyframes.empty())
    return;

  // Next (right arrow on keyboard or presenter)
  if ((_key == 16777236 || _key == 16777239) &&
      simslides::currentKeyframe + 1 < simslides::keyframes.size())
  {
    simslides::currentKeyframe++;
  }
  // Previous (left arrow on keyboard or presenter)
  else if ((_key == 16777234 || _key == 16777238) &&
      simslides::currentKeyframe >= 1)
  {
    simslides::currentKeyframe--;
  }
  // Current (F1)
  else if (_key == 16777264)
  {
  }
  // Home (F6)
  else if (_key == 16777269)
  {
    simslides::currentKeyframe = -1;
  }
  else
    return;
}

/////////////////////////////////////////////////
void simslides::ChangeKeyframe(int _keyframe)
{
  if (simslides::keyframes.empty())
    return;

  if (_keyframe > simslides::keyframes.size())
    simslides::currentKeyframe = simslides::keyframes.size() - 1;
  else
    simslides::currentKeyframe = _keyframe;
}

/////////////////////////////////////////////////
void simslides::Common::Update()
{
  // Reset presentation
  if (simslides::currentKeyframe < 0)
  {
    auto initialPose = simslides::Common::Instance()->initialCameraPose();
    if (!std::isnan(initialPose.Pos().X()))
      simslides::Common::Instance()->moveCamera(initialPose);
    return;
  }

  auto keyframe = simslides::keyframes[simslides::currentKeyframe];

  // Set text
  simslides::Common::Instance()->SetText(keyframe->Text());

  // Log seek
  if (keyframe->GetType() == KeyframeType::LOG_SEEK)
  {
    simslides::Common::Instance()->moveCamera(keyframe->CamPose());
    simslides::Common::Instance()->seekLog(keyframe->LogSeek());
    return;
  }

  // Cam pose
  if (keyframe->GetType() == KeyframeType::CAM_POSE)
  {
    simslides::Common::Instance()->moveCamera(keyframe->CamPose());
    return;
  }

  // Look at
  if (keyframe->GetType() == KeyframeType::LOOKAT ||
      keyframe->GetType() == KeyframeType::STACK)
  {
    // Target in world frame
    auto origin = simslides::Common::Instance()->visualPose(keyframe->Visual());

    auto bbPos = origin.Pos() + ignition::math::Vector3d(0, 0, 0.5);
    auto targetWorld = ignition::math::Matrix4d(ignition::math::Pose3d(
        bbPos, origin.Rot()));

    // Eye in target frame
    auto offset = keyframe->EyeOffset();
    if (offset == ignition::math::Pose3d::Zero)
    {
      offset = ignition::math::Pose3d(
               simslides::kEyeOffsetX,
               simslides::kEyeOffsetY,
               simslides::kEyeOffsetZ,
               simslides::kEyeOffsetRoll,
               simslides::kEyeOffsetPitch,
               simslides::kEyeOffsetYaw);
    }
    ignition::math::Matrix4d eyeTarget(offset);

    // Eye in world frame
    auto eyeWorld = targetWorld * eyeTarget;

    // Look At
    auto mat = ignition::math::Matrix4d::LookAt(eyeWorld.Translation(),
        targetWorld.Translation());

    simslides::Common::Instance()->moveCamera(mat.Pose());
  }

  // Set stack visibility
  if (keyframe->GetType() == KeyframeType::STACK)
  {
    auto frontKeyframe = simslides::currentKeyframe;
    while (frontKeyframe > 0 &&
        simslides::keyframes[frontKeyframe-1]->GetType() == KeyframeType::STACK)
    {
      frontKeyframe--;
    }

    auto backKeyframe = simslides::currentKeyframe;
    while (backKeyframe + 1 < simslides::keyframes.size() &&
        simslides::keyframes[backKeyframe+1]->GetType() == KeyframeType::STACK)
    {
      backKeyframe++;
    }

    std::cout << "Stack front [" << frontKeyframe << "], back [" << backKeyframe
              << "]" << std::endl;

    for (int i = frontKeyframe; i <= backKeyframe; ++i)
    {
      auto name = simslides::keyframes[i]->Visual();
      simslides::Common::Instance()->setVisualVisible(name,
          name == keyframe->Visual());
    }
  }

}
