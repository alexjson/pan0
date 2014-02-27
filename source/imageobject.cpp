#include <imageobject.h>

using namespace std;
using namespace cv;

Imageobject::Imageobject(Json::Value jsonRoot, Mat image, string fileName) :
    jsonRoot_(jsonRoot),
    image_(image),
    fileName_(fileName),
    secondMatchID_(-1) {
    unsigned int idx = 0;
    for (int i = 0; i <= 2; ++i) {
        mag_data.push_back(jsonRoot_["meta"]["mag_data"]["samples"][idx][i].asInt());
        acc_data.push_back(jsonRoot_["meta"]["acc_data"]["samples"][idx][i].asInt());
    }
    triggerMethod_ = jsonRoot_["meta"]["trigger"].asString();

};
Imageobject::Imageobject(Json::Value jsonRoot, string fileName) :
    jsonRoot_(jsonRoot),
    fileName_(fileName),
    secondMatchID_(-1) {
    unsigned int idx = 0;
    for (int i = 0; i <= 2; ++i) {
        mag_data.push_back(jsonRoot_["meta"]["mag_data"]["samples"][idx][i].asInt());
        acc_data.push_back(jsonRoot_["meta"]["acc_data"]["samples"][idx][i].asInt());
    }
    triggerMethod_ = jsonRoot_["meta"]["trigger"].asString();
};

Imageobject::~Imageobject() {};

void Imageobject::setKeyPoints(std::vector<KeyPoint> keyPoints) {
    keyPointVec_ = keyPoints;
};

void Imageobject::setDescriptors(Mat descriptors) {
    descriptors_ = descriptors;
};