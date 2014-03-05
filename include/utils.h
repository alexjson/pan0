#ifndef UTILS_H
#define UTILS_H
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>




//-------------------------------------------------------------------------------------
// Note Translation is assumed to be zero during the whole stitching pipeline.

struct CV_EXPORTS CameraParams {
    CameraParams();
    CameraParams(const CameraParams &other);
    const CameraParams &operator =(const CameraParams &other);
    cv::Mat K() const;

    double focal; // Focal length ~648? mm? pixlar?
    double aspect; // Aspect ratio width/height
    double ppx; // Principal point X  ImagecenterX width/2
    double ppy; // Principal point Y  ImagecenterY height/2
    cv::Mat R; // Rotation Matrix 3x3? 2x2?
    // Note Translation is assumed to be zero during the whole stitching pipeline.
    cv::Mat t; // Translation
};

double get_wall_time();
double get_cpu_time();

double eDistance(std::vector<int> vec1, std::vector<int> vec2);


#endif // UTILS_H

