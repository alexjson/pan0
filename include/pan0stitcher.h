#ifndef PAN0STITCHER_H
#define PAN0STITCHER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include </home/alex/xjobb/c++/include/imageobject.h>

using namespace std;
using namespace cv;

class Pan0Stitcher {
public:
    Pan0Stitcher(std::vector<Imageobject> *imageVector);
    ~Pan0Stitcher();

    void stitch();
    void setPanoIDs(std::vector< std::vector<int> >* panoIDVec) {
        panoIDVec_ = panoIDVec;
    };
    Mat getHomography(int id1, int id2);

private:
    std::vector<Imageobject> *imageVector_;
    std::vector< std::vector<int> > *panoIDVec_;
};


#endif //PAN0STITCHER_H