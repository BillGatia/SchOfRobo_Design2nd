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
    Mat redFilteredMask, greenFilteredMask;
    vector<vector<Point>> redContourList, greenContourList;
    vector<vector<Point>> redBigContours, greenBigContours;
    Mat redContours = Mat::zeros(frame.size(), CV_8UC3);
    Mat greenContours = Mat::zeros(frame.size(), CV_8UC3);

    // 高斯模糊
    cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);
    cv::cvtColor(blurred, blurred, cv::COLOR_BGR2HSV);

    // 使用HSV颜色空间进行红色检测
    inRange(blurred, redScalarLow_, redScalarHigh_, redMask);
    // 使用HSV颜色空间进行绿色检测
    inRange(blurred, greenScalarLow_, greenScalarHigh_, greenMask);
    if (greenReflectScalarLow_ != Scalar(0, 0, 0) && greenReflectScalarHigh_ != Scalar(0, 0, 0))
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

    // 按面积过滤，保留大块连通域
    for (const auto &c : redContourList)
    {
        if (contourArea(c) >= minContourArea_)
            redBigContours.push_back(c);
    }
    for (const auto &c : greenContourList)
    {
        if (contourArea(c) >= minContourArea_)
            greenBigContours.push_back(c);
    }

    // 绘制轮廓
    cv::drawContours(redContours, redBigContours, -1, cv::Scalar(0, 0, 255), 2);
    cv::drawContours(greenContours, greenBigContours, -1, cv::Scalar(0, 255, 0), 2);

    // 只统计大块连通域的像素
    redFilteredMask = Mat::zeros(frame.size(), CV_8UC1);
    greenFilteredMask = Mat::zeros(frame.size(), CV_8UC1);
    cv::drawContours(redFilteredMask, redBigContours, -1, cv::Scalar(255), cv::FILLED);
    cv::drawContours(greenFilteredMask, greenBigContours, -1, cv::Scalar(255), cv::FILLED);

    // 统计红色和绿色像素数量
    int redCount = countNonZero(redFilteredMask);
    int greenCount = countNonZero(greenFilteredMask);

    // // 显示结果
    // imshow("Red Mask", redFilteredMask);
    // imshow("Green Mask", greenFilteredMask);
    // imshow("Red Contours", redContours);
    // imshow("Green Contours", greenContours);

    int *result = new int[2];
    result[0] = redCount;
    result[1] = greenCount;
    return result;
}

int *TrafficLight02::ReadCircleFiltered(Mat frame)
{
    Mat blurred, redMask, greenMask, greenReflectMask, redOpened, greenOpened, redClosed, greenClosed;
    Mat redFilteredMask, greenFilteredMask;
    vector<vector<Point>> redContourList, greenContourList;
    vector<vector<Point>> redBigContours, greenBigContours;
    Mat redContours = Mat::zeros(frame.size(), CV_8UC3);
    Mat greenContours = Mat::zeros(frame.size(), CV_8UC3);

    // 高斯模糊
    cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);
    cv::cvtColor(blurred, blurred, cv::COLOR_BGR2HSV);

    // 使用HSV颜色空间进行红色检测
    inRange(blurred, redScalarLow_, redScalarHigh_, redMask);
    // 使用HSV颜色空间进行绿色检测
    inRange(blurred, greenScalarLow_, greenScalarHigh_, greenMask);
    if (greenReflectScalarLow_ != Scalar(0, 0, 0) && greenReflectScalarHigh_ != Scalar(0, 0, 0))
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

    // 按面积与圆形度过滤，保留更像圆的连通域
    for (const auto &c : redContourList)
    {
        double area = contourArea(c);
        if (area < minContourArea_)
            continue;
        double perimeter = arcLength(c, true);
        if (perimeter <= 1e-5)
            continue;
        double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);
        if (circularity >= minCircularity_)
            redBigContours.push_back(c);
    }
    for (const auto &c : greenContourList)
    {
        double area = contourArea(c);
        if (area < minContourArea_)
            continue;
        double perimeter = arcLength(c, true);
        if (perimeter <= 1e-5)
            continue;
        double circularity = 4.0 * CV_PI * area / (perimeter * perimeter);
        if (circularity >= minCircularity_)
            greenBigContours.push_back(c);
    }

    // 绘制轮廓
    cv::drawContours(redContours, redBigContours, -1, cv::Scalar(0, 0, 255), 2);
    cv::drawContours(greenContours, greenBigContours, -1, cv::Scalar(0, 255, 0), 2);

    // 只统计筛选后的连通域像素
    redFilteredMask = Mat::zeros(frame.size(), CV_8UC1);
    greenFilteredMask = Mat::zeros(frame.size(), CV_8UC1);
    cv::drawContours(redFilteredMask, redBigContours, -1, cv::Scalar(255), cv::FILLED);
    cv::drawContours(greenFilteredMask, greenBigContours, -1, cv::Scalar(255), cv::FILLED);

    // 统计红色和绿色像素数量
    int redCount = countNonZero(redFilteredMask);
    int greenCount = countNonZero(greenFilteredMask);

    // // 显示结果
    // imshow("Red Mask", redFilteredMask);
    // imshow("Green Mask", greenFilteredMask);
    // imshow("Red Contours", redContours);
    // imshow("Green Contours", greenContours);

    int *result = new int[2];
    result[0] = redCount;
    result[1] = greenCount;
    return result;
}
