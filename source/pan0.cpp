#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include </usr/local/include/opencv2/stitching/stitcher.hpp>
#include </home/alex/xjobb/c++/include/pan0.h>
#include </home/alex/xjobb/c++/include/dataparser.h>

double eDistance(vector<int> vec1, vector<int> vec2);

map<pair<string, string>, vector< DMatch> > calculateMatches(vector<Imageobject> imageVector);
std::vector<string> filterImages( vector<Imageobject> imageVector, map<pair<string, string>, vector< DMatch> > matchLookUp);
void stitchTest(vector<string> filesToStitch);

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
    SiftDescriptorExtractor featureExtractor;


    cout << "Calculating descriptors" << endl;
    for (std::vector<Imageobject>::iterator i = imageVector.begin(); i != imageVector.end(); ++i) {
        featureDetector.detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        featureExtractor.compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
    }
    cout << "done." << endl;

    map<pair<string, string>, vector< DMatch> >  matchLookUp = calculateMatches(imageVector);
    std::vector<string> toStitch =  filterImages(imageVector, matchLookUp);

    cout << "Images to stitch" << endl;
    for (int i = 0; i < toStitch.size(); ++i) {
        cout << toStitch[i] << endl;
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


map<pair<string, string>, vector< DMatch> > calculateMatches(vector<Imageobject> imageVector) {
    cout << "matching" << endl;
    FlannBasedMatcher flannMatcher;
    map<pair<string, string>, vector< DMatch> > matchLookUp;

    std::vector< DMatch > matches;
    for (int i = 0; i < imageVector.size() ; ++i) {
        for (int k = i + 1; k < imageVector.size(); ++k) {
            std::vector< DMatch > good_matches;
            double max_dist = 0; double min_dist = 100;
            flannMatcher.match(imageVector[i].getDescriptors(), imageVector[k].getDescriptors(), matches);

            for ( int y = 0; y < imageVector[i].getDescriptors().rows; y++ ) {
                if ( matches[y].distance <= 50.0 ) {
                    good_matches.push_back( matches[y]);
                }
            }
            matchLookUp[make_pair(imageVector[i].getFileName(), imageVector[k].getFileName())] = good_matches;
        }
    }
    cout << "done." << endl;
    return matchLookUp;
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

std::vector<string> filterImages( vector<Imageobject> imageVector , map<pair<string, string>, vector< DMatch> > matchLookUp) {

    pair<string, string> key1;
    pair<string, string> key2;
    std::vector<string> toStitch;
    std::vector<pair <string, std::vector<string> > > lookUp;
    int TRESH = 50;
    double magDiff = 0.0;


    //FIXME: Grupepra bilderna i "panorama" grupper om gruppen är mindre än 4 strunta i den

    cout << "Filtering results" << endl;
    for (int i = 0; i < imageVector.size(); ++i) {
        std::vector<string> tmp;
        for (int k = 0; k < imageVector.size(); ++k) {

            key1 = make_pair(imageVector[i].getFileName(), imageVector[k].getFileName());
            key2 = make_pair(imageVector[k].getFileName(), imageVector[i].getFileName());

            if (matchLookUp[key1].size() > TRESH  || matchLookUp[key2].size() > TRESH ) {
                magDiff = eDistance(imageVector[i].getMag_data(), imageVector[k].getMag_data());

                if (magDiff > 25.0 && magDiff < 60.0) {
                    toStitch.push_back(imageVector[i].getFileName());
                    toStitch.push_back(imageVector[k].getFileName());
                }
            }
        }
        lookUp.push_back(imageVector[i].getFileName(),tmp);
    }
    cout << "done." << endl;


    //Remove duplicates from the vector
    cout << "Removing duplicates" << endl;
    sort( toStitch.begin(), toStitch.end() );
    toStitch.erase( unique( toStitch.begin(), toStitch.end() ), toStitch.end() );
    cout << "done." << endl;

    return toStitch;

};


void stitchTest(vector<string> filesToStitch) {

    cout << "stitching!" << endl;
    string tmpPath = "/home/alex/xjobb/images2/test/";
    vector< Mat > vImg;
    Mat rImg;

    for (int i = 0; i < filesToStitch.size(); ++i) {
        vImg.push_back( imread(tmpPath + filesToStitch[i]));
    }


    Stitcher stitcher = Stitcher::createDefault();


    unsigned long AAtime = 0, BBtime = 0; //check processing time
    AAtime = getTickCount(); //check processing time

    stitcher.stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime) / getTickFrequency() ); //check processing time

    imshow("Stitching Result", rImg);

    waitKey(0);

};