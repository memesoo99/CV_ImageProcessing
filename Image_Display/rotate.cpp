
//
//  rotate.cpp
//
//  Created by 김지수 on 2021/03/14.
//

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);

int main()
{
    
    Mat input, rotated;
    
    // Read each image
    input = imread("lena.jpg");

    // Check for invalid input
    if (!input.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    
    // original image
    namedWindow("image");
    imshow("image", input);

    rotated = myrotate<Vec3b>(input, 45, "nearest");

    // rotated image
    namedWindow("rotated");
    imshow("rotated", rotated);

    waitKey(0);

    return 0;
}

//45degree rotate to the left
template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt) {
    int row = input.rows;
    int col = input.cols;

    //호의 길이가 반지름과 같게 되는 만큼의 각을 1 라디안이라고 정의합니다.

    float radian = angle * CV_PI / 180; //CV_PI : pi
   
    float sq_row = ceil(row * sin(radian) + col * cos(radian));
    float sq_col = ceil(col * sin(radian) + row * cos(radian));
    Mat output = Mat::zeros(sq_row, sq_col, input.type());
   
    for (int i = 0; i < sq_row; i++) {
        for (int j = 0; j < sq_col; j++) { //x,y는 변환된 그림의 725*725 좌표체계에서 다시 512*512로 변환했을때 범위에 든 좌표들을 골라내는 과정
            float x = (j - sq_col / 2) * cos(radian) - (i - sq_row / 2) * sin(radian) + col / 2;
            float y = (j - sq_col / 2) * sin(radian) + (i - sq_row / 2) * cos(radian) + row / 2;
            
            //i,j는 정수체계이고, x,y는 실수로 나타남. 보간법 필요
            if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))) {
                if (!strcmp(opt, "nearest")) {
                    
                    output.at<Vec3b>(i,j) = input.at<Vec3b>(round(y),round(x));
                    //nearest 보간법은 좌표체계중 가장 가까운 점의 pixel값을 가져오기 때문에 x,y를 반올림한 값의 pixel을 가져온다
                }
                else if (!strcmp(opt, "bilinear")) {
                   
                    float lam1 = x - floor(x); //x좌표와 내림한 x좌표의 차이 -> lambda1
                    float lam2 = y - floor(y); //y좌표와 내림한 y좌표의 차이 -> lambda2
                    
                    Vec<unsigned char, 3> A = input.at<Vec3b>(floor(y),floor(x));
                    Vec<unsigned char, 3> B = input.at<Vec3b>(ceil(y),floor(x));
                    Vec<unsigned char, 3> C = input.at<Vec3b>(ceil(y),ceil(x));
                    Vec<unsigned char, 3> D = input.at<Vec3b>(floor(y),ceil(x));
                    
                    output.at<Vec3b>(i,j) = (A*(1-lam2) +B*(lam2))*(1-lam1)+(D*(1-lam2)+C*(lam2))*(lam1);
                    
                   //bilinear 보간법은 (x,y) = P 라고 가정했을 떄, 왼쪽위부터 시계방향으로 정수로 이루어진 좌표 A,B,C,D좌표 네개를 찾는다.
                   //계산 식: {A*(1-lambda2) + B*(lambda2)}*(1-lambda1) + {C*lambda2 + D*(1-lambda2)}*(lambda1)
                }
            }
        }
    }

    return output;
}
