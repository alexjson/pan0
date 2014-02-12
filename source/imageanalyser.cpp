#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<ImageObject> imageVector) {

    matcher = DescriptorMatcher::create("BruteForce");
    detector = FeatureDetector::create("SIFT");
    extractor = DescriptorExtractor::create("SIFT");

    cout << "Calculating descriptors" << endl;
    for (std::vector<Imageobject>::iterator i = imageVector.begin(); i != imageVector.end(); ++i) {
        detector->detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        extractor->compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
    }
    cout << "done." << endl;


};

