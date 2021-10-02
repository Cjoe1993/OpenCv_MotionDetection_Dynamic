# OpenCv_MotionDetection_Dynamic
Implementing motion detection using OpenCV with C++ using a static background image that updates with the current frame.

The first frame image is taken, however this means that motion is often detected by trivial pixel changes, such as small changes in lighting or reflections. To combat this,
the first frame, aka the frame to be compared to, is reset with the current frame about once a second (increased by changing the counter check).

