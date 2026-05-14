#include <opencv2/opencv.hpp>

class TrafficLight02
{
private:
    // // HSV颜色范围
    // int redLow = 150;    // 红色通道下限
    // int redHigh = 255;   // 红色通道上限
    // int greenLow = 0;    // 绿色通道下限
    // int greenHigh = 100; // 绿色通道上限
    // int blueLow = 0;     // 蓝色通道下限
    // int blueHigh = 100;  // 蓝色通道上限

    // HSV颜色范围的Scalar表示
    cv::Scalar redScalarLow_ = cv::Scalar(0, 120, 120);
    cv::Scalar redScalarHigh_ = cv::Scalar(10, 255, 255);

    cv::Scalar greenScalarLow_ = cv::Scalar(35, 40, 80);
    cv::Scalar greenScalarHigh_ = cv::Scalar(85, 255, 255);
    // cv::Scalar greenReflectScalarLow_ = cv::Scalar(25, 0, 200);
    // cv::Scalar greenReflectScalarHigh_ = cv::Scalar(95, 20, 255);
    cv::Scalar greenReflectScalarLow_ = cv::Scalar(0, 0, 0);
    cv::Scalar greenReflectScalarHigh_ = cv::Scalar(0, 0, 0);

    // cv::Scalar redScalarLow_ = cv::Scalar(140, 140, 225);
    // cv::Scalar redScalarHigh_ = cv::Scalar(200, 210, 255);
    // cv::Scalar greenScalarLow_ = cv::Scalar(200, 230, 230);
    // cv::Scalar greenScalarHigh_ = cv::Scalar(240, 255, 255);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

public:
    TrafficLight02();
    TrafficLight02(cv::Scalar redScalarLow,
                   cv::Scalar redScalarHigh,
                   cv::Scalar greenScalarLow,
                   cv::Scalar greenScalarHigh);

    ~TrafficLight02();
    int *Read(cv::Mat frame);
};