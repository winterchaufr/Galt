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

#include <cstdint>
#include <cmath>

#include <geometry_msgs/Quaternion.h>
#include <cv_bridge/cv_bridge.h>
#include <visualization_msgs/Marker.h>
#include <laser_assembler/AssembleScans.h>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace galt {
namespace thermal_map {

ThermalMapNode::ThermalMapNode(const ros::NodeHandle &nh) : nh_{nh}, it_{nh} {
  image_transport::TransportHints hints("raw", ros::TransportHints(), nh_);
  sub_camera_ = it_.subscribeCamera("color_map", 1, &ThermalMapNode::CameraCb,
                                    this, hints);
  client_ = nh_.serviceClient<laser_assembler::AssembleScans>("assemble_scans");
  pub_cloud_ = nh_.advertise<sensor_msgs::PointCloud>("thermal_cloud", 1);
}

void ThermalMapNode::ConnectCb() {}

void ThermalMapNode::CameraCb(
    const sensor_msgs::ImageConstPtr &image_msg,
    const sensor_msgs::CameraInfoConstPtr &cinfo_msg) {
  // Get point cloud from laser assembler
  static ros::Time prev_time(0);
  if (prev_time == ros::Time(0)) {
    prev_time = image_msg->header.stamp - ros::Duration(kDelay);
    return;
  }
  laser_assembler::AssembleScans srv;
  const ros::Time &curr_time = image_msg->header.stamp - ros::Duration(kDelay);
  srv.request.end = curr_time;
  srv.request.begin = prev_time;
  prev_time = curr_time;

  if (!client_.call(srv)) {
    ROS_WARN("Sercie call failed");
    return;
  }

  if (srv.response.cloud.points.empty()) {
    ROS_WARN("Empty cloud");
    return;
  }

  // Project point cloud back into thermal image and get color
  sensor_msgs::PointCloud thermal_cloud;
  const sensor_msgs::PointCloud &laser_cloud = srv.response.cloud;
  camera_model_.fromCameraInfo(cinfo_msg);
  const cv::Mat image = cv_bridge::toCvShare(image_msg)->image;
  ProjectCloud(laser_cloud, image, camera_model_, thermal_cloud);
  // Publish point cloud
  pub_cloud_.publish(thermal_cloud);
}

void ThermalMapNode::ProjectCloud(
    const sensor_msgs::PointCloud &cloud_in, const cv::Mat &image,
    const image_geometry::PinholeCameraModel &model,
    sensor_msgs::PointCloud &cloud_out) const {
  std::vector<cv::Point3f> camera_points;
  CloudToPoints(cloud_in, camera_points);
  std::vector<cv::Point2f> image_points;
  cv::Mat zeros = cv::Mat::zeros(1, 3, CV_64FC1);
  cv::projectPoints(camera_points, zeros, zeros, model.fullIntrinsicMatrix(),
                    model.distortionCoeffs(), image_points);
  PixelsToCloud(image, image_points, cloud_in, cloud_out);
}

void ThermalMapNode::PixelsToCloud(const cv::Mat &image,
                                   const std::vector<cv::Point2f> &pixels,
                                   const sensor_msgs::PointCloud cloud_in,
                                   sensor_msgs::PointCloud &cloud_out) const {
  cloud_out.header = cloud_in.header;
  sensor_msgs::ChannelFloat32 channel;
  channel.name = "rgb";
  union {
    uint8_t rgb[4];
    float val;
  } color;

  // Go through each pixel, add to cloud_out if it's inside the image
  for (decltype(pixels.size()) i = 0, e = pixels.size(); i != e; ++i) {
    const cv::Point2f &pixel = pixels[i];
    cv::Point2i pixel_rounded(pixel.x, pixel.y);
    if (InsideImage(image.size(), pixel_rounded)) {
      cloud_out.points.push_back(cloud_in.points[i]);
      // Get rgb data from image
      const uchar *p = image.ptr<uchar>(pixel_rounded.y);
      int col = pixel.x;
      color.rgb[0] = p[3 * col];
      color.rgb[1] = p[3 * col + 1];
      color.rgb[2] = p[3 * col + 2];
      color.rgb[3] = 0;
      channel.values.push_back(color.val);
    }
  }
  cloud_out.channels.push_back(channel);
}

void ThermalMapNode::CloudToPoints(const sensor_msgs::PointCloud &cloud,
                                   std::vector<cv::Point3f> &points) const {
  for (const geometry_msgs::Point32 &point : cloud.points) {
    points.push_back(cv::Point3f(point.x, point.y, point.z));
  }
}

}  // namespace thermal_map
}  // namespace galt
