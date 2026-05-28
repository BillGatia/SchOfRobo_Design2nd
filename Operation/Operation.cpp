#include "Operation.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Operation::Operation()
{
    frame_ = Mat::zeros(256, 256, CV_8UC3);
}

Operation::Operation(Mat initFrame)
{
    frame_ = initFrame;
}

void Operation::Cut(int x, int y, int width, int height)
{
    // 这里是裁剪操作的示例
    Rect roi(x, y, width, height); // 定义一个感兴趣区域
    frame_ = frame_(roi).clone();  // 裁剪图像
}

void Operation::Resize(int width, int height)
{
    // 这里是调整图像大小的示例
    resize(frame_, frame_, Size(width, height)); // 调整图像大小为 width x height
}
void Operation::Gray()
{
    // 这里是将图像转换为灰度图的示例
    cvtColor(frame_, frame_, COLOR_BGR2GRAY); // 转换为灰度图
}

void Operation::Range(int lowH, int lowS, int lowV, int highH, int highS, int highV)
{
    // 这里是一个示例，使用inRange函数进行颜色范围过滤
    Mat mask;
    inRange(frame_, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), mask); // 创建一个掩膜，过滤掉所有颜色
    frame_ = mask;                                                                // 将结果赋值回frame_                                            // 显示过滤后的图像
}

void Operation::AdaptiveThreshold(int adaptiveMethod, int thresholdType, int blockSize, double C)
{
    // 首先确保图像是灰度图，如果不是则转换
    if (frame_.channels() > 1)
    {
        cvtColor(frame_, frame_, COLOR_BGR2GRAY);
    }

    // 应用高斯模糊减少噪声
    GaussianBlur(frame_, frame_, Size(5, 5), 0);

    // 应用自适应二值化
    adaptiveThreshold(frame_, frame_, 255, adaptiveMethod, thresholdType, blockSize, C);

    // 应用形态学操作：先腐蚀后膨胀，去除细小噪声
    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));
    erode(frame_, frame_, kernel);
    dilate(frame_, frame_, kernel);
}

void Operation::Rectangle(int x, int y, int width, int height, Scalar color, int thickness)
{
    // 这里是一个示例，在图像上绘制一个矩形
    rectangle(frame_, Point(x, y), Point(x + width, y + height), color, thickness); // 在图像上绘制一个矩形
}

void Operation::PlaceOnWhiteBackground(int targetWidth, int targetHeight, float scaleRatio)
{
    // 创建白色背景
    Mat whiteBackground = Mat::zeros(targetHeight, targetWidth, CV_8UC3);
    whiteBackground = Scalar(255, 255, 255); // 设置为白色

    // 计算调整后图片的大小
    int imgWidth = static_cast<int>(targetWidth * scaleRatio);
    int imgHeight = static_cast<int>(targetHeight * scaleRatio);

    // 调整原始图片大小
    Mat resizedImg;
    resize(frame_, resizedImg, Size(imgWidth, imgHeight));

    // 确保resizedImg是3通道彩色图
    if (resizedImg.channels() == 1)
    {
        cvtColor(resizedImg, resizedImg, COLOR_GRAY2BGR);
    }

    // 计算放置位置（居中）
    int x = (targetWidth - imgWidth) / 2;
    int y = (targetHeight - imgHeight) / 2;

    // 将调整后的图片复制到白色背景的中间
    resizedImg.copyTo(whiteBackground(Rect(x, y, imgWidth, imgHeight)));

    // 更新frame_
    frame_ = whiteBackground;
}

void Operation::bitwiseNot()
{
    // 这里是一个示例，使用bitwise_not函数进行图像取反
    bitwise_not(frame_, frame_); // 对输入图像进行取反，并将结果赋值回frame_

    // 应用形态学操作：先腐蚀后膨胀，去除细小噪声
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(frame_, frame_, kernel);
    dilate(frame_, frame_, kernel);

    convertScaleAbs(frame_, frame_, 4.0, 100);
}