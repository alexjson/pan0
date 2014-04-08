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
#include <stitching_detailed.h>

using namespace std;
using namespace cv;
using namespace boost;
typedef property < edge_weight_t, int >Weight;
typedef adjacency_list < listS, vecS, undirectedS,
        no_property, property < edge_weight_t, int > > Graph;

class Pan0Stitcher {
public:
    Pan0Stitcher(std::vector<Imageobject> *imageVector, string PATH);
    ~Pan0Stitcher();

    void stitch();
    void setGraph(Graph *g) {
        graph_ = g;
    };
    void setLookUpMap(std::map<int, int> map) {
        lookUpMap_ = map;
    };
    // void add(int id);
    bool checkSequence();
    void printID();
    void generateOutput(int id);
    void parseImgs();
    void writeImg(int id, Mat img);
    void setMinimumRotation(int rot) {
        MINDIST_ = rot;
    }
    void setIdsToStitch(  std::vector < std::vector<int>  > idVec) {
        idVec_ = idVec;
    }
    void prepareImages();
private:
    std::vector<Imageobject> *imageVector_;
    std::vector<Mat> imagesToStitch_;
    Graph *graph_;
    std::map<int, int> lookUpMap_;
    string path_;
    std::vector<int> idsToStitch_;
    std::vector < std::vector<int>  > idVec_;
    int MINDIST_;
    Ptr<DescriptorMatcher> matcher;
    Ptr<FeatureDetector> detector;
    Ptr<DescriptorExtractor> extractor;
};

#endif //PAN0STITCHER_H