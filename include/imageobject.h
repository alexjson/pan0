#include <opencv2/opencv.hpp>
#include </usr/include/jsoncpp/json.h>

using namespace cv;
using namespace std;
class Imageobject {
public:
    Imageobject(Json::Value jsonRoot, Mat image, string fielName);
    Imageobject(Json::Value jsonRoot, string fielName);
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
    vector<KeyPoint> getDKeypoints() const {
        return keyPointVec_;
    };

private:
    Json::Value jsonRoot_;
    Mat image_;
    string fileName_;
    vector<int> mag_data;
    vector<int> acc_data;
    std::vector<KeyPoint> keyPointVec_;
    Mat descriptors_;
    string triggerMethod_;
};



