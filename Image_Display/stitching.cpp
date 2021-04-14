// stitching.cpp : Defines the entry point for the console application.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

template <typename T>
Mat cal_affine(int ptl_x[], int ptl_y[], int ptr_x[], int ptr_y[], int number_of_points);

void blend_stitching(const Mat I1, const Mat I2, Mat &I_f, int diff_x, int diff_y, float alpha);

int main() {
    Mat I1, I2;

    // Read each image
    I1 = imread("Img01.jpg");
    I2 = imread("Img02.jpg");

    I1.convertTo(I1, CV_32FC3, 1.0 / 255);
    I2.convertTo(I2, CV_32FC3, 1.0 / 255);
    
    // corresponding pixels
    int ptl_x[28] = { 509, 558, 605, 649, 680, 689, 705, 730, 734, 768, 795, 802, 818, 837, 877, 889, 894, 902, 917, 924, 930, 948, 964, 969, 980, 988, 994, 998 };
    int ptl_y[28] = { 528, 597, 581, 520, 526, 581, 587, 496, 506, 500, 342, 558, 499, 642, 474, 456, 451, 475, 530, 381, 472, 475, 426, 539, 329, 341, 492, 511 };
    int ptr_x[28] = { 45, 89, 142, 194, 226, 230, 246, 279, 281, 314, 352, 345, 365, 372, 421, 434, 439, 446, 456, 472, 471, 488, 506, 503, 527, 532, 528, 531 };
    int ptr_y[28] = { 488, 561, 544, 482, 490, 546, 552, 462, 471, 467, 313, 526, 468, 607, 445, 429, 424, 447, 500, 358, 446, 449, 403, 510, 312, 324, 466, 484 };

    // Check for invalid input
    if (!I1.data || !I2.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // height(row), width(col) of each image
    const float I1_row = I1.rows;
    const float I1_col = I1.cols;
    const float I2_row = I2.rows;
    const float I2_col = I2.cols;

    // calculate affine Matrix A12, A21
    Mat A12 = cal_affine<float>(ptl_x, ptl_y, ptr_x, ptr_y, 28);
    Mat A21 = cal_affine<float>(ptr_x, ptr_y, ptl_x, ptl_y, 28);

    // compute corners (p1, p2, p3, p4)
    // p1: (0,0)
    // p2: (row, 0)
    // p3: (row, col)
    // p4: (0, col)
    Point2f p1(A21.at<float>(0) * 0 + A21.at<float>(1) * 0 + A21.at<float>(2), A21.at<float>(3) * 0 + A21.at<float>(4) * 0 + A21.at<float>(5));
    Point2f p2(A21.at<float>(0) * 0 + A21.at<float>(1) * I2_row + A21.at<float>(2), A21.at<float>(3) * 0 + A21.at<float>(4) * I2_row + A21.at<float>(5));
    Point2f p3(A21.at<float>(0) * I2_col + A21.at<float>(1) * I2_row + A21.at<float>(2), A21.at<float>(3) * I2_col + A21.at<float>(4) * I2_row + A21.at<float>(5));
    Point2f p4(A21.at<float>(0) * I2_col + A21.at<float>(1) * 0 + A21.at<float>(2), A21.at<float>(3) * I2_col + A21.at<float>(4) * 0 + A21.at<float>(5));

    // compute boundary for merged image(I_f)
    // bound_u <= 0
    // bound_b >= I1_row-1
    // bound_l <= 0
    // bound_b >= I1_col-1
    int bound_u = (int)round(min(0.0f, min(p1.y, p4.y)));
    int bound_b = (int)round(max(I1_row-1, max(p2.y, p3.y)));
    int bound_l = (int)round(min(0.0f, min(p1.x, p2.x)));
    int bound_r = (int)round(max(I1_col-1, max(p3.x, p4.x)));
    
    // initialize merged image
    Mat I_f(bound_b - bound_u + 1, bound_r - bound_l + 1, CV_32FC3, Scalar(0));

    // inverse warping with bilinear interplolation
    for (int i = bound_u; i <= bound_b; i++) {
        for (int j = bound_l; j <= bound_r; j++) {
            float x = A12.at<float>(0) * j + A12.at<float>(1) * i + A12.at<float>(2) - bound_l;
            float y = A12.at<float>(3) * j + A12.at<float>(4) * i + A12.at<float>(5) - bound_u;

            float y1 = floor(y);
            float y2 = ceil(y);
            float x1 = floor(x);
            float x2 = ceil(x);

            float mu = y - y1;
            float lambda = x - x1;

            if (x1 >= 0 && x2 < I2_col && y1 >= 0 && y2 < I2_row)
                I_f.at<Vec3f>(i - bound_u, j - bound_l) = lambda * (mu * I2.at<Vec3f>(y2, x2) + (1 - mu) * I2.at<Vec3f>(y1, x2)) +
                                                          (1 - lambda) *(mu * I2.at<Vec3f>(y2, x1) + (1 - mu) * I2.at<Vec3f>(y1, x1));
        }
    }

    // image stitching with blend
    blend_stitching(I1, I2, I_f, bound_l, bound_u, 0.5);
    
    namedWindow("Left Image");
    imshow("Left Image", I1);

    namedWindow("Right Image");
    imshow("Right Image", I2);

    namedWindow("result");
    imshow("result", I_f);

    I_f.convertTo(I_f, CV_8UC3, 255.0);
    imwrite("result.png", I_f);

    waitKey(0);

    return 0;
}

template <typename T>
Mat cal_affine(int ptl_x[], int ptl_y[], int ptr_x[], int ptr_y[], int number_of_points) {

    Mat M(2 * number_of_points, 6, CV_32F, Scalar(0));
    Mat b(2 * number_of_points, 1, CV_32F);

    Mat M_trans, temp, affineM;

    // initialize matrix
    for (int i = 0; i < number_of_points; i++) {
        M.at<T>(2 * i, 0) = ptl_x[i];        M.at<T>(2 * i, 1) = ptl_y[i];        M.at<T>(2 * i, 2) = 1;
        M.at<T>(2 * i + 1, 3) = ptl_x[i];        M.at<T>(2 * i + 1, 4) = ptl_y[i];        M.at<T>(2 * i + 1, 5) = 1;
        b.at<T>(2 * i) = ptr_x[i];        b.at<T>(2 * i + 1) = ptr_y[i];
    }

    // (M^T * M)^(−1) * M^T * b ( * : Matrix multiplication)
    transpose(M, M_trans);
    invert(M_trans * M, temp);
    affineM = temp * M_trans * b;
    std::cout << affineM <<std::endl;
    return affineM;
}

void blend_stitching(const Mat I1, const Mat I2, Mat &I_f, int bound_l, int bound_u, float alpha) {

    int col = I_f.cols;
    int row = I_f.rows;

    // I2 is already in I_f by inverse warping
    for (int i = 0; i < I1.rows; i++) {
        for (int j = 0; j < I1.cols; j++) {
            bool cond_I2 = I_f.at<Vec3f>(i - bound_u, j - bound_l) != Vec3f(0, 0, 0) ? true : false;

            if (cond_I2)
                I_f.at<Vec3f>(i - bound_u, j - bound_l) = alpha * I1.at<Vec3f>(i, j) + (1 - alpha) * I_f.at<Vec3f>(i - bound_u, j - bound_l);
            else
                I_f.at<Vec3f>(i - bound_u, j - bound_l) = I1.at<Vec3f>(i, j);

        }
    }
}
