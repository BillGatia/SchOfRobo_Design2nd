#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include "QRcode.hpp"
#include <string>
using namespace cv;
using namespace std;

QRcode::QRcode() : cv::QRCodeDetector()
{
}

string QRcode::Read(cv::Mat AimFrame)
{
    frame_ = AimFrame;
    // 检测并解码二维码
    string data = detectAndDecode(frame_); // 输出解码结果

    if (data.length() > 0)
    {
        data_ = data;
        QRflag = 1;
        std::cout << "Decoded QR Code data: " << data << std::endl;
    }
    else
    {
    }

    return "a_" + data;
}