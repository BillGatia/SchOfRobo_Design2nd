#include "TrafficLight02.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

TrafficLight02::TrafficLight02()
{
}

TrafficLight02::TrafficLight02(cv::Scalar redScalarLow, cv::Scalar redScalarHigh, cv::Scalar greenScalarLow, cv::Scalar greenScalarHigh)
{
    redScalarLow_ = redScalarLow;
    redScalarHigh_ = redScalarHigh;
    greenScalarLow_ = greenScalarLow;
    greenScalarHigh_ = greenScalarHigh;
}

TrafficLight02::~TrafficLight02()
{
}

int *TrafficLight02::Read(Mat frame)
{
    Mat blurred, redMask, greenMask, greenReflectMask, redOpened, greenOpened, redClosed, greenClosed;
    vector<vector<Point>> redContourList, greenContourList;
    Mat redContours = Mat::zeros(frame.size(), CV_8UC3);
    Mat greenContours = Mat::zeros(frame.size(), CV_8UC3);

    // 高斯模糊
    cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);
    cv::cvtColor(blurred, blurred, cv::COLOR_BGR2HSV);

    // 使用HSV颜色空间进行红色检测
    inRange(blurred, redScalarLow_, redScalarHigh_, redMask);
    // 使用HSV颜色空间进行绿色检测
    inRange(blurred, greenScalarLow_, greenScalarHigh_, greenMask);
    if(greenReflectScalarLow_ != Scalar(0, 0, 0) && greenReflectScalarHigh_ != Scalar(0, 0, 0))
    {
        inRange(blurred, greenReflectScalarLow_, greenReflectScalarHigh_, greenReflectMask);
        bitwise_or(greenMask, greenReflectMask, greenMask);
    }

    // 开运算去除噪声
    cv::morphologyEx(redMask, redOpened, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(greenMask, greenOpened, cv::MORPH_OPEN, kernel);

    // 闭运算去补小孔
    cv::morphologyEx(redOpened, redClosed, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(greenOpened, greenClosed, cv::MORPH_CLOSE, kernel);

    // 从闭运算结果中提取轮廓
    cv::findContours(redClosed.clone(), redContourList, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(greenClosed.clone(), greenContourList, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 绘制轮廓
    cv::drawContours(redContours, redContourList, -1, cv::Scalar(0, 0, 255), 2);
    cv::drawContours(greenContours, greenContourList, -1, cv::Scalar(0, 255, 0), 2);

    // 统计红色和绿色像素数量
    int redCount = countNonZero(redClosed);
    int greenCount = countNonZero(greenClosed);

    // 显示结果
    imshow("Red Mask", redClosed);
    imshow("Green Mask", greenClosed);
    imshow("Red Contours", redContours);
    imshow("Green Contours", greenContours);

    int *result = new int[2];
    result[0] = redCount;
    result[1] = greenCount;
    return result;
}