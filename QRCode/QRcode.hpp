#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>

class QRcode : public cv::QRCodeDetector
{
private:
    cv::Mat frame_;
    std::string data_;

public:
    int QRflag = 0; // 用于记录是否还要继续扫描二维码，0为扫，1为不扫

    QRcode();
    void SetFrame(cv::Mat newFrame) { frame_ = newFrame; }
    std::string Read(cv::Mat AimFrame);
    std::string GetData() { return data_; }
};