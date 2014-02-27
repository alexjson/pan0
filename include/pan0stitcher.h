#ifndef PAN0STITCHER_H
#define PAN0STITCHER_H
#define _USE_MATH_DEFINES

#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <imageobject.h>
#include <bfsvertexvisitor.h>
#include <opencv2/stitching/warpers.hpp>
#include </usr/local/include/opencv2/stitching/stitcher.hpp>

using namespace std;
using namespace cv;
using namespace boost;

class Pan0Stitcher {
public:
    Pan0Stitcher(std::vector<Imageobject> *imageVector);
    ~Pan0Stitcher();

    void stitch();
    void setPanoIDs(std::vector< std::vector<int> > *panoIDVec) {
        panoIDVec_ = panoIDVec;
    };
    Mat getHomography(int id1, int id2, std::vector<DMatch> good_matches);
    cv::Point2f convertPoints(cv::Point2f points, int w, int h);
    Mat mapImgToCyl(Mat img);
    cv::Point2f convertTest(cv::Point2f point, int w, int h);
    void GraphTest(std::vector<int> G);
    void add(int id);

private:
    std::vector<Imageobject> *imageVector_;
    std::vector< std::vector<int> > *panoIDVec_;
};

#endif //PAN0STITCHER_H