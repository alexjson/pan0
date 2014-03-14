#include <pan0.h>

int SIZE = 4;
string PATH = "";
std::vector<string> findCandidates(vector<Imageobject> *imageVector);
// bool checkMagDiff(int id1, int id2,    vector<Imageobject> *imageVector);
// bool CheckTiltDiff(int id1, int id2,    vector<Imageobject> *imageVector);



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

    // cout << "Parsing data... ";
    vector<Imageobject> *imageVector = parser->getImageVector();
    // cout << "Done." << endl;

    std::vector<string>  imageNames = findCandidates(imageVector);
    imageVector->clear();

    // cout << "Parsing images... ";
    parser->parseImages(PATH, imageNames);
    // cout << "Done." << endl;
    imageVector = parser->getImageVector();

    // cout << "Init analyser... ";
    ImageAnalyser *analyser = new ImageAnalyser(imageVector);
    // cout << "Done." << endl;

    cout << "Calculate descriptors... ";
    analyser->calculateDescriptors();
    cout << "Done." << endl;


    cout << " analyse... ";
    analyser->analyse();
    cout << "Done." << endl;

    // Graph *G = analyser->getGraph();


    // Pan0Stitcher *stitcher = new Pan0Stitcher(imageVector, PATH);
    // stitcher->setGraph(G);
    // stitcher->setLookUpMap(analyser->getLookUpMap());
    // stitcher->stitch();

    return 0;
};



std::vector<string> findCandidates(vector<Imageobject> *imageVector) {
    std::map<string, string> tmp;
    for (int i = 0; i < imageVector->size(); ++i) {
        for (int k = 0; k < imageVector->size(); ++k) {
            //TDOD: Add time check here to prevent bad inputs
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

    cout << "Removing duplicates" << endl;
    sort( result.begin(), result.end() );
    result.erase( unique( result.begin(), result.end() ), result.end() );
    cout << "done." << endl;

    return result;
}

// bool checkMagDiff(int id1, int id2, vector<Imageobject> *imageVector) {
//     double magDiff = eDistance((*imageVector)[id1].getMag_data(), (*imageVector)[id2].getMag_data());

//     return (magDiff > 21.0 && magDiff < 45.0);
// };
// bool CheckTiltDiff(int id1, int id2, vector<Imageobject> *imageVector) {

//     double tiltDiff = abs((*imageVector)[id1].getTilt() - (*imageVector)[id2].getTilt());
//     return tiltDiff < 2.5;

// };

