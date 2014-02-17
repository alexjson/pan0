#ifndef IMAGEANALYSER_H
#define IMAGEANALYSER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include </home/alex/xjobb/c++/include/imageobject.h>

using namespace std;
using namespace cv;
class ImageObject;

class ImageAnalyser {
public:
    ImageAnalyser();
    ~ImageAnalyser();

    std::vector<Imageobject> calculateDescriptors(std::vector<Imageobject> imageVector);
    std::vector<Imageobject> analyse(std::vector<Imageobject> imageVector);
    void findPanoramas(std::vector<Imageobject> imageVector);
    bool hasIntersections(std::vector<int> v1, std::vector<int> v2);
    std::vector< std::vector<int> > removeDuplicates( std::vector< std::vector<int> > Vec);

private:
    Ptr<DescriptorMatcher> matcher;
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;
};


#endif