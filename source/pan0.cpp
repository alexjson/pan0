#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include </usr/local/include/opencv2/stitching/stitcher.hpp>
#include </home/alex/xjobb/c++/include/pan0.h>
#include </home/alex/xjobb/c++/include/dataparser.h>

int SIZE = 4;
string PATH = "";
double eDistance(vector<int> vec1, vector<int> vec2);
map<pair<string, string>, vector< DMatch> > calculateMatches(vector<Imageobject> imageVector);
std::vector<string> filterImages( vector<Imageobject> imageVector, map<pair<string, string>, vector< DMatch> > matchLookUp);
void stitchTest(vector<string> filesToStitch, string PATH);
std::vector<string> findCandidates(vector<Imageobject> imageVector);


using namespace cv;
using namespace std;

int main( int argc, char **argv ) {


    // Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " PATH" << std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }

    PATH = argv[1];



    
    Dataparser *parser = new Dataparser();
    parser->parseData(PATH);
      
    vector<Imageobject> imageVector = parser->getImageVector();

    std::vector<string>  imageNames = findCandidates(imageVector);

    // if (imageNames.size() > SIZE) {
    //     stitchTest(imageNames,PATH);
    // }else{
    //     cout << "To few matching images" << endl;
    // }




    // vector<KeyPoint> keypoints;
    // Mat descriptors;

    // vector< vector < KeyPoint > > keyPointVector;
    // std::vector<Mat> descriptorsVector;
    // SiftFeatureDetector featureDetector;
    // SiftDescriptorExtractor featureExtractor;

    // cout << "Calculating descriptors" << endl;
    // for (std::vector<Imageobject>::iterator i = imageVector.begin(); i != imageVector.end(); ++i) {
    //     featureDetector.detect(i->getImage(), keypoints);
    //     i->setKeyPoints(keypoints);
    //     featureExtractor.compute(i->getImage(), keypoints, descriptors);
    //     i->setDescriptors(descriptors);
    // }
    // cout << "done." << endl;

    // map<pair<string, string>, vector< DMatch> >  matchLookUp = calculateMatches(imageVector);
    // std::vector<string> toStitch =  filterImages(imageVector, matchLookUp);

    // cout << "Images to stitch" << endl;
    // for (int i = 0; i < toStitch.size(); ++i) {
    //     cout << toStitch[i] << endl;
    // }

};



