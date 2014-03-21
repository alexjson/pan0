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

    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();

    Dataparser *parser = new Dataparser();
    parser->parseData(PATH);

    vector<Imageobject> *imageVector = parser->getImageVector();

    ImageAnalyser *analyser = new ImageAnalyser(imageVector);
    // analyser->setMatchginThreshold(40);  DEFAULT VALUE

    cout << " analyse... ";
    analyser->analyse();
    cout << "Done." << endl;

    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();
    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "CPU Time  = " << cpu1  - cpu0  << endl;

    // Graph *G = analyser->getGraph();

    // Pan0Stitcher *stitcher = new Pan0Stitcher(imageVector, PATH); //120(2.25*120=270) degree rotation default


    // stitcher->setGraph(G);
    // stitcher->setLookUpMap(analyser->getLookUpMap());
    // stitcher->stitch();

    return 0;
};