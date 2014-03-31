#include <imageanalyser.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<Imageobject> *imageVector) : imageVector_(imageVector), MATCHTRESH_(40) {
    matcher = DescriptorMatcher::create("FlannBased"); // FlannBased , BruteForce
    detector = FeatureDetector::create("SIFT");
    detector->set("nFeatures", 500);
    extractor = DescriptorExtractor::create("SIFT");

    // printAlgorithmParams(detector);
};

void ImageAnalyser::printAlgorithmParams( cv::Algorithm *algorithm) {
    std::vector<cv::String> parameters;
    algorithm->getParams(parameters);

    for (int i = 0; i < (int) parameters.size(); i++) {
        std::string param = parameters[i];
        int type = algorithm->paramType(param);
        std::string helpText = algorithm->paramHelp(param);
        std::string typeText;

        switch (type) {
        case cv::Param::BOOLEAN:
            typeText = "bool";
            break;
        case cv::Param::INT:
            typeText = "int";
            break;
        case cv::Param::REAL:
            typeText = "real (double)";
            break;
        case cv::Param::STRING:
            typeText = "string";
            break;
        case cv::Param::MAT:
            typeText = "Mat";
            break;
        case cv::Param::ALGORITHM:
            typeText = "Algorithm";
            break;
        case cv::Param::MAT_VECTOR:
            typeText = "Mat vector";
            break;
        }
        std::cout << "Parameter '" << param << "' type=" << typeText << " help=" << helpText << std::endl;
    }

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
            // double magDiff = eDistance((*imageVector_)[id1].getMag_data(), (*imageVector_)[matchIDvec.at(idx)].getMag_data());;
            boost::add_edge(id1, matchIDvec.at(idx), Weight(1), *G_);
            // boost::add_edge(id1, matchIDvec.at(idx), *G_);
        }
        ++show_progress;
    }

    // double wall1 = get_wall_time();
    // double cpu1  = get_cpu_time();


    printGraph("before");
    // filterPanoramas();
    // shortestPath();
    test(*G_);
};

void ImageAnalyser::test(Graph g) {

    std::vector<MyVertex> p(num_vertices(g));
    std::vector<int> d(num_vertices(g));
    MyVertex start_vertex = vertex(1, g);

    dijkstra_shortest_paths(g, start_vertex, predecessor_map(&p[0]).distance_map(&d[0]));


    std::cout << "distances and parents:" << std::endl;
    graph_traits < Graph >::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
        std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
        std::cout << "node(" << *vi << ") = " << "Parent ( " << p[*vi] << std::endl;
    }
    std::cout << std::endl;


    std::vector< MyVertex > path;
    MyVertex current = vertex(60,g);

    while (current != start_vertex) {
        path.push_back(current);
        current = p[current];
    }
    path.push_back(start_vertex);

    //This prints the path reversed use reverse_iterator and rbegin/rend
    std::vector<MyVertex >::iterator it;
    for (it = path.begin(); it != path.end(); ++it) {

        std::cout << *it << " ";
    }
    std::cout << std::endl;

};

void ImageAnalyser::shortestPath() {
    using namespace boost;

    typedef size_t vertex_descriptor;


    std::vector<int>::iterator it;
    std::vector<int> component(num_vertices(*G_));
    int num = boost::connected_components(*G_, &component[0]);

    for (int idx = 0; idx < num; ++idx) {
        int numberOfElements = std::count(component.begin(), component.end(), idx);

        if (numberOfElements > 3) {
            std::vector<int> idVec;
            int id1 = 0;
            for (int a = 0; a < numberOfElements; ++a) {
                it = find(component.begin() + id1, component.end(), idx);
                id1 = std::distance( component.begin(), it);
                idVec.push_back(id1);
                ++id1;
            }

            double maxDist = 0.0;
            int startID, endID;

            for (int idx = 0; idx < idVec.size() - 1; ++idx) {

                std::vector<int> Xvec = imageVector_->at(idVec[idx]).getMag_data();
                for (int idy = idx + 1; idy < idVec.size(); ++idy) {
                    std::vector<int> Yvec = imageVector_->at(idVec[idy]).getMag_data();
                    double dist = eDistance(Xvec, Yvec);
                    if (maxDist < dist) {
                        maxDist = dist;
                        startID = idVec.at(idx);
                        endID = idVec.at(idy);
                    }
                }
            }

            std::vector<int>::iterator startIt =  find(component.begin(), component.end(), startID);
            std::vector<int>::iterator endIt =  find(component.begin(), component.end(), endID);
            std::vector<MyVertex> p(boost::num_vertices(*G_));
            std::vector<int> d(num_vertices(*G_));
            // std::vector<MyVertex> predecessors(boost::num_vertices(*G_));
            MyVertex start_vertex = distance(component.begin(), startIt);
            MyVertex finish_vertex = distance(component.begin(), endIt);
            vertex_descriptor s = vertex(start_vertex, *G_);
            dijkstra_shortest_paths(*G_, s, predecessor_map(&p[0]).distance_map(&d[0]));

            std::cout << "distances and parents:" << std::endl;
            graph_traits < Graph >::vertex_iterator vi, vend;
            for (boost::tie(vi, vend) = vertices(*G_); vi != vend; ++vi) {
                std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
                std::cout << "parent(" << *vi << ") = " << p[*vi] << std::
                          endl;
            }
            std::cout << std::endl;

        }
    }
}

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

        if (numberOfElements > 3) {
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

    }
    refineGraph();
    printGraph("after");
};

