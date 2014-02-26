#include </home/alex/xjobb/c++/include/imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<Imageobject> *imageVector) : imageVector_(imageVector) {

    matcher = DescriptorMatcher::create("FlannBased"); // FlannBased
    detector = FeatureDetector::create("SIFT");
    extractor = DescriptorExtractor::create("SIFT");
};

void ImageAnalyser::calculateDescriptors() {

    vector<KeyPoint> keypoints;
    Mat descriptors;

    cout << "Calculating descriptors" << endl;
    for (std::vector<Imageobject>::iterator i = imageVector_->begin(); i != imageVector_->end(); ++i) {
        detector->detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        extractor->compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
        keypoints.clear();
    }
    cout << "done." << endl;
};

void ImageAnalyser::analyse() {
    printf("Analysing\n");
    int MATCHTRESH = 50;
    int numberOfMatches = MATCHTRESH;
    int numberOfMatches2 = -1;
    int firstMatch = -1;
    int secondMatch = -1;
    std::vector< std::vector < cv::DMatch > > matches;

    vector<DMatch> BestMatches;
    vector<DMatch> SecondBestMatches;
    std::vector<int> matchIDvec;

    for (int id1 = 0; id1 < imageVector_->size(); ++id1) {

        numberOfMatches = MATCHTRESH;
        numberOfMatches2 = -1;

        for (int id2 = 0; id2 < imageVector_->size(); ++id2) {
            if (id1 == id2) {
                continue;
            } else {
                matcher->knnMatch((*imageVector_)[id1].getDescriptors(),(*imageVector_)[id2].getDescriptors(), matches, 2);  // Find two nearest matches
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
        } //END INNER LOOP

        (*imageVector_)[id1].setFirstMatchID(firstMatch);
        (*imageVector_)[id1].setFirstMatches(BestMatches);

        if (numberOfMatches2 > MATCHTRESH) {
            (*imageVector_)[id1].setSecondMatchID(secondMatch);
            (*imageVector_)[id1].setSecondMatches(SecondBestMatches);
        }


        // if (numberOfMatches2 > MATCHTRESH) {
        //     cout << " [" << numberOfMatches << "] " << (*imageVector_)[firstMatch].getFileName() <<
        //          "<====[" << (*imageVector_)[id1].getFileName() << "]====>" << (*imageVector_)[secondMatch].getFileName()
        //          << "[" << numberOfMatches2 << "] " <<
        //          "      " << firstMatch << "    " << id1 << "     " <<  secondMatch << endl;
        // } else {
        //     cout << " [" << numberOfMatches << "] " << (*imageVector_)[firstMatch].getFileName() << "<====[" <<
        //          (*imageVector_)[id1].getFileName() << "]" <<
        //          "      " << firstMatch << "    " << id1 << "     " << endl;
        // }



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
    printf("Done.\n");

};
std::vector< std::vector<int> > *ImageAnalyser::findPanoramas() {

    printf("Finding panoramas\n");

    std::vector<int> tmpVec;
    std::vector<int> ID;
    std::vector< std::vector<int> > *panoramas = new std::vector<std::vector<int> >();
    std::vector<int> currentID;
    std::vector<int>::iterator it;
    bool intersect;


    for (int current = 0; current < imageVector_->size(); ++current) {
        if (current == 0) {
            ID.push_back((*imageVector_)[current].getFirstMatchID());
            ID.push_back(current);
            panoramas->push_back(ID);
            continue;
        }

        currentID.push_back(current);
        currentID.push_back((*imageVector_)[current].getFirstMatchID());
        tmpVec.push_back(current);
        tmpVec.push_back((*imageVector_)[current].getFirstMatchID());
        if ((*imageVector_)[current].getSecondMatchID() != -1) {
            currentID.push_back((*imageVector_)[current].getSecondMatchID());
            tmpVec.push_back((*imageVector_)[current].getSecondMatchID());
        }

        for (int panoID = 0; panoID < panoramas->size(); ++panoID) {
            intersect = hasIntersections((*panoramas)[panoID], tmpVec);

            if (intersect) {
                for (int idx = 0; idx < currentID.size(); ++idx) {
                    (*panoramas)[panoID].push_back(currentID[idx]);
                    break;
                }
            }
        }
        if (!intersect)
            panoramas->push_back(tmpVec);

        currentID.clear();
        tmpVec.clear();
    }

    removeDuplicates(panoramas);

    cout << "panoSize   " << panoramas->size() << endl;
    for (int y = 0; y < panoramas->size(); ++y) {
        printf("Pano contains: ");
        for (int idx = 0; idx < (*panoramas)[y].size(); ++idx) {
            cout << (*panoramas)[y][idx] << " ";
        }
        printf("\n");
    }

    printf("Done.\n");
    return panoramas;
}

bool ImageAnalyser::hasIntersections(std::vector<int> v1, std::vector<int> v2) {

    std::vector<int>::iterator it;

    std::sort (v1.begin(), v1.begin() + v1.size());
    std::sort (v2.begin(), v2.begin() + v2.size());

    //Get the intersection
    it = std::set_intersection (v1.begin(), v1.begin() + v1.size(), v2.begin(), v2.begin() + v2.size(), v2.begin());
    v2.resize(it - v2.begin());

    return v2.size() > 0;

}
void ImageAnalyser::removeDuplicates( std::vector< std::vector<int> > *vec) {
    for (int idx = 0; idx < vec->size(); ++idx) {
        sort( (*vec)[idx].begin(), (*vec)[idx].end() );
        (*vec)[idx].erase( unique( (*vec)[idx].begin(), (*vec)[idx].end() ), (*vec)[idx].end() );
    }
}
