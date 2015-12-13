from __future__ import print_function, division, absolute_import
import rosbag
import cv2
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import matplotlib.pyplot as plt
from sklearn.externals import joblib
from aye import *

bagfile = '/home/chao/Workspace/bag/frame/rect_fixed/frame4_rect_fixed.bag'

# load
clf = joblib.load('../model/svc.pkl')
scaler = joblib.load('../model/scaler.pkl')
im_topic = '/color/image_rect_color'

detector = FruitDetector(clf, scaler)

bridge = CvBridge()
fig = plt.figure()
plt.ion()
ax_bgr = fig.add_subplot(121)
ax_bw = fig.add_subplot(122)
h_bgr = None
h_bw = None
with rosbag.Bag(bagfile) as bag:
    for i, (topic, msg, t) in enumerate(bag.read_messages(im_topic)):
        try:
            im_bgr = bridge.imgmsg_to_cv2(msg)
            # Rotate image 90 degree
            im_bgr = im_bgr[200:1000, :1480, :]
            im_bgr = rotate_image(im_bgr)
            # Use a small portion of the image here?

        except CvBridgeError as e:
            print(e)
            continue

        # Detect
        s, bw = detector.detect(im_bgr)

        # Clean up bw image for blob analysis
        stats, bw_filled = region_props(bw)

        # Input to BlobAnalyser and get back a bunch of bounding boxes

        # Give bounding boxes and image and timestamp to tracker for tracking

        # Visualize result
        b, g, r = cv2.split(s.im_raw)
        im_rgb = cv2.merge([r, g, b])

        if h_bgr:
            h_bw.set_data(bw_filled)
            h_bgr.set_data(im_rgb)
        else:
            h_bw = ax_bw.imshow(bw_filled, cmap=plt.cm.gray)
            h_bgr = ax_bgr.imshow(im_rgb)
        plt.pause(0.001)
