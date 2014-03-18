#include <pan0stitcher.h>

Pan0Stitcher::Pan0Stitcher(std::vector<Imageobject> *imageVector ,string PATH) : imageVector_(imageVector),
    path_(PATH),
    MINDIST_(270) {};

void Pan0Stitcher::add(int id) {
    int imgID = 0;
    for (std::map<int, int>::iterator it = lookUpMap_.begin(); it != lookUpMap_.end(); ++it ) {
        if (it->second == id)
            imgID = it->first;
    }

    Imageobject current = imageVector_->at(imgID);
    string img1 =  current.getFileName();
    img1 = path_ + img1;
    imagesToStitch_.push_back(imread(img1));
    idsToStitch_.push_back(imgID);

};

void Pan0Stitcher::stitch() {
    std::vector<int> component(num_vertices(*graph_));
    int num = boost::connected_components(*graph_, &component[0]);
    Stitcher stitcher = Stitcher::createDefault(1);
    Mat dst;
    std::vector<int>::iterator it;
    BFSVertexVisitor visitor;
    visitor.setPan0Stitcher(this);
    for (int idx = 0; idx < num; ++idx) {
        it = find(component.begin(), component.end(), idx);

        boost::breadth_first_search(*graph_, boost::vertex(distance(component.begin(), it),
                                    *graph_), boost::visitor(visitor));

        if (checkSequence()) {
            cout << "Stitching...." << endl;
            cout << "Number of iamges:  " << imagesToStitch_.size() << endl;
            printID();
            stitcher.stitch(imagesToStitch_, dst);
            writeImg(idx, dst);
        }
        imagesToStitch_.clear();
        idsToStitch_.clear();
    }
};

void Pan0Stitcher::printID() {
    cout << "Printing IDs" << endl;
    for (int idx = 0; idx < idsToStitch_.size(); ++idx) {
        cout << imageVector_->at(idsToStitch_.at(idx)).getFileName() << endl;
    }
};

void Pan0Stitcher::writeImg(int id, Mat img) {
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    string outfile = to_string(id) + "pano.png";
    imwrite(outfile, img, compression_params);

    cout << "saved file as: " << outfile << endl;
}

Mat Pan0Stitcher::getHomography(int id1, int id2, std::vector<DMatch> good_matches) {
    std::vector<Point2f> firstImage, secondImage;
    for (int i = 0; i < good_matches.size(); ++i) {
        firstImage.push_back( (*imageVector_)[id1].getKeypoints()[good_matches[i].queryIdx].pt);
        secondImage.push_back( (*imageVector_)[id2].getKeypoints()[good_matches[i].trainIdx].pt);
    }
    //Find homography with RANSAC
    Mat H = findHomography( secondImage, firstImage, RANSAC);
    return H;
};

cv::Point2f Pan0Stitcher::convertPoints(cv::Point2f points, int w, int h) {
    //Reverse mapping cylindrical
    float x1, y1, f, x, y, s;
    f = 644.82;
    s = f;
    x = points.x - w / 2;
    y = points.y - h / 2;
    // (image_width_in_pixels * 0.5) / tan(FOV * 0.5 * PI/180)
    // FOV 65 grader diagonalt ==> 55.8 horisontellt
    x1 = f * tan(x / s);
    y1 = f * (y / s) * (1 / cos(x / s));
    cv::Point2f ret(x1 + w / 2, y1 + h / 2);
    return ret;
};

Mat Pan0Stitcher::mapImgToCyl(Mat image) {
    Mat map_x, map_y;
    int height = image.rows;
    int width = image.cols;
    Mat dst = Mat::zeros(height, width, CV_32FC1);
    map_x.create( image.size(), CV_32FC1 );
    map_y.create( image.size(), CV_32FC1 );
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Point2f current_pos(x, y);
            Point2f convPoints = convertPoints(current_pos, height, width);

            map_x.at<float>(y, x) = convPoints.x;
            map_y.at<float>(y, x) = convPoints.y;
        }
    }
    // Remapping with bilinear interpolation
    remap( image, dst, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0) );

    return dst;
};

bool Pan0Stitcher::checkSequence() {
    if (idsToStitch_.size() < 3)
        return false;

    double maxDist = 0.0;
    for (int idx = 0; idx < idsToStitch_.size() - 1; ++idx) {

        std::vector<int> Xvec = imageVector_->at(idsToStitch_[idx]).getMag_data();
        for (int idy = idx + 1; idy < idsToStitch_.size(); ++idy) {
            std::vector<int> Yvec = imageVector_->at(idsToStitch_[idy]).getMag_data();
            double dist = eDistance(Xvec, Yvec);
            if (maxDist < dist) {
                maxDist = dist;
            }
        }
    }

    cout << "maxDist " << maxDist << endl;
    return maxDist > MINDIST_;
};

