#ifndef PAN0STITCHER_H
#define PAN0STITCHER_H
#define _USE_MATH_DEFINES

#include <bfsvertexvisitor.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <imageobject.h>
#include <opencv2/stitching/warpers.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <utils.h>

using namespace std;
using namespace cv;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;


class Pan0Stitcher {
public:
    Pan0Stitcher(std::vector<Imageobject> *imageVector, string PATH);
    ~Pan0Stitcher();

    void stitch();
    void setGraph(Graph* g){
        graph_=g;
    };
    Mat getHomography(int id1, int id2, std::vector<DMatch> good_matches);
    cv::Point2f convertPoints(cv::Point2f points, int w, int h);
    Mat mapImgToCyl(Mat img);
    cv::Point2f convertTest(cv::Point2f point, int w, int h);
    void GraphTraverse(std::vector<int> G);
    void add(int id);
    bool checkSequence();

private:
    std::vector<Imageobject> *imageVector_;
    std::vector<Mat> imagesToStitch_;
    Graph* graph_;
    string path_;
    std::vector<int> idsToStitch_;
};

#endif //PAN0STITCHER_H