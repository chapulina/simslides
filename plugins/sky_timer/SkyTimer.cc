#include <boost/bind.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <stdio.h>
#include <math.h>

namespace gazebo
{
  class SkyTimer : public WorldPlugin
  {
    /////////////////////////////////////////////////
    public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
    {
      // Get input params from SDF
      if (!_sdf->HasElement("total_minutes"))
      {
        gzerr << "Missing <total_minutes>" << std::endl;
        return;
      }

      this->totalMinutes = _sdf->Get<int>("total_minutes");

      // Initialize transport
      this->node = gazebo::transport::NodePtr(
          new gazebo::transport::Node());
      this->node->Init();

      this->skyPub = this->node->Advertise<gazebo::msgs::Sky>("~/sky");

      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&SkyTimer::OnUpdate, this, std::placeholders::_1));
    }

    /////////////////////////////////////////////////
    void OnUpdate(const common::UpdateInfo &_info)
    {
      static int currentSunTime = 0;
      int sunrise = 8;
      int sunset = 18;

      auto currentMinutes = _info.realTime.Double() / 60;

      auto newSunTime = currentMinutes * (sunset - sunrise) /
                        this->totalMinutes;

      if (newSunTime <= currentSunTime)
        return;

      currentSunTime = ceil(newSunTime);

      // Sky
      gazebo::msgs::Sky skyMsg;
      skyMsg.set_time(currentSunTime + sunrise);
      this->skyPub->Publish(skyMsg);

      gzmsg << "Sky time [" << currentSunTime + sunrise << "]"
            << std::endl;
    }

    private: int totalMinutes;
    private: event::ConnectionPtr updateConnection;
    private: gazebo::transport::NodePtr node;
    private: gazebo::transport::PublisherPtr skyPub;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_WORLD_PLUGIN(SkyTimer)
}

