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

#include "include/simslides/common/Common.hh"

std::string simslides::slidePrefix;
std::string simslides::slidePath;
std::vector<simslides::Keyframe *> simslides::keyframes;
double simslides::nearClip;
double simslides::farClip;

/////////////////////////////////////////////////
void simslides::LoadPluginSDF(const sdf::ElementPtr _sdf)
{
  // TODO Deprecate prefix in favor of listing all visual names
  if (_sdf->HasElement("slide_prefix"))
  {
    simslides::slidePrefix = _sdf->Get<std::string>("slide_prefix");
  }

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
}
