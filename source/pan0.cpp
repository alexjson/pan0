#include <pan0.h>

// int SIZE = 4;
string PATH = "";
std::vector<string> findCandidates(vector<Imageobject> *imageVector);

using namespace cv;
using namespace std;

int main( int argc, char **argv ) {

    if (argc < 2) {

        // PATH = "/home/alex/xjobb/images2/resized/";
        std::cerr << "Usage: " << argv[0] << " PATH" << std::endl;
        return 1;
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
    // analyser->setMatchginThreshold(40);  DEFAULT VALUE

    cout << "Calculate descriptors... ";
    analyser->calculateDescriptors();
    // cout << "Calculate AKAZE... ";
    // analyser->calculateAKAZE();
    cout << "Done." << endl;


    cout << " analyse... ";
    analyser->analyse();
    cout << "Done." << endl;

    Graph *G = analyser->getGraph();

    Pan0Stitcher *stitcher = new Pan0Stitcher(imageVector, PATH);
    // stitcher->setMinimumRotation(120)  DEFAULT VALUE


    stitcher->setGraph(G);
    stitcher->setLookUpMap(analyser->getLookUpMap());
    stitcher->stitch();

    return 0;
};

std::vector<string> findCandidates(vector<Imageobject> *imageVector) {
    std::map<string, string> tmp;
    for (int i = 0; i < imageVector->size(); ++i) {
        for (int k = 0; k < imageVector->size(); ++k) {
            if (checkMagDiff(i, k, imageVector) && CheckTiltDiff(i, k, imageVector) && checkTimeDiff(i ,k, imageVector)) {
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

    sort( result.begin(), result.end() );
    result.erase( unique( result.begin(), result.end() ), result.end() );

    return result;
}


