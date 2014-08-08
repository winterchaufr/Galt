#ifndef GALT_STEREO_VO_COMMON_H_
#define GALT_STEREO_VO_COMMON_H_

#include "kr_math/pose.hpp"
#include <opencv2/core/core.hpp>

namespace galt {
namespace stereo_vo {

using scalar_t = float;
using Pose = kr::Pose<scalar_t>;
using CvPoint2 = cv::Point_<scalar_t>;
using Corners2 = std::vector<CvPoint2>;
using CvPoint3 = cv::Point3_<scalar_t>;

}  // namespace stereo_vo

}  // namespace galt

#endif  // GALT_STEREO_VO_COMMON_H_
