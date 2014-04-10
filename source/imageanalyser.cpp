#include <imageanalyser.h>
#include <omp.h>

using namespace std;
using namespace cv;

ImageAnalyser::ImageAnalyser(std::vector<Imageobject> *imageVector) :
    imageVector_(imageVector), MATCHTRESH_(20) {
    matcher = DescriptorMatcher::create("BruteForce"); // FlannBased , BruteForce
    detector = FeatureDetector::create("ORB");
    // detector->set("nFeatures", 500);
    extractor = DescriptorExtractor::create("ORB"); // ORB SIFT
    RATIO_ = 0.81;
    // RATIO_ = 0.7; //SIFT VALUE, MATCHTRESH 40

    // printAlgorithmParams(detector);  // For printing detector variables
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
    imageVector_->at(id).setImageFeatures();

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
            // const float ratio = 0.9; // As in Lowe's SIFT paper; can be tuned

            if (matches[i][0].distance < RATIO_ * matches[i][1].distance) {
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

    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(4); // Use 4 cores
    #pragma omp parallel for
    for (int id1 = 0; id1 < imageVector_->size(); ++id1) {
        numberOfMatches = MATCHTRESH_;
        std::vector<int> matchIDvec;


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
            boost::add_edge(id1, matchIDvec.at(idx), Weight(1), *G_);
        }
        ++show_progress;
    }

    printGraph("before");
    shortestPath(*G_);
};

void ImageAnalyser::shortestPath(Graph g) {

    std::vector<int>::iterator it;
    std::vector<int> component(num_vertices(*G_));
    int num = boost::connected_components(*G_, &component[0]);

    for (int idx = 0; idx < num; ++idx) {
        int numberOfElements = std::count(component.begin(), component.end(), idx);

        if (numberOfElements > 3) {
            int startID, endID;
            std::vector<int> idVec;
            int id1 = 0;
            for (int a = 0; a < numberOfElements; ++a) {
                it = find(component.begin() + id1, component.end(), idx);
                id1 = std::distance( component.begin(), it);
                idVec.push_back(id1);
                ++id1;
            }

            double maxDist = 0.0;

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
            std::vector<MyVertex> p(num_vertices(g));
            std::vector<int> d(num_vertices(g));
            MyVertex start_vertex = vertex(startID, g);

            dijkstra_shortest_paths(g, start_vertex, predecessor_map(&p[0]).distance_map(&d[0]));

            std::vector<int> ID;
            MyVertex current = vertex(endID, g);

            while (current != start_vertex) {
                ID.push_back(current);
                current = p[current];
            }
            ID.push_back(start_vertex);

            imageIDs.push_back(ID);

        }
    }

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
