// dataparser.cpp
#include </home/alex/xjobb/c++/include/dataparser.h>

using namespace std;
using namespace cv;

Dataparser::Dataparser() {
    imageVector_ = new std::vector<Imageobject>();
}

void Dataparser::parseData(std::string dataPath) {

    Json::Reader reader;
    Json::Value root;
    string tmp = "";
    string jsonFile = "";

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dataPath.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            tmp = ent->d_name;
            if (tmp.find(".jpg") != string::npos) {
                jsonFile = dataPath + tmp.substr(0, tmp.find(".jpg")) + ".json";
                ifstream test(jsonFile.c_str(), ifstream::binary);
                reader.parse(test, root, false);
                Imageobject imgObj(root, tmp);
                imageVector_->push_back(imgObj);
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
    }
}

void Dataparser::parseImages(std::string dataPath, std::vector<string> imageNames) {

    imageVector_->clear();
    Json::Reader reader;
    Json::Value root;
    string tmp = "";
    string jsonFile = "";

    for (std::vector<string>::iterator it = imageNames.begin(); it != imageNames.end(); ++it) {
        tmp = *it;
        //Read as grayscale
        Mat img = imread(dataPath + "/" + tmp,0);
        jsonFile = dataPath + tmp.substr(0, tmp.find(".jpg")) + ".json";
        ifstream test(jsonFile.c_str(), ifstream::binary);
        reader.parse(test, root, false);
        Imageobject imgObj(root, img, tmp);
        imageVector_->push_back(imgObj);
    }
}
