/*
 * node.cpp
 *
 *  Copyright (c) 2013 Nouka Technologies. All rights reserved.
 *
 *  This file is part of gps_odom.
 *
 *	Created on: 31/07/2014
 *		  Author: gareth
 */

#include <ros/package.h>
#include <gps_odom/node.hpp>
#include <nav_msgs/Odometry.h>

namespace gps_odom {

Node::Node() : nh_("~")
{
}

void Node::initialize() {
 
  //  IMU and GPS are synchronized separately
  subImu_.subscribe(nh_, "imu", kROSQueueSize);
  subPressure_.subscribe(nh_, "pressure",  kROSQueueSize);
  
  syncImu_ = std::make_shared<SynchronizerIMU>(TimeSyncIMU(kROSQueueSize), 
                                               subImu_, subPressure_);
  syncImu_->registerCallback( boost::bind(&Node::imuCallback, this, _1, _2) );
  
  subFix_.subscribe(nh_, "fix", kROSQueueSize);
  subFixVelocity_.subscribe(nh_, "fix_velocity", kROSQueueSize);
  
  syncGps_ = std::make_shared<SynchronizerGPS>(TimeSyncGPS(kROSQueueSize),
                                               subFix_, subFixVelocity_);
  syncGps_->registerCallback( boost::bind(&Node::gpsCallback, this, _1, _2) );
  
  //  advertise odometry as output
  pubOdometry_ = nh_.advertise<nav_msgs::Odometry>("odometry", 1);
}

void Node::imuCallback(const sensor_msgs::ImuConstPtr& imu,
                       const sensor_msgs::FluidPressureConstPtr& fluidPressure) {
  
}

void Node::gpsCallback(const sensor_msgs::NavSatFixConstPtr& navSatFix,
                       const geometry_msgs::Vector3StampedConstPtr& velVec) {
  
  const double lat = navSatFix->latitude;
  const double lon = navSatFix->longitude;
  const double hWGS84 = navSatFix->altitude;
  
  //  get path to geoids folder and load geoid if required
  if (!geoid_) {
    const std::string pkgName = "gps_odom";
    const std::string pkgPath = ros::package::getPath(pkgName);
    if (pkgPath.empty()) {
      ROS_ERROR("Failed to find path to package: %s", pkgName.c_str());
      return;
    }
   
    try {
      geoid_ = std::make_shared<GeographicLib::Geoid>("egm84-15", pkgPath + "/geoids");
    } catch (GeographicLib::GeographicErr& e) {
      ROS_ERROR("Failed to load geoid. Reason: %s", e.what());
      return;
    }
  }
  
  //  convert to height above sea level
  const double hMSL = geoid_->ConvertHeight(lat,lon,hWGS84,GeographicLib::Geoid::ELLIPSOIDTOGEOID);
  
  ROS_INFO("hMSL: %f", hMSL);
  
}
} //  namespace gps_odom
