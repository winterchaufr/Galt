/*
 * thermal_map_node.cpp
 *  _   _             _           _____         _
 * | \ | | ___  _   _| | ____ _  |_   _|__  ___| |__
 * |  \| |/ _ \| | | | |/ / _` |   | |/ _ \/ __| '_ \
 * | |\  | (_) | |_| |   < (_| |   | |  __/ (__| | | |
 * |_| \_|\___/ \__,_|_|\_\__,_|   |_|\___|\___|_| |_|
 *
 *  Copyright (c) 2014 Nouka Technologies. All rights reserved.
 *
 *  This file is part of thermal_map.
 *
 *	Created on: 23/08/2014
 */

#include "thermal_map/thermal_map_node.h"

#include <geometry_msgs/Quaternion.h>
#include <cv_bridge/cv_bridge.h>
#include <visualization_msgs/Marker.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace galt {
namespace thermal_map {

ThermalMapNode::ThermalMapNode(const ros::NodeHandle &nh) : nh_{nh}, it_{nh} {
  image_transport::TransportHints hints("raw", ros::TransportHints(), nh_);
  sub_camera_ = it_.subscribeCamera("color_map", 1, &ThermalMapNode::CameraCb,
                                    this, hints);
  sub_odom_ = nh_.subscribe("odometry", 1, &ThermalMapNode::OdomCb, this);
  sub_laser_ = nh_.subscribe("scan", 1, &ThermalMapNode::LaserCb, this);
  pub_traj_ = nh_.advertise<visualization_msgs::Marker>("trajectory", 1);
  std_msgs::ColorRGBA traj_color;
  traj_color.r = 1;
  traj_color.a = 1;
  viz_traj_ = TrajectoryVisualizer(pub_traj_, traj_color, 0.2, "line");
}

void ThermalMapNode::OdomCb(const nav_msgs::OdometryConstPtr &odom_msg) {
  const geometry_msgs::Point &position = odom_msg->pose.pose.position;
  const geometry_msgs::Quaternion &quat = odom_msg->pose.pose.orientation;
  tf::Transform transform;
  transform.setOrigin(tf::Vector3(position.x, position.y, position.z));
  tf::Quaternion q(quat.x, quat.y, quat.z, quat.w);
  transform.setRotation(q);
  broadcaster_.sendTransform(
      tf::StampedTransform(transform, odom_msg->header.stamp,
                           std::string("world"), std::string("imu")));

  // Publish trajectory
  viz_traj_.PublishTrajectory(position, odom_msg->header);
}

void ThermalMapNode::LaserCb(const sensor_msgs::LaserScanConstPtr &scan_msg) {
}

void ThermalMapNode::CameraCb(
    const sensor_msgs::ImageConstPtr &image_msg,
    const sensor_msgs::CameraInfoConstPtr &cinfo_msg) {
  const cv::Mat image = cv_bridge::toCvShare(image_msg)->image;

  cv::imshow("image", image);
  cv::waitKey(1);
}

}  // namespace thermal_map
}  // namespace galt
