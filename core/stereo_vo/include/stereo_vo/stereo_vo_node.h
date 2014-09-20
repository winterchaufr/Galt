#ifndef GALT_STEREO_VO_NODE_H_
#define GALT_STEREO_VO_NODE_H_

#include <utility>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>
#include <geometry_msgs/PoseStamped.h>
#include <cv_bridge/cv_bridge.h>
#include <image_geometry/stereo_camera_model.h>
#include <dynamic_reconfigure/server.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <stereo_vo/StereoVoDynConfig.h>

#include <rviz_helper/visualizer.h>

#include "stereo_vo/stereo_vo.h"
#include "stereo_vo/common.h"

namespace galt {
namespace stereo_vo {

using sensor_msgs::Image;
using sensor_msgs::CameraInfo;
using sensor_msgs::ImageConstPtr;
using sensor_msgs::CameraInfoConstPtr;
using message_filters::sync_policies::ExactTime;

class StereoVoNode {
 public:
  StereoVoNode(const ros::NodeHandle& nh);

 private:
  using CinfoSubscriberFilter = message_filters::Subscriber<CameraInfo>;
  using ExactPolicy = ExactTime<Image, CameraInfo, Image, CameraInfo>;
  using ExactSync = message_filters::Synchronizer<ExactPolicy>;

  void SubscribeStereoTopics(const std::string& image_topic,
                             const std::string& cinfo_topic,
                             const std::string& transport);

  //  void OdometryCb(const nav_msgs::OdometryConstPtr& odom_msg);

  void StereoCb(const ImageConstPtr& l_image_msg,
                const CameraInfoConstPtr& l_cinfo_msg,
                const ImageConstPtr& r_image_msg,
                const CameraInfoConstPtr& r_cinfo_msg);

  void ConfigCb(const StereoVoDynConfig& config, int level);

  ros::NodeHandle nh_;                              ///< Private nodehandle
  image_transport::ImageTransport it_;              ///< Private image transport
  image_transport::SubscriberFilter sub_l_image_;   ///< Left image subscriber
  image_transport::SubscriberFilter sub_r_image_;   ///< Right image subscriber
  CinfoSubscriberFilter sub_l_cinfo_;               ///< Left cinfo subscriber
  CinfoSubscriberFilter sub_r_cinfo_;               ///< Right cinfo subscriber
  boost::shared_ptr<ExactSync> exact_sync_;         ///< Exact time sync policy
  dynamic_reconfigure::Server<StereoVoDynConfig> cfg_server_;
  stereo_vo::StereoVo stereo_vo_;
  //  std::string frame_id_;
  //  ros::Subscriber sub_odom_;
  //  ros::Publisher pub_point_;
  //  ros::Publisher pub_pose_;
  //  visualization_msgs::Marker traj_;
  //  kr::rviz_helper::TfPublisher tf_pub_;
  //  kr::rviz_helper::TrajectoryVisualizer traj_viz_;
  //  tf2::BufferCore core_;
  //  tf2_ros::TransformListener tf_listener_;

  /// Visualize point cloud of triangulated points
  //  void PublishPointCloud(const ros::Time& time,
  //                         const std::string& frame_id = "world") const;

  //  void PublishPointCloud(const std::map<Id, Point3d>& point3ds,
  //                         const std::deque<FramePtr>& key_frames,
  //                         const ros::Time& time,
  //                         const std::string& frame_id) const;
  //  void PublishTrajectory(const geometry_msgs::Pose& pose, const ros::Time&
  // time,
  //                         const std::string& frame_id);
};

CvStereoImage FromImage(const ImageConstPtr& l_image_msg,
                        const ImageConstPtr& r_image_msg);

}  // namespace stereo_vo
}  // namespace galt

#endif  // GALT_STEREO_VO_NODE_H_
