#include <opencv2/opencv.hpp>
#include "TrafficLight02.hpp"

int main()
{
    // 打开摄像头
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "can not open camera" << std::endl;
        return -1;
    }

    TrafficLight02 TL02;
    cv::Mat frame;

    while (true)
    {
        cap >> frame;

        int *result = TL02.Read(frame);
        if (result[0] > 100)
            std::cout << "Red Light Detected" << std::endl;
        else if (result[1] > 100)
            std::cout << "Green Light Detected" << std::endl;
        // 显示原始图像
        cv::imshow("Original", frame);

        if (cv::waitKey(1) == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}