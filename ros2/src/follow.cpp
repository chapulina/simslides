// Copyright 2018 Louise Poubel.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using namespace std::chrono_literals;

class Follow : public rclcpp::Node
{
public:
  explicit Follow() : Node("follow")
  {
    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel");

    auto onSensorMsg =
      [this](const sensor_msgs::msg::LaserScan::SharedPtr msg) -> void
      {
        auto cmd_msg = std::make_shared<geometry_msgs::msg::Twist>();

        // Find closest hit
        float min_range = msg->range_max + 1;
        int idx = -1;
        for (int i = 0; i < msg->ranges.size(); ++i)
        {
          auto range = msg->ranges[i];
          if (range > msg->range_min && range < msg->range_max && range < min_range)
          {
            min_range = range;
            idx = i;
          }
        }

        double turn = msg->angle_min + msg->angle_increment * idx;
        cmd_msg->angular.z = turn * 0.08;

        // Too close, stop
        if (idx < 0 || min_range <= min_dist_)
        {
          cmd_msg->linear.x = 0;
        }
        else
        {
          cmd_msg->linear.x = 0.02 / abs(turn);
        }

        cmd_pub_->publish(cmd_msg);
      };
    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("laser_scan", onSensorMsg);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  double min_dist_ = 1.0;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<Follow>();

  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}

