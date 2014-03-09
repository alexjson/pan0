#include <imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<Imageobject> *imageVector) : imageVector_(imageVector) {
    matcher = DescriptorMatcher::create("FlannBased"); // FlannBased , BruteForce
    detector = FeatureDetector::create("SIFT");
    extractor = DescriptorExtractor::create("SIFT");
};

void ImageAnalyser::calculateDescriptors() {
    vector<KeyPoint> keypoints;
    Mat descriptors;
    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();

    for (std::vector<Imageobject>::iterator i = imageVector_->begin(); i != imageVector_->end(); ++i) {
        detector->detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        extractor->compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
        keypoints.clear();
    }
    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();
    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "CPU Time  = " << cpu1  - cpu0  << endl;
};

int ImageAnalyser::checkMatches(int id1, int id2) {
    std::vector< std::vector < cv::DMatch > > matches;
    const float ratio = 0.7; // As in Lowe's SIFT paper; can be tuned
    matcher->knnMatch((*imageVector_)[id1].getDescriptors(), (*imageVector_)[id2].getDescriptors(), matches, 2); // Find two nearest matches
    vector<cv::DMatch> good_matches;
    for (int i = 0; i < matches.size(); ++i) {
        if (matches[i][0].distance < ratio * matches[i][1].distance) {
            good_matches.push_back(matches[i][0]);
        }
    }

    return good_matches.size();
};

void ImageAnalyser::analyse() {
    printf("Analysing\n");
    const int MATCHTRESH = 50;
    int numberOfMatches = MATCHTRESH;
    int numberOfMatches2 = -1;
    int firstMatch = -1;
    int secondMatch = -1;
    int currentNum = -1;
    // vector<DMatch> BestMatches;
    // vector<DMatch> SecondBestMatches;
    // std::vector<int> matchIDvec;
    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();

    for (int id1 = 0; id1 < imageVector_->size(); ++id1) {
        numberOfMatches = MATCHTRESH;
        numberOfMatches2 = -1;

        for (int id2 = 0; id2 < imageVector_->size(); ++id2) {
            if (id1 == id2) {
                continue;
            } else {
                //Spara 2 bilder som good_matches för att kunna pussla ihop panorama senare
                // firstMatch secondMatch.

                currentNum = checkMatches(id1, id2);

                if (currentNum > numberOfMatches) {

                    numberOfMatches2 = numberOfMatches;
                    numberOfMatches = currentNum;

                    secondMatch = firstMatch;
                    firstMatch = id2;
                }

            }

        } //END INNER LOOP
        //Lägg till i GRAPH här istället för firstMatchID och secondMatchID, ta bort GRAPH init
        (*imageVector_)[id1].setFirstMatchID(firstMatch);
        // (*imageVector_)[id1].setFirstMatches(BestMatches);

        if (numberOfMatches2 > MATCHTRESH) {
            (*imageVector_)[id1].setSecondMatchID(secondMatch);
            // (*imageVector_)[id1].setSecondMatches(SecondBestMatches);
        }
    } //END BIG LOOP

    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();

    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "CPU Time  = " << cpu1  - cpu0  << endl;
    printf("Done.\n");
    initGraph();
    filterPanoramas();
};

void ImageAnalyser::initGraph() {
    G_ = new Graph();
    //Use boost with graphs
    for (std::vector<Imageobject>::iterator it = imageVector_->begin(); it != imageVector_->end(); ++it) {
        Imageobject current = *it;
        int first = current.getFirstMatchID();
        int currentID = current.getID();
        boost::add_edge(currentID, first, *G_);
        if (current.getSecondMatchID() != -1) {
            boost::add_edge(currentID, current.getSecondMatchID(), *G_);
        }
    }
};

//Method for removing none contributing images
void ImageAnalyser::filterPanoramas() {
    std::vector<int>::iterator it;
    std::vector<int> component(num_vertices(*G_));
    int num = boost::connected_components(*G_, &component[0]);

    for (int idx = 0; idx < num; ++idx) {
        it = find(component.begin(), component.end(), idx);
        int beg = std::distance( component.begin(), it);
        int end = std::count(component.begin(), component.end(), idx) + beg - 1;

        for (int id1 = beg; id1 <  end; ++id1) {
            if (imageVector_->at(id1).getStatus() != NONE )
                continue;

            for (int id2 = beg; id2 <  end; ++id2) {
                //If it is the last picture in the sequence
                if (id2 + 1 ==  end) {
                    imageVector_->at(id1).setStatus(INCLUDED);
                    imageVector_->at(id1).setFirstMatchID(id2);
                }
                if (id1 == id2 || imageVector_->at(id2).getStatus() != NONE)
                    continue;

                if ( !checkMagDiff(id1, id2) )
                    imageVector_->at(id2).setStatus(REJECTED);
                else {
                    imageVector_->at(id1).setStatus(INCLUDED);
                    imageVector_->at(id1).setFirstMatchID(id2);
                }

            }
        }
    }
    //Recreate graph to prevent missing connections after deletions.
    G_ = new Graph();

    for (std::vector<Imageobject>::iterator it = imageVector_->begin(); it != imageVector_->end(); ++it) {
        Imageobject current = *it;
        if (current.getStatus() == INCLUDED) {
            boost::add_edge(current.getID(), current.getFirstMatchID(), *G_);
        }
    }

};

bool ImageAnalyser::checkMagDiff(int id1, int id2) {
    double magDiff = eDistance((*imageVector_)[id1].getMag_data(), (*imageVector_)[id2].getMag_data());
    return magDiff > 21.0;
};