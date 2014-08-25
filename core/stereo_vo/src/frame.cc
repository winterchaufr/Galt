#include "stereo_vo/frame.h"
#include "stereo_vo/feature.h"
#include "stereo_vo/utils.h"

namespace galt {

namespace stereo_vo {

Frame::Frame(const CvStereoImage &stereo_image)
    : is_keyframe_(false), stereo_image_(stereo_image) {
  static Id frame_counter = 0;
  id_ = frame_counter++;
}

size_t Frame::RemoveById(const std::set<Id> &ids_to_remove, bool force) {
//  features_.erase(
//      std::remove_if(features_.begin(), features_.end(), [&](const Feature &f) {
//        return (f.init() &&
//                (ids_to_remove.find(f.id()) != ids_to_remove.end()));
//      }),
//      features_.end());

  size_t erased=0;
  for (auto ite = features_.begin(); ite != features_.end();) {
    bool erase = false;
    if (ite->init() || force) {
      if (ids_to_remove.find(ite->id()) != ids_to_remove.end()) {
        erase = true;
      }
    }
    if (erase) {
      ite = features_.erase(ite);
      erased++;
    } else {
      ite++;
    }
  }
  
  if (features_.empty()) {
    ROS_WARN("Frame %lu is empty", id());
  }
  
  return erased;
}

}  // namespace stereo_vo

}  // namespace galt