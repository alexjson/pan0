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
    int MATCHTRESH = 25;
    int numberOfMatches = MATCHTRESH;
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
        numberOfMatches = MATCHTRESH;
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
                    numberOfMatches2 = numberOfMatches;
                    numberOfMatches = good_matches.size();

                    SecondBestMatches = BestMatches;
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

        // if (imageVector[id1].getFileName() == "123.jpg" && imageVector[secondMatch].getFileName() == "122.jpg") {
        //     Mat outImg;
        //     cout << "Mathces size " << SecondBestMatches.size() << "     " << BestMatches.size() << endl;
        //     drawMatches(imageVector[secondMatch].getImage(), imageVector[secondMatch].getKeypoints(),
        //                 imageVector[id1].getImage(), imageVector[id1].getKeypoints(), SecondBestMatches, outImg);

        //     imshow("Matches?", outImg);
        //     waitKey(0);

        // }

        if (numberOfMatches2 > MATCHTRESH) {
            cout << "[" << numberOfMatches << "] " << imageVector[firstMatch].getFileName() << "<====[" << imageVector[id1].getFileName() << "]====>" << imageVector[secondMatch].getFileName() << "[" << numberOfMatches2 << "] " << endl; /* code */
        } else {
            cout << "[" << numberOfMatches << "] " << imageVector[firstMatch].getFileName() << "<====[" << imageVector[id1].getFileName() << "]" << endl;
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

    //Ta intersect först, om intersect ta unionen av båda och spara resultaten i en resultat vector med ID över bilder
    // Hur lösa om det är flera panorama?
    std::vector<int> tmpVec;
    std::vector<int> ID;
    std::vector<int>::iterator it;

    for (int current = 0; current < imageVector.size(); ++current) {
        tmpVec.push_back(imageVector[current].getFirstMatch());
        tmpVec.push_back(imageVector[current].getSecondMatch());

        std::sort (ID, ID.size());
        std::sort (tmpVec, tmpVec.size());

        it = std::set_intersection (ID, ID.size(), tmpVec, tmpVec.size(), v.begin());

        tmpVec.resize(it - tmpVec.begin());

        if (tmpVec.size() > 0) {
            for (int idx = 0; idx < tmpVec.size(); ++idx) {
                ID.push_back(tmpVec[i]);
            }
        }

        std::cout << "The intersection has " << (tmpVec.size()) << " elements:\n";
        for (it = v.begin(); it != v.end(); ++it)
            std::cout << ' ' << *it;
        std::cout << '\n';


    }

}
