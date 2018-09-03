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
#include <boost/filesystem.hpp>
#include <gazebo/common/Console.hh>
#include <gazebo/transport/Node.hh>

#include "Common.hh"

std::string simslides::slidePrefix;
std::string simslides::slidePath;
std::vector<simslides::Keyframe *> simslides::keyframes;

/////////////////////////////////////////////////
void simslides::LoadSlides()
{
  if (simslides::slidePrefix.empty() || simslides::slidePath.empty())
  {
    gzerr << "Missing slide prefix or path." << std::endl;
    return;
  }

  // Count number of models in path
  int count = std::count_if(
      boost::filesystem::directory_iterator(simslides::slidePath),
      boost::filesystem::directory_iterator(),
      boost::bind( static_cast<bool(*)(const boost::filesystem::path&)>(
      boost::filesystem::is_directory),
        boost::bind( &boost::filesystem::directory_entry::path, _1 ) ) );

  // Setup transport
  auto node = gazebo::transport::NodePtr(new gazebo::transport::Node());
  node->Init();
  auto factoryPub =
       node->Advertise<gazebo::msgs::Factory>("/gazebo/default/factory");

  int countX= 0;
  int countY= 0;
  for (int i = 0; i < count; ++i)
  {
    gazebo::msgs::Factory msg;

    std::string filename("file://" + simslides::slidePath + "/" +
        simslides::slidePrefix + "-" + std::to_string(i));
    msg.set_sdf_filename(filename);

    gazebo::msgs::Set(msg.mutable_pose(),
        ignition::math::Pose3d(countX, countY, 0, 0, 0, 0));

    factoryPub->Publish(msg);

    if (countX > 30)
    {
      countX = 0;
      countY = countY + 10;
    }
    else
    {
      countX = countX + 10;
    }
  }

  factoryPub.reset();
  node->Fini();
}
