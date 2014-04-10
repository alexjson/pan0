#include <pan0stitcher.h>

Pan0Stitcher::Pan0Stitcher(std::vector<Imageobject> *imageVector , string PATH) : imageVector_(imageVector),
    path_(PATH),
    MINDIST_(50) {
    matcher = DescriptorMatcher::create("BruteForce"); // FlannBased , BruteForce
    detector = FeatureDetector::create("ORB"); 
    // detector->set("nFeatures", 1500);
    extractor = DescriptorExtractor::create("ORB"); // ORB SIFT
};


void Pan0Stitcher::stitch() {

    for (int idx = 0; idx < idVec_.size(); ++idx) {

        idsToStitch_ = idVec_.at(idx);

        cout << "Number of iamges:  " << idsToStitch_.size() << endl;

        if (checkSequence()) {
            // cout << "panorama found " << endl;
            // cout << "Number of iamges:  " << idsToStitch_.size() << endl;

            prepareImages();
            stitching_detailed(imageVector_, idsToStitch_, to_string(idx) + ".jpg");
            generateOutput(idx);

        }
        imagesToStitch_.clear();
        idsToStitch_.clear();
    }
};

void Pan0Stitcher::prepareImages() {
    int top, bottom, left, right;
    for (std::vector<int>::iterator it = idsToStitch_.begin(); it != idsToStitch_.end(); ++it) {

        if ( abs(imageVector_->at(*it).getRollDegrees()) > 45) {

            Mat img =  imageVector_->at(*it).getImage();
            double angle;

            if (imageVector_->at(*it).getRollDegrees() < -180) {
                angle = 90;
                // cout<< "inne i roll" << endl;
                // cout << angle << endl;
               // cout << "Roll  "<< imageVector_->at(*it).getRollDegrees() << endl;
            }else{
                angle = -90;
            }

            double scale = 1;
            Size newSize(img.size().width , img.size().width );

            int T = img.size().width - img.size().height;
            Mat t_mat = (Mat_<double>(2, 3) << 1, 0, T, 0, 1, T);

            Mat borderImg;
            top = (int) (0.25 * img.rows); bottom = (int) (0.25 * img.rows);
            left = (int) (0.25 * img.cols); right = (int) (0.25 * img.cols);
            Scalar value = Scalar( 0, 0, 0 );
            copyMakeBorder( img, borderImg, top, bottom, left, right, BORDER_CONSTANT, value );

            Point center = Point( borderImg.cols / 2, borderImg.rows / 2 );

            /// Get the rotation matrix with the specifications above
            Mat rot_mat = getRotationMatrix2D( center, angle, scale );

            Mat rotate_dst;
            /// Rotate the warped image
            t_mat = rot_mat + t_mat;
            warpAffine( borderImg, rotate_dst, rot_mat, borderImg.size() );

            int Y1, X1;
            X1 = (borderImg.rows - img.cols) / 2;
            Y1 = (borderImg.cols - img.rows) / 2;

            cv::Rect roi = cv::Rect(Y1, X1, img.size().height, img.size().width);
            Mat roiImg = rotate_dst(roi).clone();
            imageVector_->at(*it).setImage(roiImg);

            extractDescriptors(*it);
        }

    }
};

void Pan0Stitcher::extractDescriptors(int id) {

    vector<KeyPoint> keypoints;
    Mat descriptors;

    detector->detect(imageVector_->at(id).getImage(), keypoints);
    imageVector_->at(id).setKeyPoints(keypoints);
    extractor->compute(imageVector_->at(id).getImage(), keypoints, descriptors);
    imageVector_->at(id).setDescriptors(descriptors);
    imageVector_->at(id).setImageFeatures();

};


void Pan0Stitcher::generateOutput(int id) {
    string tmp = "mkdir " + to_string(id);
    system(tmp.c_str());

    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    for (std::vector<int>::iterator it = idsToStitch_.begin(); it != idsToStitch_.end(); ++it) {
        string outfile = to_string(id) + "/" + to_string(*it) + ".png";
        imwrite(outfile, imageVector_->at(*it).getImage(), compression_params);
        cout << "saved file as: " << outfile << endl;
    }
}

//For debugging only
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