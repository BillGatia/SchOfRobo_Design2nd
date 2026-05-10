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
    Mat blurred, redMask, greenMask, redOpened, greenOpened;

    // 高斯模糊
    cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);

    // 使用BGR颜色空间进行红色检测
    // 注意：OpenCV的BGR顺序，所以红色是第三个通道
    inRange(blurred, redScalarLow_, redScalarHigh_, redMask);
    // 使用BGR颜色空间进行绿色检测
    inRange(blurred, greenScalarLow_, greenScalarHigh_, greenMask);

    // 开运算去除噪声
    cv::morphologyEx(redMask, redOpened, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(greenMask, greenOpened, cv::MORPH_OPEN, kernel);

    // 统计红色和绿色像素数量
    int redCount = countNonZero(redOpened);
    int greenCount = countNonZero(greenOpened);

    // 显示结果
    imshow("Red Mask", redOpened);
    imshow("Green Mask", greenOpened);

    int *result = new int[2];
    result[0] = redCount;
    result[1] = greenCount;
    return result;
}