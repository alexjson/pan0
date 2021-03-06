#include <imageobject.h>

using namespace std;
using namespace cv;

Imageobject::Imageobject(Json::Value jsonRoot, string fileName, int id, string path) :
    jsonRoot_(jsonRoot),
    fileName_(fileName),
    currentID_(id),
    PATH_(path){
    const unsigned int idx = 0;
    for (int i = 0; i <= 2; ++i) {
        mag_data.push_back(jsonRoot_["mag_data"]["samples"][idx][i].asInt());
        acc_data.push_back(jsonRoot_["acc_data"]["samples"][idx][i].asInt());
    }
    triggerMethod_ = jsonRoot_["trigger"].asString();
    timeFromFileName();

    tiltAdapter();
    rollAdapter();
};

Imageobject::~Imageobject() {};

void Imageobject::timeFromFileName() {

    using namespace boost::posix_time;
    using namespace boost::gregorian;

    date d(1970, Jan, 1); //an arbitrary date
    int H = atoi(fileName_.substr(0, 2).c_str());
    int M = atoi(fileName_.substr(2, 2).c_str());
    int S = atoi(fileName_.substr(4, 2).c_str());
    ptime t1 (d, hours(H) + minutes(M) + seconds(S) + millisec(0));
    taken_at = t1;
};
void Imageobject::setImageFeatures() {
    imageFeatures_.img_idx = currentID_;
    imageFeatures_.img_size = image_.size();
    imageFeatures_.keypoints = keyPointVec_;
    imageFeatures_.descriptors = descriptors_;
};
void Imageobject::tiltAdapter() {
    int ax, ay, az;
    ax = acc_data[0];
    ay = acc_data[1];
    az = acc_data[2];

    double horizontal = sqrt(pow(ax, 2) + pow(ay, 2));
    double tilt = atan2(az, horizontal);
    tilt_ = tilt;
};

void Imageobject::rollAdapter() {
#define _USE_MATH_DEFINES
    int ax, ay;
    ax = acc_data[0];
    ay = acc_data[1];
    double roll = fmod((-M_PI / 2 - atan2(ax, ay)) , (2 * M_PI));
    roll_ = roll;
};
double Imageobject::getRollDegrees() {
#define _USE_MATH_DEFINES
    return (roll_ * (180 / M_PI));
};

void Imageobject::setKeyPoints(std::vector<KeyPoint> keyPoints) {
    keyPointVec_ = keyPoints;
};

void Imageobject::setDescriptors(Mat descriptors) {
    descriptors_ = descriptors;
};

void Imageobject::loadImage() {
    image_ = imread(PATH_ + fileName_);
};