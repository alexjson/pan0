#include <cstdio>
#include <cstring>
#include <vector>
#include </usr/include/jsoncpp/json.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <fstream>
#include </home/alex/xjobb/c++/include/imageobject.h>


using namespace cv;

class Dataparser {
public:
    Dataparser();
    ~Dataparser();

    std::vector<Imageobject> getImageVector() {
        return *imageVector_;
    };
    // std::vector<Json::Value> getJsonVector(){return *jsonVector_; };
    void parseData(std::string dataPath);

private:
    std::vector<Imageobject> *imageVector_;
};