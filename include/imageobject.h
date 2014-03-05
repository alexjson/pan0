#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <opencv2/opencv.hpp>
#include </usr/include/jsoncpp/json.h>

using namespace cv;
using namespace std;

enum nodeStatus { NONE, INCLUDED, REJECTED  };

class Imageobject {
public:
    Imageobject(Json::Value jsonRoot, Mat image, string fielName, int id);
    Imageobject(Json::Value jsonRoot, string fielName, int id);
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

    void setImage(Mat image) {
        image_ = image;
    };


    void setKeyPoints(std::vector<KeyPoint> keyPoints);
    void setDescriptors(Mat descriptors);

    Mat getDescriptors() const {
        return descriptors_;
    };
    vector<KeyPoint> getKeypoints() const {
        return keyPointVec_;
    };
    int getFirstMatchID() const {
        return firstMatchID_;
    };
    int getSecondMatchID() const {
        return secondMatchID_;
    };


    void setFirstMatchID(int match) {
        firstMatchID_ = match;
    };
    void setSecondMatchID(int match) {
        secondMatchID_ = match;
    };

    void setFirstMatches(vector<DMatch> matches) {
        firstBestMatches_ = matches;
    };
    void setSecondMatches(vector<DMatch> matches) {
        secondBestMatches_ = matches;
    };

    std::vector<DMatch> getFirstMatches() {
        return firstBestMatches_;
    };
    std::vector<DMatch> getSecondMatches() {
        return secondBestMatches_;
    };

    int getID() {
        return currentID_;
    };

    nodeStatus getStatus(){
        return status_;
    };

    void setStatus(nodeStatus status){
        status_ = status;
    }


private:
    Json::Value jsonRoot_;
    Mat image_;
    string fileName_;
    vector<int> mag_data;
    vector<int> acc_data;
    std::vector<KeyPoint> keyPointVec_;
    Mat descriptors_;
    string triggerMethod_;
    int currentID_;
    int firstMatchID_;
    int secondMatchID_;
    Mat homographyFirstMatch_;
    Mat homographySecondtMatch_;
    vector<DMatch> firstBestMatches_;
    vector<DMatch> secondBestMatches_;
    nodeStatus status_;
    
};


#endif //IMAGEOBJECT_H