bool cmp(const Imageobject &i, const Imageobject &j) {
    std::vector<int> magData1 = i.getMag_data();
    std::vector<int> magData2 = j.getMag_data();
    double first = sqrt(pow(magData1[0], 2) + pow(magData1[1], 2) + pow(magData1[2], 2));
    double second = sqrt(pow(magData2[0], 2) + pow(magData2[1], 2) + pow(magData2[2], 2));
    return first < second;
};


//TODO needs improvement, try to maximise magDiff
void ImageAnalyser::analyseComponent(std::vector<int> idVec) {

    if (idVec.size() < 3)
        return;


    double maxDist = 0.0;
    int startID, endID;

    for (int idx = 0; idx < idVec.size() - 1; ++idx) {

        std::vector<int> Xvec = imageVector_->at(idVec[idx]).getMag_data();
        for (int idy = idx + 1; idy < idVec.size(); ++idy) {
            std::vector<int> Yvec = imageVector_->at(idVec[idy]).getMag_data();
            double dist = eDistance(Xvec, Yvec);
            if (maxDist < dist) {
                maxDist = dist;
                startID = idVec.at(idx);
                endID = idVec.at(idy);
            }
        }
    }

    std::vector<Imageobject> objectVector;
    for (std::vector<int>::iterator it = idVec.begin(); it != idVec.end(); ++it) {
        objectVector.push_back(imageVector_->at(*it));
    }


    std::sort (objectVector.begin(), objectVector.end(), cmp);


    // DEBUG ONLY
    cout << "Printing vectors" << endl;
    for (std::vector<Imageobject>::iterator itr = objectVector.begin(); itr != objectVector.end(); ++itr) {
        std::vector<int> magData1 = itr->getMag_data();
        double first = sqrt(pow(magData1[0], 2) + pow(magData1[1], 2) + pow(magData1[2], 2));
        cout << first << endl;
    }
    cout << "------------------------------------------------------------------------------------" << endl;
    cout << "------------------------------------------------------------------------------------" << endl;


    bool start = false;
    bool end = false;


    for (int idx = 0; idx <  idVec.size(); ++idx) {
        int id1 = idVec.at(idx);
        if (imageVector_->at(id1).getStatus() != NONE )
            continue;

        if (checkMagDiff(id1, startID, imageVector_) && !start) {
            start = true;
            imageVector_->at(id1).setStatus(INCLUDED);
            imageVector_->at(id1).setFirstMatchID(startID);
            continue;
        } else if (checkMagDiff(id1, endID, imageVector_) && !end) {
            end = true;
            imageVector_->at(id1).setStatus(INCLUDED);
            imageVector_->at(id1).setFirstMatchID(endID);
            continue;
        }

        for (int idy = 0; idy <  idVec.size(); ++idy) {
            int id2 = idVec.at(idy);
            if (id1 == id2 || imageVector_->at(id2).getStatus() != NONE) {
                continue;
            } else if (checkMagDiff(id1, id2, imageVector_) && checkTimeDiff(id1, id2, imageVector_)) {
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

void ImageAnalyser::addToComponent(std::vector<int> idVec, int ID) {
    for (int idx = 0; idx <  idVec.size(); ++idx) {
        int id1 = idVec.at(idx);
        if (imageVector_->at(id1).getStatus() == INCLUDED)
            if (imageVector_->at(id1).getFirstMatchID() == -1)
                imageVector_->at(id1).setFirstMatchID(ID);
    }
};

bool ImageAnalyser::isIncluded(std::vector<int> idVec, int ID) {
    for (int idx = 0; idx <  idVec.size(); ++idx) {
        int id1 = idVec.at(idx);
        if (imageVector_->at(id1).getStatus() == INCLUDED)
            if (id1 == ID || imageVector_->at(id1).getFirstMatchID() == ID)
                return true;
    }
    return false;
}

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