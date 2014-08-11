#ifndef GALT_STEREO_VO_UTILS_H_
#define GALT_STEREO_VO_UTILS_H_

#include "stereo_vo/common.h"
#include "stereo_vo/feature.h"
#include "stereo_vo/key_frame.h"

#include <deque>

namespace galt {

namespace stereo_vo {

namespace cv_color {
const cv::Scalar BLUE = cv::Scalar(255, 0, 0);
const cv::Scalar GREEN = cv::Scalar(0, 255, 0);
const cv::Scalar RED = cv::Scalar(0, 0, 255);
const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
const cv::Scalar ORANGE = cv::Scalar(0, 128, 255);
const cv::Scalar MAGENTA = cv::Scalar(255, 0, 255);
const cv::Scalar CYAN = cv::Scalar(255, 255, 0);
}

template <typename T, typename U>
void PruneByStatus(const std::vector<U> &status, std::vector<T> &objects,
                   std::vector<T> &removed) {
  ROS_ASSERT_MSG(status.size() == objects.size(),
                 "status and object size mismatch");
  auto it_obj = objects.begin();
  for (const auto &s : status) {
    if (s) {
      it_obj++;
    } else {
      removed.push_back(*it_obj);
      it_obj = objects.erase(it_obj);
    }
  }
}

template <typename T, typename U>
void PruneByStatus(const std::vector<U> &status, std::vector<T> &objects) {
  ROS_ASSERT_MSG(status.size() == objects.size(),
                 "status and object size mismatch");
  auto it_obj = objects.begin();
  for (const auto &s : status) {
    if (s) {
      it_obj++;
    } else {
      it_obj = objects.erase(it_obj);
    }
  }
}

void Display(const CvStereoImage &stereo_image,
             const std::vector<Corner> &corners, const KeyFrame &key_frame);

}  // namespace stereo_vo

}  // namespace galt

#endif  // GALT_STEREO_VO_UTILS_H_
