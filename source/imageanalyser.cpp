#include <imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<Imageobject> *imageVector) : imageVector_(imageVector), MATCHTRESH_(40) {
    matcher = DescriptorMatcher::create("FlannBased"); // FlannBased , BruteForce
    detector = FeatureDetector::create("SIFT");
    extractor = DescriptorExtractor::create("SIFT");
};


void ImageAnalyser::extractDescriptors(int id) {

    vector<KeyPoint> keypoints;
    Mat descriptors;

    detector->detect(imageVector_->at(id).getImage(), keypoints);
    imageVector_->at(id).setKeyPoints(keypoints);
    extractor->compute(imageVector_->at(id).getImage(), keypoints, descriptors);
    imageVector_->at(id).setDescriptors(descriptors);
};

int ImageAnalyser::checkMatches(int id1, int id2) {
    std::vector< std::vector < cv::DMatch > > matches;
    if ((*imageVector_)[id1].getImage().size().width == 0) {
        (*imageVector_)[id1].loadImage();
        extractDescriptors(id1);
    }
    if ((*imageVector_)[id2].getImage().size().width == 0) {
        (*imageVector_)[id2].loadImage();
        extractDescriptors(id2);
    }

    if ((*imageVector_)[id1].getDescriptors().size().height != 0 &&
            (*imageVector_)[id2].getDescriptors().size().height != 0) {
        matcher->knnMatch((*imageVector_)[id1].getDescriptors(),
                          (*imageVector_)[id2].getDescriptors(), matches, 2); // Find two nearest matches
        vector<cv::DMatch> good_matches;
        for (int i = 0; i < matches.size(); ++i) {
            const float ratio = 0.7; // As in Lowe's SIFT paper; can be tuned

            if (matches[i][0].distance < ratio * matches[i][1].distance) {
                good_matches.push_back(matches[i][0]);
            }
        }
        return good_matches.size();
    }

    return 0;

};

void ImageAnalyser::analyse() {
    int numberOfMatches = MATCHTRESH_;
    int currentNum = -1;
    G_ = new Graph(imageVector_->size());

    progress_display show_progress( imageVector_->size() );
    imageVector_->at(0).setMatched(true);

    for (int id1 = 0; id1 < imageVector_->size(); ++id1) {
        numberOfMatches = MATCHTRESH_;
        std::vector<int> matchIDvec;
        //TODO OpenMP pragma here
        for (int id2 = 0; id2 < imageVector_->size(); ++id2) {
            if (id1 == id2) {
                continue;
            } else if (checkEdge(id1, id2)) {
                continue;
            } else {
                if (checkTrigger(id1, id2, imageVector_) &&
                        checkTimeDiff(id1, id2, imageVector_) &&
                        checkTiltDiff(id1, id2, imageVector_) &&
                        checkMagDiffMax(id1, id2, imageVector_) ) {
                    currentNum = checkMatches(id1, id2);
                    if (currentNum > numberOfMatches) {
                        matchIDvec.push_back(id2);
                    }
                }
            }
        }
        for (int idx = 0; idx < matchIDvec.size(); ++idx) {
            boost::add_edge(id1, matchIDvec.at(idx), *G_);
        }
        ++show_progress;
    }

    // double wall1 = get_wall_time();
    // double cpu1  = get_cpu_time();


    printGraph("before", true);
    filterPanoramas();
};

bool ImageAnalyser::checkEdge(int id1, int id2) {
    std::pair < MyEdge, bool > p = boost::edge( id1, id2, *G_);
    if (p.second == true)  return true;
    else return false;
};

//Just for debugging
void ImageAnalyser::printGraph(string fileName, bool names) {
    if (names) {
        std::vector<std::string> names;
        for (int i = 0 ; i < imageVector_->size() ; i++) {
            names.push_back(imageVector_->at(i).getFileName());
        }
        std::ofstream dmp;
        dmp.open(fileName + ".dot");
        boost::write_graphviz(dmp, (*G_), boost::make_label_writer(&names[0]));
    } else {
        std::ofstream dmp;
        dmp.open(fileName + ".dot");
        boost::write_graphviz(dmp, (*G_));
    }
}

//Method for removing none contributing images
void ImageAnalyser::filterPanoramas() {
    std::vector<int>::iterator it;
    std::vector<int> component(num_vertices(*G_));
    int num = boost::connected_components(*G_, &component[0]);

    for (int idx = 0; idx < num; ++idx) {
        int numberOfElements = std::count(component.begin(), component.end(), idx);

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