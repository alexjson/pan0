#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <opencv2/opencv.hpp>
#include <jsoncpp/json.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <opencv2/stitching/detail/matchers.hpp>


using namespace cv;
using namespace std;

class Imageobject {
public:
    Imageobject(Json::Value jsonRoot, string fielName, int id, string path);
    ~Imageobject();

    bool operator==(const Imageobject &rhs) {
        return this->getFileName() == rhs.getFileName();
    };
    vector<int> getMag_data() const {
        return mag_data;
    };
    vector<int> getAcc_data() const {
        return acc_data;
    };
    string getTrigger() const {
        return triggerMethod_;
    };
    string getFileName() const {
        return fileName_;
    };
    Mat getImage() const {
        return image_;
    };

    double getTilt() const {
        return tilt_;
    };

    double getRoll() const {
        return roll_;
    };

    double getRollDegrees();

    void setImage(Mat image) {
        image_ = image;
    };

    void loadImage();
    void setKeyPoints(std::vector<KeyPoint> keyPoints);
    void setDescriptors(Mat descriptors);

    Mat getDescriptors() const {
        return descriptors_;
    };
    vector<KeyPoint> getKeypoints() const {
        return keyPointVec_;
    };

    int getID() {
        return currentID_;
    };

    boost::posix_time::ptime getTime(){
        return taken_at;
    };

    void tiltAdapter();
    void rollAdapter();
    void timeFromFileName();
    void setImageFeatures();
    cv::detail::ImageFeatures getImageFeatures(){
        return imageFeatures_;
    }
private:
    Json::Value jsonRoot_;
    string PATH_;
    Mat image_;
    string fileName_;
    vector<int> mag_data;
    vector<int> acc_data;
    double tilt_;
    double roll_;
    std::vector<KeyPoint> keyPointVec_;
    Mat descriptors_;
    string triggerMethod_;
    int currentID_;
    boost::posix_time::ptime taken_at;
    cv::detail::ImageFeatures imageFeatures_;
};


#endif //IMAGEOBJECT_H
