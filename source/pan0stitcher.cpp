#include </home/alex/xjobb/c++/include/pan0stitcher.h>

Pan0Stitcher::Pan0Stitcher(std::vector<Imageobject> *imageVector) : imageVector_(imageVector) {

};

void Pan0Stitcher::stitch() {
    Mat result;
    for (std::vector< std::vector <int> >::iterator it = panoIDVec_->begin(); it != panoIDVec_->end(); ++it) {

        for (int idx = 0; idx < (*it).size(); ++idx) {
            int current = (*it)[idx];
            int firstMatchID =  imageVector_->at(current).getFirstMatchID();
            Mat img1 = imageVector_->at(current).getImage();
            Mat img2 = imageVector_->at(firstMatchID).getImage();

            Mat H = getHomography(current, firstMatchID);
            cv::Mat result;
            warpPerspective(img1, result, H, cv::Size(img1.cols + img2.cols, img1.rows));
            cv::Mat half(result, cv::Rect(0, 0, img2.cols, img2.rows));
            img2.copyTo(half);
            imshow( "Result", result );
            waitKey(0);
        }
    }

};

Mat Pan0Stitcher::getHomography(int id1, int id2) {

    // FEL i homography
    std::vector<Point2f> firstImage;
    std::vector<Point2f> secondImage;
    std::vector<DMatch> BestMatches = (*imageVector_)[id1].getFirstMatches();
    std::vector< KeyPoint > keypoints_object, keypoints_scene;

    for (int i = 0; i < BestMatches.size(); ++i) {
        firstImage.push_back( (*imageVector_)[id1].getKeypoints()[BestMatches[i].queryIdx].pt);
        secondImage.push_back( (*imageVector_)[id2].getKeypoints()[BestMatches[i].trainIdx].pt);
    }
    //Find homography with RANSAC
    Mat H = findHomography( firstImage, secondImage, RANSAC);
    return H;
};
