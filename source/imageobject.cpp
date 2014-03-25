#include <imageobject.h>

using namespace std;
using namespace cv;

Imageobject::Imageobject(Json::Value jsonRoot, Mat image, string fileName, int id) :
    jsonRoot_(jsonRoot),
    image_(image),
    fileName_(fileName),
    currentID_(id),
    firstMatchID_(-1),
    secondMatchID_(-1),
    matched_(false),
    status_(NONE) {
    using namespace boost::posix_time;

    unsigned int idx = 0;
    for (int i = 0; i <= 2; ++i) {
        mag_data.push_back(jsonRoot_["meta"]["mag_data"]["samples"][idx][i].asInt());
        acc_data.push_back(jsonRoot_["meta"]["acc_data"]["samples"][idx][i].asInt());
    }
    triggerMethod_ = jsonRoot_["meta"]["trigger"].asString();

    string time = jsonRoot_["taken_at_utc"].asString();
    time.erase (time.end() - 6, time.end());
    taken_at = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(time, 'T');

    const unsigned int idx2 = 0;
    tilt_ = jsonRoot_["meta"]["rotation"][idx2].asDouble();
};
Imageobject::Imageobject(Json::Value jsonRoot, string fileName, int id, string path) :
    jsonRoot_(jsonRoot),
    fileName_(fileName),
    currentID_(id),
    PATH_(path),
    firstMatchID_(-1),
    secondMatchID_(-1),
    matched_(false),
    status_(NONE) {
    const unsigned int idx = 0;
    for (int i = 0; i <= 2; ++i) {
        mag_data.push_back(jsonRoot_["mag_data"]["samples"][idx][i].asInt());
        acc_data.push_back(jsonRoot_["acc_data"]["samples"][idx][i].asInt());
    }
    triggerMethod_ = jsonRoot_["trigger"].asString();
    timeFromFileName();

    // string time = jsonRoot_["taken_at_utc"].asString();
    // time.erase (time.end() - 6, time.end());
    // taken_at = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(time, 'T');

    // const unsigned int idx2 = 0;
    // tilt_ = jsonRoot_["meta"]["rotation"][idx2].asDouble();
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
    double roll = fmod((-M_PI/ 2 - atan2(ax, ay)) , (2 * M_PI));
    cout << "Roll " << roll << endl;
    roll_ = roll;
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