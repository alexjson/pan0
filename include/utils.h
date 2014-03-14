
#ifndef UTILS_H
#define UTILS_H
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <imageobject.h>
#include <boost/date_time/posix_time/posix_time.hpp>


double get_wall_time();
double get_cpu_time();

double eDistance(std::vector<int> vec1, std::vector<int> vec2);
bool checkTimeDiff(int id1, int id2, std::vector<Imageobject>* imageVector);
bool checkMagDiff(int id1, int id2,    vector<Imageobject> *imageVector);
bool CheckTiltDiff(int id1, int id2,    vector<Imageobject> *imageVector);


#endif // UTILS_H

