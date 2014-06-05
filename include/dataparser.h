#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <cstdio>
#include <cstring>
#include <vector>
// #include </usr/include/jsoncpp/json.h>
#include <../libs/jsoncpp/json.h>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <fstream>
#include <imageobject.h>


using namespace cv;

class Dataparser {
public:
    Dataparser();
    ~Dataparser();

    std::vector<Imageobject>* getImageVector() {
        return imageVector_;
    };
    void parseData(std::string dataPath);

private:
    std::vector<Imageobject> *imageVector_;
};

#endif