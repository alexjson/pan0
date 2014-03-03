#include <pan0stitcher.h>

Pan0Stitcher::Pan0Stitcher(std::vector<Imageobject> *imageVector) : imageVector_(imageVector) {
    prev_H = Mat::eye(3, 3, 6);
};


void Pan0Stitcher::add(int id) {

    // BundleAdjusterBase::estimate ? Använd function för bundle adjustment

    //Translation matrix
    Mat T = (Mat_<double>(3, 3) << 1, 0, 100, 0, 1, 25, 0, 0, 1);

    Mat result;
    Mat result2;
    Mat result3;
    Mat slask;
    Mat dst;


    Imageobject current = imageVector_->at(id);
    int id1 = current.getFirstMatchID();
    int id2 = -1;
    if (current.getSecondMatchID() != -1)
        id2 = current.getSecondMatchID();


    // Mat img1 =  mapImgToCyl(current.getImage());
    // Mat img2 = mapImgToCyl(imageVector_->at(id1).getImage());

    // detail::BundleAdjusterBase ba;


    Mat img1 =  current.getImage();
    Mat img2 = imageVector_->at(id1).getImage();

    Mat H1 = getHomography(id, id1, current.getFirstMatches());
    // H1 = H1 * prev_H;
    warpPerspective(img2, result, (H1 * T), Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);
    warpPerspective(img1, result2, T, Size(img1.cols * 2, img1.rows * 1.2), INTER_CUBIC);

    addWeighted(result, 0.5, result2, 0.5, 0.0, dst);

    imshow("dst", dst);


    // if (id2 != -1) {
    //     Mat test;
    //     Mat img3 = imageVector_->at(id2).getImage();
    //     Mat H2 = getHomography(id, id2, current.getSecondMatches());

    //     warpPerspective(img3, result3, H2, Size(img3.cols * 2, img3.rows * 1.2), INTER_CUBIC);
    //     addWeighted(dst, 0.5, result3, 0.5, 0.0, test);
    //     imshow("dst", test);
    // }

    waitKey(0);


    // prev_H = H1;

    current.setMakred(true);
    imageVector_->at(id1).setMakred(true);
    if (id2 != -1)
        imageVector_->at(id2).setMakred(true);

};

void Pan0Stitcher::stitch() {

    Mat result;
    Mat tmpResult;
    Mat prev_H;
    Mat M = (Mat_<double>(3, 3) << 1, 0, 100, 0, 1, 25, 0, 0, 1);

    std::vector<int> component(num_vertices(*graph_));
    int num = boost::connected_components(*graph_, &component[0]);

    std::vector<int>::size_type i;
    cout << "Total number of components: " << num << endl;
    for (i = 0; i != component.size(); ++i)
        cout << "Vertex " << i << " is in component " << component[i] << endl;
    cout << endl;

    std::vector<int>::iterator it;
    BFSVertexVisitor visitor;
    visitor.setPan0Stitcher(this);
    cout << "breadth_first_search" << "\n";
    for (int idx = 0; idx < num; ++idx) {
        it = find(component.begin(), component.end(), idx);

        cout << "idx " << idx << "it val  " << distance(component.begin(),it)<< endl;
        boost::breadth_first_search(*graph_, boost::vertex(distance(component.begin(),it), *graph_), boost::visitor(visitor));

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

void Pan0Stitcher::GraphTraverse(std::vector<int> G) {


    std::vector<Edge> edgeVec;
    for (std::vector<int>::iterator it = G.begin(); it != G.end(); ++it) {
        int current = *it;
        int first = imageVector_->at(*it).getFirstMatchID();
        edgeVec.push_back(Edge(current, first));
        if (imageVector_->at(*it).getSecondMatchID() != -1) {
            edgeVec.push_back(Edge(current, imageVector_->at(*it).getSecondMatchID()));
        }
    }

    Graph g(edgeVec.begin(), edgeVec.end(), G.size());

    BFSVertexVisitor visitor;
    visitor.setPan0Stitcher(this);
    cout << "breadth_first_search" << "\n";
    boost::breadth_first_search(g, boost::vertex(G.at(0), g), boost::visitor(visitor));

    edgeVec.clear();
    g.clear();

};
