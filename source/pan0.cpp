#include <pan0.h>

int SIZE = 4;
string PATH = "";
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

    cout << "Parsing data... ";
    vector<Imageobject> *imageVector = parser->getImageVector();
    cout << "Done." << endl;

    std::vector<string>  imageNames = findCandidates(imageVector);
    imageVector->clear();

    cout << "Parsing images... ";
    parser->parseImages(PATH, imageNames);
    cout << "Done." << endl;
    imageVector = parser->getImageVector();

    cout << "Init analyser... ";
    ImageAnalyser *analyser = new ImageAnalyser(imageVector);
    cout << "Done." << endl;

    cout << "Calculate descriptors... ";
    analyser->calculateDescriptors();
    cout << "Done." << endl;


    cout << " analyse... ";
    analyser->analyse();
    cout << "Done." << endl;

    // Graph *G = analyser->findPanoramas();

    Graph *G = analyser->getGraph();



    Pan0Stitcher *stitcher = new Pan0Stitcher(imageVector, PATH);
    stitcher->setGraph(G);
    stitcher->stitch();

    return 0;
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

