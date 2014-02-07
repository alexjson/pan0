#include <opencv2/opencv.hpp>
#include </usr/include/jsoncpp/json.h>

using namespace cv;
using namespace std;
class Imageobject
{
public:
	Imageobject(Json::Value jsonRoot, Mat image, string fielName);
	~Imageobject();

	vector<int> getMag_data(){return mag_data;};
	vector<int> getAcc_data(){return acc_data;};
	string getTrigger(){return triggerMethod_;};
	string getFileName(){return fileName_;};
	Mat getImage(){return image_;};

	void setKeyPoints(std::vector<KeyPoint> keyPoints);
	void setDescriptors(Mat descriptors);

	Mat getDescriptors(){return descriptors_;};
	vector<KeyPoint> getDKeypoints(){return keyPointVec_;};

private:
	Json::Value jsonRoot_;
	Mat	image_;
	string fileName_;
	vector<int> mag_data;
	vector<int> acc_data;
	std::vector<KeyPoint> keyPointVec_;
	Mat descriptors_;
	string triggerMethod_;
};