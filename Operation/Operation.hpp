#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <opencv2/opencv.hpp>
using namespace cv;

class Operation
{
private:
    Mat frame_;

public:
    Operation();
    Operation(Mat initFrame);
    Mat ReturnFrame() { return frame_; }
    void SetFrame(Mat newFrame) { frame_ = newFrame; }

    void Cut(int x = 50, int y = 50, int width = 200, int height = 200);
    void Resize(int width = 256, int height = 256);
    void Gray();
    void Range(int lowH = 0, int lowS = 0, int lowV = 0, int highH = 180, int highS = 255, int highV = 255);
    void AdaptiveThreshold(int adaptiveMethod = ADAPTIVE_THRESH_GAUSSIAN_C, int thresholdType = THRESH_BINARY, int blockSize = 11, double C = 2);
    void Rectangle(int x = 50, int y = 50, int width = 200, int height = 200, Scalar color = Scalar(0, 255, 0), int thickness = 2);
    void PlaceOnWhiteBackground(int targetWidth = 800, int targetHeight = 600, float scaleRatio = 0.4); // 默认占比40%
    void bitwiseNot();
};

#endif // OPERATION_HPP