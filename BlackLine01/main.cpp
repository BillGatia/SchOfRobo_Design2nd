#include "BlackLine.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    Mat original;
    BlackLine bl(Rect(200, 400, 880, 200));
    VideoCapture cap(0);

    while (1)
    {
        cap >> original;
        resize(original, original, Size(1280, 720));
        rectangle(original, Rect(200, 400, 880, 200), Scalar(0, 255, 0), 2);
        imshow("original", original);

        Mat image = bl.Perprocess(original);
        
        // 使用新的LocateLine03方法（Hough直线检测+智能采样）
        int *result = bl.LocateLine03(image, 5);

        // 释放动态分配的内存
        delete[] result;

        if (waitKey(10) == 'q')
            break;
    }

    // 释放摄像头
    cap.release();
    // 关闭所有窗口
    destroyAllWindows();

    return 0;
}