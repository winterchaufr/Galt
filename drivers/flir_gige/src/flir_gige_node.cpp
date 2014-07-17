#include <memory>
#include <stdexcept>

#include <ros/ros.h>
#include <ros/node_handle.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <dynamic_reconfigure/server.h>

#include "flir_gige/gige_camera.h"
#include "flir_gige/FlirConfig.h"

namespace flir_gige {

class FlirNode {
 private:
  // ROS related
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Publisher image_pub_;
  sensor_msgs::ImagePtr image_msg_;
  dynamic_reconfigure::Server<flir_gige::FlirConfig> server_;
  // Flir Camera
  std::shared_ptr<flir_gige::GigeCamera> camera_;

 public:
  FlirNode(const ros::NodeHandle &nh) : nh_{nh}, it_{nh} {
    // Create a camera
    std::string ip_address;
    nh_.param("ip_address", ip_address, std::string(""));
    camera_ = std::make_shared<flir_gige::GigeCamera>(ip_address);
    // Setup image publisher and dynamic reconfigure callback
    image_pub_ = it_.advertise("image_raw", 1);
    server_.setCallback(
        boost::bind(&FlirNode::ReconfigureCallback, this, _1, _2));
  }

  FlirNode(const FlirNode &) = delete;             // No copy constructor
  FlirNode &operator=(const FlirNode &) = delete;  // No assignment operator

  bool init() {
    camera_->FindDevice();
    camera_->ConnectDevice();
    camera_->OpenStream();
    camera_->ConfigureStream();
    camera_->CreatePipeline();
    camera_->AcquireImages();
    return true;
  }

  void ReconfigureCallback(flir_gige::FlirConfig &config, int level) {
    ROS_INFO("%s", "In reconfigure callback");
    ROS_INFO_STREAM("Level " << level << "Width " << config.width
                             << " Height " << config.height);
  }
};

}  // namespace flir_gige

int main(int argc, char *argv[]) {
  ros::init(argc, argv, "flir_node");

  ros::NodeHandle nh("~");
  try {
    flir_gige::FlirNode fn(nh);
    fn.init();
    ros::spin();
  }
  catch (const std::exception &e) {
    ROS_ERROR_STREAM(e.what());
  }

  return 0;
}
