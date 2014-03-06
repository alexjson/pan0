
#ifndef UTILS_H
#define UTILS_H
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>


double get_wall_time();
double get_cpu_time();

double eDistance(std::vector<int> vec1, std::vector<int> vec2);


#endif // UTILS_H

