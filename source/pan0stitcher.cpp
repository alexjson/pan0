#include </home/alex/xjobb/c++/include/pan0stitcher.h>

Pan0Stitcher::Pan0Stitcher(std::vector<Imageobject> *imageVector) : imageVector_(imageVector) {

};

void Pan0Stitcher::stitch() {

    Mat result;
    Mat tmpResult;
    Mat prev_H;
    Mat M = (Mat_<double>(3, 3) << 1, 0, 100, 0, 1, 25, 0, 0, 1);

    for (std::vector< std::vector <int> >::iterator it = panoIDVec_->begin(); it != panoIDVec_->end(); ++it) {
        if ((*it).size() < 3) {
            printf("To few pictures, not stitching\n");
            continue;
        }
        prev_H = Mat::eye(3, 3, 6);
        // Mat final(Size(img_scene.cols + img_object.cols, img_scene.rows*2),CV_8UC3)
        int numberOfImages = (*it).size();
        // Mat final = Mat::zeros(imageVector_->at(0).getImage().rows * 2,
        //                        imageVector_->at(0).getImage().cols * numberOfImages, 0);
        Mat final = Mat::zeros(imageVector_->at(0).getImage().rows * 1.2,
                               imageVector_->at(0).getImage().cols * 2, 0);

        // Mat refImg = imageVector_->at((*it)[0]).getImage();
        // Mat refMat;
        // //Gör för första bilden i loopen bara. Använd som ref bild
        // warpPerspective(refImg, refMat, M, Size(refImg.cols * 2, refImg.rows * 1.2), INTER_CUBIC);
        // Mat roiRef(final, Rect(0, 0, refMat.cols, refMat.rows));
        // refMat.copyTo(roiRef);

        for (int idx = 0; idx < (*it).size(); ++idx) {

            int current = (*it)[idx];
            Imageobject currentObject = imageVector_->at(current);
            int firstMatchID =  currentObject.getFirstMatchID();
            int secondMatchID = currentObject.getSecondMatchID();

            Mat img1 = currentObject.getImage();

            Mat img2 = imageVector_->at(firstMatchID).getImage();

            Mat H = getHomography(current, firstMatchID, currentObject.getFirstMatches());


            // H = H * prev_H;
            Mat HM = H * M;


            // Mat mask = Mat::zeros(img1.rows,img1.cols, 0);
            // Mat warpedMask;
            // warpPerspective(mask, warpedMask, HM, Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);



            Mat result;
            Mat result2;
            Mat slask;
            Mat dst;
            // warpPerspective(img2, slask, HM, Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);
            // Mat roi(slask, cv::Rect(0, 0, slask.cols, slask.rows));



            warpPerspective(img2, result, HM, Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);
            warpPerspective(img1, result2, M, Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);

            // bitwise_or(result,result2, slask);
            // result2.copyTo(roi, warpedMask);
            // imshow("slask", slask);

            addWeighted(result, 0.5, result2, 0.5, 0.0, dst);

            // warpPerspective(result, dst, M, Size(result.cols, result.rows), INTER_CUBIC);
            imshow("dst", dst);
            // imshow("current", img1);
            // imshow("firstMatch", img2);

            if (secondMatchID != -1) {
                Mat result3;
                Mat test;
                Mat img3 = imageVector_->at(secondMatchID).getImage();
                // Mat roi2(result, Rect(0, 0,  img3.cols, img3.rows));
                Mat H2 = getHomography(current, secondMatchID, currentObject.getSecondMatches());
                // H2 = H2 * prev_H;
                H2 = H2 * M;
                warpPerspective(img3, result3, H2, Size(img3.cols * 2, img3.rows * 1.2), INTER_CUBIC);
                // result3.copyTo(roi2);
                addWeighted(dst, 0.5, result3, 0.5, 0.0, test);
                imshow("test", test);
            }
            // imshow("result", result);

            prev_H = H;
            Mat roiFINAL(final, Rect(0, 0, result.cols, result.rows));

            // // result.copyTo(roiFINAL);
            // Mat dst;
            // double alpha = 0.5;
            // double beta = ( 1.0 - alpha );
            // addWeighted( result, alpha, final, beta, 0.0, dst);
            // namedWindow( "Final", WINDOW_AUTOSIZE );
            // imshow("Final", dst);
            // tmpResult.copyTo(half);
            waitKey(0);
        }
    }

};

Mat Pan0Stitcher::getHomography(int id1, int id2, std::vector<DMatch> good_matches) {
    std::vector<Point2f> firstImage;
    std::vector<Point2f> secondImage;
    std::vector< KeyPoint > keypoints_object, keypoints_scene;

    for (int i = 0; i < good_matches.size(); ++i) {
        firstImage.push_back( (*imageVector_)[id1].getKeypoints()[good_matches[i].queryIdx].pt);
        secondImage.push_back( (*imageVector_)[id2].getKeypoints()[good_matches[i].trainIdx].pt);
    }
    //Find homography with RANSAC
    Mat H = findHomography( secondImage, firstImage, RANSAC);
    return H;
};

void Pan0Stitcher::estimateCameraParams() {
    int current = panoIDVec_->at(0)[0];
    Imageobject currentObject = imageVector_->at(current);
    int firstMatchID =  currentObject.getFirstMatchID();
    int secondMatchID = currentObject.getSecondMatchID();

    Mat img1 = currentObject.getImage();
    Mat img2 = imageVector_->at(firstMatchID).getImage();

    Mat H = getHomography(current, firstMatchID, currentObject.getFirstMatches());

};

cv::Point2f Pan0Stitcher::convertPoints(cv::Point2f points) {
    float x1, y1, f, x, y;
    f = 612.0;
    x = points.x;
    y = points.y;
    // (image_width_in_pixels * 0.5) / tan(FOV * 0.5 * PI/180)
    // FOV 65 grader diagonalt ==> 55.8 horisontellt
    x1 = atan(x / f);
    y1 = y / (sqrt(pow(x, 2) + pow(f, 2)));

    cv::Point2f ret(x1, y1);
    return ret;
};

void Pan0Stitcher::mapImgToCyl(Mat image) {



    // Fel här, implementera om bilinear interpolation
    int height = image.rows;
    int width = image.cols;

    Mat dest_im = Mat::zeros(height, width, CV_8U);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cv::Point2f current_pos(x, y);
            current_pos = convertPoints(current_pos);

            cv::Point2i top_left((int)current_pos.x, (int)current_pos.y); //top left because of integer rounding

            //make sure the point is actually inside the original image
            if (top_left.x < 0 ||
                    top_left.x > width - 2 ||
                    top_left.y < 0 ||
                    top_left.y > height - 2) {
                continue;
            }

            //bilinear interpolation
            float dx = current_pos.x - top_left.x;
            float dy = current_pos.y - top_left.y;

            float weight_tl = (1.0 - dx) * (1.0 - dy);
            float weight_tr = (dx)       * (1.0 - dy);
            float weight_bl = (1.0 - dx) * (dy);
            float weight_br = (dx)       * (dy);

            uchar value =   weight_tl * image.at<uchar>(top_left) +
                            weight_tr * image.at<uchar>(top_left.y, top_left.x + 1) +
                            weight_bl * image.at<uchar>(top_left.y + 1, top_left.x) +
                            weight_br * image.at<uchar>(top_left.y + 1, top_left.x + 1);

            dest_im.at<uchar>(y, x) = value;
        }
    }

    // imshow("im", image);
    // imshow("dest_im", dest_im);
    // waitKey(0);

};