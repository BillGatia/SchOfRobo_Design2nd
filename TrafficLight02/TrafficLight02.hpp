#include <opencv2/opencv.hpp>

class TrafficLight02
{
private:
    // // BGR颜色范围
    // int redLow = 150;    // 红色通道下限
    // int redHigh = 255;   // 红色通道上限
    // int greenLow = 0;    // 绿色通道下限
    // int greenHigh = 100; // 绿色通道上限
    // int blueLow = 0;     // 蓝色通道下限
    // int blueHigh = 100;  // 蓝色通道上限

    // BGR颜色范围的Scalar表示
    // BGR颜色范围的Scalar表示
    cv::Scalar redScalarLow_ = cv::Scalar(0, 0, 200);
    cv::Scalar redScalarHigh_ = cv::Scalar(150, 150, 255);
    cv::Scalar greenScalarLow_ = cv::Scalar(150, 200, 150);
    cv::Scalar greenScalarHigh_ = cv::Scalar(210, 255, 250);
    // cv::Scalar redScalarLow_ = cv::Scalar(140, 140, 225);
    // cv::Scalar redScalarHigh_ = cv::Scalar(200, 210, 255);
    // cv::Scalar greenScalarLow_ = cv::Scalar(200, 230, 230);
    // cv::Scalar greenScalarHigh_ = cv::Scalar(240, 255, 255);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));

public:
    TrafficLight02();
    TrafficLight02(cv::Scalar redScalarLow,
                   cv::Scalar redScalarHigh,
                   cv::Scalar greenScalarLow,
                   cv::Scalar greenScalarHigh);

    ~TrafficLight02();
    int *Read(cv::Mat frame);
};