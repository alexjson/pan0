#ifndef IMAGEANALYSER_H
#define IMAGEANALYSER_H

#include <opencv2/opencv.hpp>
#include </home/alex/xjobb/c++/include/imageobject.h>

using namespace std;
using namespace cv;
class ImageObject;

class ImageAnalyser
{
public:
	ImageAnalyser(std::vector<ImageObject> imageVector);
	ImageAnalyser();

	~ImageAnalyser();

	void analyse();

private:
	Ptr<DescriptorMatcher> matcher;
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
};


#endif