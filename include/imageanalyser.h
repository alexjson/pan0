#ifndef IMAGEANALYSER_H
#define IMAGEANALYSER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <imageobject.h>
#include <opencv2/stitching/stitcher.hpp>
#include <utils.h>
#include <bfsvertexvisitor.h>
#include <boost/graph/graphviz.hpp>
#include <boost/progress.hpp>

using namespace std;
using namespace cv;
class Imageobject;

class ImageAnalyser {
public:
    ImageAnalyser(std::vector<Imageobject> *imageVector);
    ~ImageAnalyser();

    void analyse();
    bool verifyImage(int id1, int id2, vector<DMatch> matches);
    int checkMatches(int id1, int id2);
    void printAlgorithmParams( cv::Algorithm* algorithm);
    void printGraph(string fileName, bool names = false);
    void extractDescriptors(int id);
    bool checkEdge(int id1, int id2);
    void setMatchingThreshold(int tresh){
        MATCHTRESH_ = tresh;
    };
    void shortestPath(Graph g);
    std::vector< std::vector<int> > getImageIDs(){
        return imageIDs;
    };
    void printKP(int id);
    void printMatches(int id1, int id2, vector<cv::DMatch> matchVec);
    void printMatches2(int id1, int id2, std::vector< vector<cv::DMatch> >  matchVec);

private:
    Ptr<DescriptorMatcher> matcher;
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;
    std::vector<Imageobject> *imageVector_;
    Graph *G_;
    std::vector< std::vector<int> > imageIDs;
    int MATCHTRESH_;
    float RATIO_;
};

#endif