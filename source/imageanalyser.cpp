#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser() {

    matcher = DescriptorMatcher::create("FlannBased"); // FlannBased
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

std::vector<Imageobject> ImageAnalyser::analyse(std::vector<Imageobject> imageVector) {
    int numberOfMatches = 0;
    int numberOfMatches2 = -1;
    int firstMatch = -1;
    int secondMatch = -1;
    // vector<DMatch> matches;
    std::vector< std::vector < cv::DMatch > > matches;

    vector<DMatch> BestMatches;
    vector<DMatch> SecondBestMatches;
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
        numberOfMatches2 = -1;

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
                //Spara 2 bilder som good_matches för att kunna pussla ihop panorama senare
                // firstMatch secondMatch.
                if (good_matches.size() > numberOfMatches) {
                    numberOfMatches = good_matches.size();
                    BestMatches = good_matches;
                    secondMatch = firstMatch;
                    firstMatch = id2;
                }
                //Clear out vectors for next iteration
                good_matches.clear();
                matches.clear();

            }
            for (int i = 0; i < BestMatches.size(); ++i) {
                firstImage.push_back( imageVector[id1].getKeypoints()[BestMatches[i].queryIdx].pt);
                secondImage.push_back( imageVector[firstMatch].getKeypoints()[BestMatches[i].trainIdx].pt);
            }
        } //END INNER LOOP

        Mat H;
        std::vector<uchar> match_mask;
        //Find homography with RANSAC
        H = findHomography( firstImage, secondImage, match_mask, RANSAC );

        // imageVector[id1].setMatchID(firstMatch);
        imageVector[id1].setFirstMatch(firstMatch);
        imageVector[id1].setSecondMatch(secondMatch);

        //Clear up vectors for next iteration
        firstImage.clear();
        secondImage.clear();

        if (secondMatch > 0) {
            cout << imageVector[firstMatch].getFileName() << "<====[" << imageVector[id1].getFileName() << "]====>" << imageVector[secondMatch].getFileName() << endl; /* code */
        } else {
        	cout << imageVector[firstMatch].getFileName() << "<====[" << imageVector[id1].getFileName() <<"]"<< endl; 
        }



        // -----------------------------------------------------------------------------------------------------------------
        // ***********************************Verify image matches using probabilistic model?**********************************
        // -----------------------------------------------------------------------------------------------------------------

        // int numberOfInliers = countNonZero(Mat(match_mask));
        // double test = (5.9 + 0.22 * (double)numberOfMatches);

        // if ((double)numberOfInliers > (5.9 + 0.22 * (double)numberOfMatches)) {
        // cout << "True match" << imageVector[id1].getFileName() << "        " << imageVector[firstMatch].getFileName() << endl;
        // }else{
        //  cout << "False match" << imageVector[id1].getFileName() << "        " << imageVector[firstMatch].getFileName() << endl;
        // }
    } //END BIG LOOP

    return imageVector;

};
void ImageAnalyser::findPanoramas(std::vector<Imageobject> imageVector) {

    std::vector<int> panoramaList;

    bool done = false;
    int id = 0;
    int id2 = 0;
    panoramaList.push_back(id);

    while (!done) {
        id2 = imageVector[id].getFirstMatch();

        if (std::find(panoramaList.begin(), panoramaList.end(), id2) != panoramaList.end()) {
            for (int i = 0; i < panoramaList.size(); ++i) {
                cout << "inne" << endl;
                cout << i << endl;
                if (std::find(panoramaList.begin(), panoramaList.end(), i) != panoramaList.end()) {
                    done = true;
                } else {
                    cout << "inne2" << endl;
                    id = i;
                }
            }
        } else {
            panoramaList.push_back(id2);
            id = id2;
        }
    }

    for (int x = 0; x < panoramaList.size(); ++x) {
        cout << imageVector[panoramaList[x]].getFileName() << endl;
    }


    // for (int id = 0; id < imageVector.size(); ++id) {
    //     int id2 = imageVector[id].getFirstMatch();
    //     cout << imageVector[id].getFileName() << " ===> " << imageVector[id2].getFileName() << endl;
    // }
}