map<pair<string, string>, vector< DMatch> > calculateMatches(vector<Imageobject> imageVector) {
    cout << "matching" << endl;
    FlannBasedMatcher flannMatcher;
    map<pair<string, string>, vector< DMatch> > matchLookUp;

    std::vector< DMatch > matches;
    for (int i = 0; i <= imageVector.size() ; ++i) {
        for (int k = i + 1; k < imageVector.size(); ++k) {

            // cout <<  imageVector[i].getFileName() << "          " << imageVector[k].getFileName() << endl;


            std::vector< DMatch > good_matches;
            double max_dist = 0; double min_dist = 100;
            flannMatcher.match(imageVector[i].getDescriptors(), imageVector[k].getDescriptors(), matches);
            std::vector<vector<DMatch > > matches2;
            flannMatcher.knnMatch(imageVector[i].getDescriptors(), imageVector[k].getDescriptors(), matches2, 10);
            // flann::Index flannIndex(imageVector[i].getDescriptors(), flann::KDTreeIndexParams());

            // float sum = 0.0f;
            // for (int idx = 0; idx < 10; ++idx) {
            //     sum += matches2[0][idx].distance;
            // }
            // cout << "Mean       " << sum / 10 << endl;

            // for (int z = 0; z < 10; ++z) {
            //     matches2[z]
            // }
            // float mean = 0.0f;
            // for (int x = 0; x < matches2.size(); ++x) {


            //     // cout << matches2[x][0].distance << endl;
            //     // for (int y = 0; y < matches2[x].size(); ++y) {
            //     //     mean += matches2[x][y].distance;
            //     // }

            //     // mean = mean/10;
            //     // cout <<"mean            " <<mean << endl;

            // }


            // for (int i = 0; i < min(des_image.rows - 1, (int) matches.size()); i++) { //THIS LOOP IS SENSITIVE TO SEGFAULTS
            //     if ((matches[i][0].distance < 0.6 * (matches[i][1].distance)) && ((int) matches[i].size() <= 2 && (int) matches[i].size() > 0)) {
            //         good_matches.push_back(matches[i][0]);
            //     }
            // }



            // std::vector<int> idx;
            // std::vector<float> dist;
            // int K = 5;
            // flannIndex.knnSearch(imageVector[k].getDescriptors(), idx, dist, K, cv::flann::SearchParams(5));

            // for ( int x = 0; x < imageVector[i].getDescriptors().rows; x++ ) {
            //     double dist = matches[x].distance;
            //     if ( dist < min_dist ) min_dist = dist;
            //     if ( dist > max_dist ) max_dist = dist;
            // }

            // double second_min = 1000;

            // for ( int x = 0; x < imageVector[i].getDescriptors().rows; x++ ) {
            //     double dist = matches[x].distance;
            //     if ( dist < second_min && dist > min_dist ) second_min = dist;
            // }


            // double ratio = min_dist / second_min;
            // // printf("-- Max dist : %f \n", max_dist );
            // // printf("-- Min dist : %f \n", min_dist );
            // // printf("-- Second dist : %f \n", second_min );
            // // printf("-- ratio : %f \n", ratio );

            // if (ratio > 0.9) {
            //     cout << "Match " << imageVector[i].getFileName() << "          " << imageVector[k].getFileName() << "    " << ratio << endl;
            // }


            for ( int x = 0; x < imageVector[i].getDescriptors().rows; x++ ) {
                if ( matches[x].distance <= max(2 * min_dist, 0.02) ) {
                    good_matches.push_back( matches[x]);
                }
            }

            matchLookUp[make_pair(imageVector[i].getFileName(), imageVector[k].getFileName())] = good_matches;
        }
    }
    cout << "done." << endl;
    return matchLookUp;
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

std::vector<string> filterImages( vector<Imageobject> imageVector , map<pair<string, string>, vector< DMatch> > matchLookUp) {

    pair<string, string> key1;
    pair<string, string> key2;
    std::vector<string> toStitch;
    std::vector<pair <string, std::vector<string> > > lookUp;
    int TRESH = 50;
    double magDiff = 0.0;



    cout << "Filtering results" << endl;
    for (int i = 0; i < imageVector.size(); ++i) {
        std::vector<string> tmp;
        for (int k = 0; k < imageVector.size(); ++k) {

            key1 = make_pair(imageVector[i].getFileName(), imageVector[k].getFileName());
            key2 = make_pair(imageVector[k].getFileName(), imageVector[i].getFileName());

            // cout <<"PairMatches" << endl;
            // cout << imageVector[i].getFileName() <<"  "<<imageVector[k].getFileName() << endl;
            // cout <<"                " << matchLookUp[key1].size() << endl;
            // cout <<"                " << matchLookUp[key2].size() << endl;


            if (matchLookUp[key1].size() > TRESH  || matchLookUp[key2].size() > TRESH ) {
                magDiff = eDistance(imageVector[i].getMag_data(), imageVector[k].getMag_data());
                // cout << magDiff << endl;

                if (magDiff > 25.0 && magDiff < 60.0) {
                    tmp.push_back(imageVector[k].getFileName());
                    toStitch.push_back(imageVector[i].getFileName());
                    toStitch.push_back(imageVector[k].getFileName());
                }
            }
        }
        lookUp.push_back(make_pair(imageVector[i].getFileName(), tmp));
    }
    cout << "done." << endl;

    // cout << "lookUp size" << endl;
    // cout << lookUp.size() << endl;
    // for (int x = 0; x < lookUp.size(); ++x) {
    //     cout << lookUp[x].first << endl;
    //     cout << lookUp[x].second.size() << endl;
    //     for (int y = 0; y < lookUp[x].second.size(); ++y) {
    //         cout << "           " << lookUp[x].second[y] << endl;
    //     }
    // }


    //Remove duplicates from the vector
    cout << "Removing duplicates" << endl;
    sort( toStitch.begin(), toStitch.end() );
    toStitch.erase( unique( toStitch.begin(), toStitch.end() ), toStitch.end() );
    cout << "done." << endl;

    return toStitch;

};

std::vector<string> findCandidates(vector<Imageobject> imageVector) {
    double magDiff = 0.0;
    std::map<string, string> tmp;
    for (int i = 0; i < imageVector.size(); ++i) {
        for (int k = 0; k < imageVector.size(); ++k) {
            magDiff = eDistance(imageVector[i].getMag_data(), imageVector[k].getMag_data());

            if (magDiff > 25.0 && magDiff < 70.0) {
                tmp.insert(std::map<string, string>::value_type(imageVector[i].getFileName(), imageVector[k].getFileName()));
            }

        }

    }

    std::vector<string> result;
    std::map<string, string>::iterator it = tmp.begin();
    for (it = tmp.begin(); it != tmp.end(); ++it) {
        result.push_back(it->second);
        std::cout << it->first << " => " << it->second << '\n';
    }

    cout << "Removing duplicates" << endl;
    sort( result.begin(), result.end() );
    result.erase( unique( result.begin(), result.end() ), result.end() );
    cout << "done." << endl;

    for (std::vector<string>::iterator it = result.begin(); it != result.end(); ++it) {
        cout << *it << endl;
    }

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