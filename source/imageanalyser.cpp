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

    progress_display show_progress( imageVector_->size() );


    for (std::vector<Imageobject>::iterator i = imageVector_->begin(); i != imageVector_->end(); ++i) {
        detector->detect(i->getImage(), keypoints);
        i->setKeyPoints(keypoints);
        extractor->compute(i->getImage(), keypoints, descriptors);
        i->setDescriptors(descriptors);
        keypoints.clear();
        ++show_progress;
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
    const int MATCHTRESH = 10;
    int numberOfMatches = MATCHTRESH;
    // int firstMatch = -1;
    // int secondMatch = -1;
    int currentNum = -1;
    // vector<DMatch> BestMatches;
    // vector<DMatch> SecondBestMatches;
    // std::vector<int> matchIDvec;
    G_ = new Graph();
    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();

    progress_display show_progress( imageVector_->size() );
    imageVector_->at(0).setMatched(true);
    for (int id1 = 0; id1 < imageVector_->size(); ++id1) {
        numberOfMatches = MATCHTRESH;
        int firstMatch = -1;
        for (int id2 = 0; id2 < imageVector_->size(); ++id2) {
            if (id1 == id2) {
                continue;
            } else {
                // firstMatch secondMatch.
                if (imageVector_->at(id2).getFirstMatchID() != id1 && !imageVector_->at(id2).getMatched() ) {
                    if (checkMagDiffMax(id1, id2, imageVector_) && checkTimeDiff(id1, id2, imageVector_)) {
                        currentNum = checkMatches(id1, id2);
                        if (currentNum > numberOfMatches) {
                            numberOfMatches = currentNum;
                            firstMatch = id2;
                        }
                    }
                }

            }
        }
        if (firstMatch != -1) {
            boost::add_edge(id1, firstMatch, *G_);

            (*imageVector_)[id1].setFirstMatchID(firstMatch);
            (*imageVector_)[firstMatch].setMatched(true);

        }

        ++show_progress;
    }

    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();

    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "CPU Time  = " << cpu1  - cpu0  << endl;
    printf("Done.\n");

    printGraph("before");
    filterPanoramas();
};


void ImageAnalyser::printGraph(string fileName) {
    // std::vector<std::string> names;
    // for (int i = 0 ; i < imageVector_->size() ; i++) {
    //     names.push_back(imageVector_->at(i).getFileName());
    // }
    std::ofstream dmp;
    dmp.open(fileName + ".dot");
    // boost::write_graphviz(dmp, (*G_), boost::make_label_writer(&names[0]));
    boost::write_graphviz(dmp, (*G_));

}

//Method for removing none contributing images
void ImageAnalyser::filterPanoramas() {
    std::vector<int>::iterator it;
    std::vector<int> component(num_vertices(*G_));
    int num = boost::connected_components(*G_, &component[0]);


    cout << "components in graph  " << num << endl;


    for (int idx = 0; idx < num; ++idx) {

        int numberOfElements = std::count(component.begin(), component.end(), idx);
        cout << "numberOfElements :" << idx << "  " << numberOfElements << endl;

        std::vector<int> idVec;
        int id1 = 0;
        for (int a = 0; a < numberOfElements; ++a) {
            it = find(component.begin() + id1, component.end(), idx);
            id1 = std::distance( component.begin(), it);
            idVec.push_back(id1);
            ++id1;
        }

        analyseComponent(idVec);
    }

    refineGraph();
    printGraph("after");

};

bool ImageAnalyser::analyseComponent(std::vector<int> idVec) {


    for (int idx = 0; idx <  idVec.size(); ++idx) {
        int id1 = idVec.at(idx);
        if (imageVector_->at(id1).getStatus() != NONE )
            continue;

        for (int idy = 0; idy <  idVec.size(); ++idy) {

            int id2 = idVec.at(idy);
            //If it is the last picture in the sequence
            if (idy + 1 ==  idVec.size()) {
                imageVector_->at(id1).setStatus(INCLUDED);
                imageVector_->at(id1).setFirstMatchID(id2);
            } else if (id1 == id2 || imageVector_->at(id2).getStatus() != NONE) {
                continue;
            } else if (checkMagDiffMin(id1, id2, imageVector_) && checkTimeDiff(id1, id2, imageVector_)) {
                imageVector_->at(id1).setStatus(INCLUDED);
                imageVector_->at(id1).setFirstMatchID(id2);
                break;
            } else {
                imageVector_->at(id2).setStatus(REJECTED);
                remove_vertex(id2, (*G_));
            }
        }
    }
};


void ImageAnalyser::refineGraph() {
    G_ = new Graph();
    for (std::vector<Imageobject>::iterator it = imageVector_->begin(); it != imageVector_->end(); ++it) {
        Imageobject current = *it;
        if (current.getStatus() == INCLUDED) {
            if (lookUpMap_.find(current.getID()) != lookUpMap_.end() &&
                    lookUpMap_.find(current.getFirstMatchID()) != lookUpMap_.end()) { //if both exist in map
                boost::add_edge(lookUpMap_.find(current.getID())->second,
                                lookUpMap_.find(current.getFirstMatchID())->second, *G_);
            } else if (lookUpMap_.find(current.getID()) != lookUpMap_.end()) { // if only currentID exist in map
                lookUpMap_.emplace(current.getFirstMatchID(), num_vertices(*G_));
                boost::add_edge(num_vertices(*G_),
                                lookUpMap_.find(current.getID())->second, *G_);

            } else if (lookUpMap_.find(current.getFirstMatchID()) != lookUpMap_.end()) { //If onlyfirstMatch exist in map
                lookUpMap_.emplace(current.getID(), num_vertices(*G_));
                boost::add_edge(lookUpMap_.find(current.getFirstMatchID())->second,
                                num_vertices(*G_), *G_);
            } else { // if none exist in map
                lookUpMap_.emplace(current.getID(), num_vertices(*G_));
                lookUpMap_.emplace(current.getFirstMatchID(), num_vertices(*G_) + 1);
                boost::add_edge(num_vertices(*G_), num_vertices(*G_) + 1, *G_);
            }

        }
    }

};


std::map<int, int>::iterator ImageAnalyser::findSecond(int id) {
    for (std::map<int, int>::iterator it = lookUpMap_.begin(); it != lookUpMap_.end(); ++it ) {
        if (it->second == id)
            return it;
    }
    return lookUpMap_.end();
};