#include <utils.h>

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