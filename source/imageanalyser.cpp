#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser() {

    matcher = DescriptorMatcher::create("BruteForce");
    detector = FeatureDetector::create("SIFT");
    extractor = DescriptorExtractor::create("SIFT");


};

std::vector<Imageobject> ImageAnalyser::calculateDescriptors(std::vector<Imageobject> imageVector) {

    vector<KeyPoint> keypoints;
    Mat descriptors;

    cout << "Calculating descriptors" << endl;
    for (std::vector<Imageobject>::iterator i = imageVector.begin(); i != imageVector.end(); ++i) {
        detector->detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        extractor->compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
    }
    cout << "done." << endl;

    return imageVector;
};

void ImageAnalyser::analyse(std::vector<Imageobject> imageVector) {
    int numberOfMatches = 0;
    int matchID = -1;
    vector<DMatch> matches;
    std::vector<int> matchIDvec;
    for (int id1 = 0; id1 < imageVector.size(); ++id1) {
        // Train the matcher with the query descriptors
        matcher->add( imageVector[id1].getDescriptors() );
        matcher->train();
        numberOfMatches = 0;
        for (int id2 = 0; id2 < imageVector.size(); ++id2) {
            if (imageVector[id1].getFileName() == imageVector[id2].getFileName()) {
                continue;
            } else {
                matcher->match( imageVector[id2].getDescriptors(), matches );
                if (matches.size() > numberOfMatches) {
                    numberOfMatches = matches.size();
                    matchID = id2;
                }
                cout << "Number of matches 	" << imageVector[id1].getFileName() << "		" << imageVector[id2].getFileName() << "		" << matches.size() << endl;
                cout << "----------------------------------------------------" << endl;
            }
            matchIDvec.push_back(matchID);
        }

    }

}

