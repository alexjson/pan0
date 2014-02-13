#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser() {

    matcher = DescriptorMatcher::create("BruteForce"); // FlannBased
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
        keypoints.clear();
    }
    cout << "done." << endl;

    return imageVector;
};

void ImageAnalyser::analyse(std::vector<Imageobject> imageVector) {
    int numberOfMatches = 0;
    int matchID = -1;
    // vector<DMatch> matches;
    std::vector< std::vector < cv::DMatch > > matches;

    vector<DMatch> BestMatches;
    std::vector<int> matchIDvec;
    std::vector<Point2f> firstImage;
    std::vector<Point2f> secondImage;
    int id1 = 0;
    // int id2 = 27;

    for (int id1 = 0; id1 < imageVector.size(); ++id1) {

        // Train the matcher with the query descriptors
        matcher->clear(); //Remove previous descriptors
        matcher->add( imageVector[id1].getDescriptors() );
        matcher->train();
        numberOfMatches = 0;

        for (int id2 = 0; id2 < imageVector.size(); ++id2) {
            if (id1 == id2) {
                continue;
            } else {
                matcher->knnMatch(imageVector[id2].getDescriptors(), matches, 2);  // Find two nearest matches
                vector<cv::DMatch> good_matches;
                for (int i = 0; i < matches.size(); ++i) {
                    const float ratio = 0.7; // As in Lowe's SIFT paper; can be tuned
                    if (matches[i][0].distance < ratio * matches[i][1].distance) {
                        good_matches.push_back(matches[i][0]);
                    }
                }

                if (good_matches.size() > numberOfMatches) {
                    numberOfMatches = good_matches.size();
                    BestMatches = good_matches;
                    matchID = id2;
                }
                //Clear out vectors for next iteration
                good_matches.clear();
                matches.clear();

            }
            for (int i = 0; i < BestMatches.size(); ++i) {
                firstImage.push_back( imageVector[id1].getKeypoints()[BestMatches[i].queryIdx].pt);
                secondImage.push_back( imageVector[matchID].getKeypoints()[BestMatches[i].trainIdx].pt);
            }
        } //END INNER LOOP

        Mat H;
        std::vector<uchar> match_mask;
        //Find homography with RANSAC
        H = findHomography( firstImage, secondImage, match_mask, RANSAC );

        matchIDvec.push_back(matchID);

        //Clear up vectors for next iteration
        firstImage.clear();
        secondImage.clear();


        //Verify image matches using probabilistic model FIXA FEL ?
        int numberOfInliers = countNonZero(Mat(match_mask));
        double test = (5.9 + 0.22 * (double)numberOfMatches);

        if ((double)numberOfInliers > (5.9 + 0.22 * (double)numberOfMatches)) {
        cout << "True match" << imageVector[id1].getFileName() << "        " << imageVector[matchID].getFileName() << endl;
        }else{
         cout << "False match" << imageVector[id1].getFileName() << "        " << imageVector[matchID].getFileName() << endl;
        }
    } //END BIG LOOP

}
