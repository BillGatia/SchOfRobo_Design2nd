#include <opencv2/opencv.hpp>
using namespace cv;
class BlackLine
{
private:
    cv::Mat image_;
    Rect rio_ = Rect(100, 100, 400, 200);
    int x_ = rio_.x;
    int y_ = rio_.y;
    int width_ = rio_.width;
    int height_ = rio_.height;

public:
    BlackLine();
    BlackLine(Rect rio);
    BlackLine(cv::Mat image);
    ~BlackLine();
    Mat Perprocess(Mat image);
    int *LocateLine(Mat image);
    int *LocateLine02(Mat image, int Num_of_points = 10); // 另一种定位方法 多个条形定位出多个中心点 最后连线。结果表示为x1y1x2y2x3y3...
    int *LocateLine03(Mat image, int Num_of_points = 10); // 分段质心法直接求车道中心轨迹
};