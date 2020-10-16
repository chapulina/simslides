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

std::string simslides::slidePrefix;
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
