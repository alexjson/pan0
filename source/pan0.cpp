#include </home/alex/xjobb/c++/include/pan0.h>

int SIZE = 4;
string PATH = "";
double eDistance(vector<int> vec1, vector<int> vec2);
void stitchTest(vector<string> filesToStitch, string PATH);
std::vector<string> findCandidates(vector<Imageobject> *imageVector);


using namespace cv;
using namespace std;

int main( int argc, char **argv ) {

    if (argc < 2) {

        PATH = "/home/alex/xjobb/images/test/";
        // std::cerr << "Usage: " << argv[0] << " PATH" << std::endl;
        // return 1;
    } else {
        PATH = argv[1];
    }

    Dataparser *parser = new Dataparser();
    parser->parseData(PATH);

    vector<Imageobject> *imageVector = parser->getImageVector();


    std::vector<string>  imageNames = findCandidates(imageVector);
    imageVector->clear();
    parser->parseImages(PATH, imageNames);
    imageVector = parser->getImageVector();

    ImageAnalyser *analyser = new ImageAnalyser(imageVector);

    analyser->calculateDescriptors();

    analyser->analyse();
    std::vector< std::vector<int> > *panoIDVec = analyser->findPanoramas();

    

    Pan0Stitcher *stitcher = new Pan0Stitcher(imageVector);
    stitcher->setPanoIDs(panoIDVec);
    stitcher->stitch();

    return 0;
    // if (imageNames.size() > SIZE) {
    //     stitchTest(imageNames, PATH);
    // } else {
    //     cout << "To few matching images" << endl;
    // }
};



double eDistance(std::vector<int> vec1, std::vector<int> vec2) {
    double x1 = vec1[0];
    double x2 = vec2[0];
    double y1 = vec1[1];
    double y2 = vec2[1];
    double z1 = vec1[2];
    double z2 = vec2[2];
    double dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

    return dist;
};


std::vector<string> findCandidates(vector<Imageobject> *imageVector) {
    double magDiff = 0.0;
    std::map<string, string> tmp;
    for (int i = 0; i < imageVector->size(); ++i) {
        for (int k = 0; k < imageVector->size(); ++k) {
            magDiff = eDistance((*imageVector)[i].getMag_data(), (*imageVector)[k].getMag_data());

            if (magDiff > 25.0 && magDiff < 55.0) {
                tmp.insert(std::map<string, string>::value_type((*imageVector)[i].getFileName(),
                           (*imageVector)[k].getFileName()));
            }

        }

    }

    std::vector<string> result;
    std::map<string, string>::iterator it = tmp.begin();
    for (it = tmp.begin(); it != tmp.end(); ++it) {
        result.push_back(it->first);
        result.push_back(it->second);
    }

    cout << "Removing duplicates" << endl;
    sort( result.begin(), result.end() );
    result.erase( unique( result.begin(), result.end() ), result.end() );
    cout << "done." << endl;

    // for (std::vector<string>::iterator it = result.begin(); it != result.end(); ++it) {
    //     cout << *it << endl;
    // }

    return result;
}


void stitchTest(vector<string> filesToStitch, string PATH) {

    cout << "stitching!" << endl;
    string tmpPath = PATH;
    vector< Mat > vImg;
    Mat rImg;

    for (int i = 0; i < filesToStitch.size(); ++i) {
        vImg.push_back( imread(tmpPath + filesToStitch[i]));
    }


    Stitcher stitcher = Stitcher::createDefault();


    unsigned long AAtime = 0, BBtime = 0; //check processing time
    AAtime = getTickCount(); //check processing time

    stitcher.stitch(vImg, rImg);

    BBtime = getTickCount(); //check processing time
    printf("%.2lf sec \n",  (BBtime - AAtime) / getTickFrequency() ); //check processing time

    imshow("Stitching Result", rImg);

    waitKey(0);

};
