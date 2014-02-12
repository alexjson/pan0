#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser() {

    matcher = DescriptorMatcher::create("FlannBased");
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


    Mat descriptors_1 = imageVector[1].getDescriptors();
    Mat descriptors_2 = imageVector[6].getDescriptors();
    Mat img_1 = imageVector[1].getImage();
    Mat img_2 = imageVector[6].getImage();
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    keypoints_1 = imageVector[1].getKeypoints();
    keypoints_2 = imageVector[6].getKeypoints();

    std::vector< DMatch > matches;
    matcher->match( descriptors_1, descriptors_2, matches );

    double max_dist = 0; double min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints
    for ( int i = 0; i < descriptors_1.rows; i++ ) {
        double dist = matches[i].distance;
        if ( dist < min_dist ) min_dist = dist;
        if ( dist > max_dist ) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    std::vector< DMatch > good_matches;

    for ( int i = 0; i < descriptors_1.rows; i++ ) {
        if ( matches[i].distance <= max(2 * min_dist, 0.02) ) {
            good_matches.push_back( matches[i]);
        }
    }

    //-- Draw only "good" matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Show detected matches
    imshow( "Good Matches", img_matches );


    waitKey(0);

    // std::vector< vector< DMatch > > matches;
    // for (int idx = 0; idx < imageVector.size(); ++idx) {
    //     for (int idy = idx + 1; idy < imageVector.size(); ++idy) {
    //         std::vector< DMatch > matches;

    //         // matcher->radiusMatch(imageVector[idx].getDescriptors(), imageVector[idy].getDescriptors(), matches, 1000);
    //         matcher->match(imageVector[idx].getDescriptors(), imageVector[idy].getDescriptors(), matches);
    //         // float test = matches[0][1].distance;
    //         // cout << test << endl;
    //         cout << matches.size() << endl;
    //     }
    // }



}

