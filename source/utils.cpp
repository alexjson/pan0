#include <utils.h>

// const double MAGDIFFUPPER = 45.0;
// const double MAGDIFFLOWER = 21.0;

const double MAGDIFFUPPER = 100.0; // 85 Not in degrees... var = 2.25*Degrees
const double MAGDIFFUPPER2 = 50.0; // Not in degrees... var = 2.25*Degrees
const double MAGDIFFLOWER = 20.0;  // Not in degrees...

const double TILTDIFF = 0.7; // 0.785 = 45 degrees
const double ROLLTHRESH = 0.7; // Kolla bara raotation på första bilden
const int   TIMEDIFF = 480;
const int   TIMEDIFFTRIGGER = 12;


double get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL)) {
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
};
double get_cpu_time() {
    return (double)clock() / CLOCKS_PER_SEC;
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

bool checkTimeDiff(int id1, int id2, std::vector<Imageobject> *imageVector) {
    using namespace boost::posix_time;
    ptime t1 = (*imageVector)[id1].getTime();
    ptime t2 = (*imageVector)[id2].getTime();
    time_duration diff = t2 - t1;

    if ((*imageVector)[id1].getTrigger() == "double-tap")
        return abs(diff.total_seconds()) < TIMEDIFFTRIGGER;

    return abs(diff.total_seconds()) < TIMEDIFF;
};

bool checkMagDiff(int id1, int id2, vector<Imageobject> *imageVector) {
    double magDiff = eDistance((*imageVector)[id1].getMag_data(), (*imageVector)[id2].getMag_data());
    double upperThresh;
    if (abs(imageVector->at(id1).getRoll()) < ROLLTHRESH) {
        upperThresh = MAGDIFFUPPER;
    } else {
        upperThresh = MAGDIFFUPPER2;
    }
    return (magDiff > MAGDIFFLOWER && magDiff < upperThresh);
};
bool checkTiltDiff(int id1, int id2, vector<Imageobject> *imageVector) {
    double tiltDiff = abs((*imageVector)[id1].getTilt() - (*imageVector)[id2].getTilt());
    return tiltDiff < TILTDIFF;
};

bool checkMagDiffMin(int id1, int id2, vector<Imageobject> *imageVector) {
    double magDiff = eDistance((*imageVector)[id1].getMag_data(), (*imageVector)[id2].getMag_data());
    return magDiff > MAGDIFFLOWER;
};

bool checkMagDiffMax(int id1, int id2, vector<Imageobject> *imageVector) {
    double magDiff = eDistance((*imageVector)[id1].getMag_data(), (*imageVector)[id2].getMag_data());
    double thresh;
    double imgRoll = imageVector->at(id1).getRoll();
    if (-ROLLTHRESH < imgRoll && imgRoll < ROLLTHRESH) {
        thresh = MAGDIFFUPPER;
    } else {
        thresh = MAGDIFFUPPER2;
    }
    return magDiff < thresh;
};



bool checkTrigger(int id1, int id2, vector<Imageobject> *imageVector) {
    return (*imageVector)[id1].getTrigger() == (*imageVector)[id2].getTrigger();
};


