#ifndef IMAGEANALYSER_H
#define IMAGEANALYSER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <imageobject.h>
#include <opencv2/stitching/stitcher.hpp>
#include <utils.h>
#include <bfsvertexvisitor.h>


using namespace std;
using namespace cv;
class Imageobject;

class ImageAnalyser {
public:
    ImageAnalyser(std::vector<Imageobject> *imageVector);
    ~ImageAnalyser();

    void calculateDescriptors();
    void analyse();
    Graph *findPanoramas();
    bool hasIntersections(std::vector<int> v1, std::vector<int> v2);
    bool verifyImage(int id1, int id2, vector<DMatch> matches);

private:
    Ptr<DescriptorMatcher> matcher;
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;
    std::vector<Imageobject> *imageVector_;
};


#endif