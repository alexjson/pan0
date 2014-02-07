#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include </home/alex/xjobb/c++/include/pan0.h>
#include </home/alex/xjobb/c++/include/dataparser.h>

double eDistance(vector<int> vec1, vector<int> vec2);

using namespace cv;
using namespace std;

int main( int argc, char **argv ) {
    Dataparser *parser = new Dataparser();
    parser->parseData("/home/alex/xjobb/images/test");

    vector<Imageobject> imageVector = parser->getImageVector();


    vector<KeyPoint> keypoints;
    Mat descriptors;

    vector< vector < KeyPoint > > keyPointVector;
    std::vector<Mat> descriptorsVector;
    SiftFeatureDetector featureDetector;
    SurfDescriptorExtractor featureExtractor;
    FlannBasedMatcher flannMatcher;

    cout << "Calculating descriptors" << endl;
    for (std::vector<Imageobject>::iterator i = imageVector.begin(); i != imageVector.end(); ++i) {
        featureDetector.detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        featureExtractor.compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
    }
    cout << "done." << endl;

    cout << "matching" << endl;
    map<pair<string, string>, vector< DMatch> > matchLookUp;

    std::vector< DMatch > matches;
    for (int i = 0; i < imageVector.size() ; ++i) {
        for (int k = i + 1; k < imageVector.size(); ++k) {
            std::vector< DMatch > good_matches;
            double max_dist = 0; double min_dist = 100;
            flannMatcher.match(imageVector[i].getDescriptors(), imageVector[k].getDescriptors(), matches);
            
            for ( int y = 0; y < imageVector[i].getDescriptors().rows; y++ ) {
                if ( matches[y].distance <= 0.08 ) {
                    good_matches.push_back( matches[y]);
                }
            }
            matchLookUp[make_pair(imageVector[i].getFileName(), imageVector[k].getFileName())] = good_matches;

        }
    }
    cout << "done." << endl;


    std::vector< DMatch > currMatch;
    pair<string, string> key;
    int TRESH = 35;
    double magDiff = 0.0;
    for (int i = 0; i < imageVector.size(); ++i) {
        for (int k = 0; k < imageVector.size(); ++k) {
            if (matchLookUp[make_pair(imageVector[i].getFileName(), imageVector[k].getFileName())].size() > TRESH ) {
                magDiff = eDistance(imageVector[i].getMag_data(), imageVector[k].getMag_data());
                if (magDiff > 25.0 && magDiff < 60.0) {
                    cout << "Bra match   " << imageVector[i].getFileName() << "   " << imageVector[k].getFileName() << endl;
                    cout << matchLookUp[make_pair(imageVector[i].getFileName(), imageVector[k].getFileName())].size() << endl;
                }
            }
        }
    }

    Mat image;
    image = imread( argv[1], 1 );

    featureDetector.detect(image, keypoints);

    if ( argc != 2 || !image.data ) {
        printf( "No image data \n" );
        return -1;
    }

    namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
    imshow( "Display Image", image );

    waitKey(0);


    return 0;
};

double eDistance(std::vector<int> vec1, std::vector<int> vec2) {
    double x1 = vec1[0];
    double x2 = vec2[0];
    double y1 = vec1[1];
    double y2 = vec2[1];
    double z1 = vec1[2];
    double z2 = vec2[2];
    double dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

    return dist;
};