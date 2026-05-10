#include "BlackLine.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

BlackLine::BlackLine()
{
}

BlackLine::BlackLine(Rect rio)
{
    rio_ = rio;
}

BlackLine::BlackLine(cv::Mat image)
{
    image_ = image;
}
BlackLine::~BlackLine()
{
}

Mat BlackLine::Perprocess(Mat image)
{
    Mat rioimage, blurred, gray, binary;

    // 确保rio_是有效的
    if (rio_.width <= 0 || rio_.height <= 0 ||
        rio_.x < 0 || rio_.y < 0 ||
        rio_.x + rio_.width > image.cols ||
        rio_.y + rio_.height > image.rows)
    {
        // 如果rio_无效，使用默认ROI，基于图像大小动态计算
        std::cout << "rio_ is invalid, use default value" << std::endl;
        int default_width = std::min(400, image.cols - 100);
        int default_height = std::min(200, image.rows - 100);
        if (default_width <= 0 || default_height <= 0)
        {
            // 如果图像太小，返回空的二值图像
            std::cout << "error: image is too small, can not process it" << std::endl;
            return Mat();
        }
        rio_ = Rect(50, image.rows - default_height - 50, default_width, default_height);
    }

    // 真正的预处理部分//
    try
    {
        rioimage = image(rio_).clone();
        cvtColor(rioimage, gray, COLOR_BGR2GRAY);
        blur(gray, blurred, Size(3, 3));
        threshold(blurred, binary, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
    }
    catch (const cv::Exception &e)
    {
        std::cout << "error: " << e.what() << std::endl;
        return Mat();
    }

    return binary;
}

int *BlackLine::LocateLine(Mat image)
{
    Mat binary = image;
    int count = countNonZero(binary);
    int sum_x = 0, sum_y = 0;

    // 检查count是否为0，避免除以零
    if (count == 0)
    {
        static int result[2] = {0, 0};
        return result;
    }

    for (int y = 0; y < binary.rows; y++)
    {
        for (int x = 0; x < binary.cols; x++)
        {
            if (binary.at<uchar>(y, x) == 255)
            {
                sum_x += x;
                sum_y += y;
            }
        }
    }

    int center_x = sum_x / count;
    int center_y = sum_y / count;

    static int result[2] = {0, 0};
    result[0] = center_x;
    result[1] = center_y;
    return result;
}

int *BlackLine::LocateLine02(Mat image, int Num_of_points)
{
    // 初始化过程变量
    const int size = 2 * Num_of_points;
    int *result = new int[size];
    Mat binary = image;

    // 使用binary图像的实际高度计算分段高度
    int segment_height = binary.rows / (Num_of_points + 1);

    for (int i = 0; i < Num_of_points; i++)
    {
        // 计算当前分段的区域
        int tempY = i * segment_height;
        int segment_end_y = tempY + segment_height;

        // 确保不越界
        if (segment_end_y > binary.rows)
        {
            segment_end_y = binary.rows;
        }

        int count = 0;
        int sum_x = 0, sum_y = 0;

        // 遍历当前分段的所有像素
        for (int y = tempY; y < segment_end_y; y++)
        {
            for (int x = 0; x < binary.cols; x++)
            {
                if (binary.at<uchar>(y, x) == 255)
                {
                    sum_x += x;
                    sum_y += y;
                    count++;
                }
            }
        }

        // 检查count是否为0，避免除以零
        if (count == 0)
        {
            result[2 * i] = binary.cols / 2;
            result[2 * i + 1] = tempY + (segment_end_y - tempY) / 2;
            continue;
        }

        int center_x = sum_x / count;
        int center_y = sum_y / count;

        result[2 * i] = center_x;
        result[2 * i + 1] = center_y;
    }

    // 显示结果
    try
    {
        Mat binary_color;
        cvtColor(binary, binary_color, COLOR_GRAY2BGR);
        for (int i = 0; i < Num_of_points; i++)
        {
            circle(binary_color, Point(result[2 * i], result[2 * i + 1]), 5, Scalar(0, 255, 0), -1);
        }
        imshow("LocateLine02 Result", binary_color);
    }
    catch (const cv::Exception &e)
    {
        std::cout << "显示结果时出错：" << e.what() << std::endl;
    }

    return result;
}

// ============ LocateLine03: 分段质心法直接求车道中心轨迹 ============
int *BlackLine::LocateLine03(Mat image, int Num_of_points)
{
    const int size = 2 * Num_of_points;
    int *result = new int[size];
    Mat binary = image;

    if (binary.empty() || binary.type() != CV_8UC1)
    {
        for (int i = 0; i < Num_of_points; i++)
        {
            result[2 * i] = binary.cols / 2;
            result[2 * i + 1] = i * max(1, binary.rows / Num_of_points) + max(1, binary.rows / Num_of_points) / 2;
        }
        return result;
    }

    int stripeHeight = max(1, binary.rows / Num_of_points);
    int prev_center_x = binary.cols / 2;

    for (int i = 0; i < Num_of_points; i++)
    {
        int startY = i * stripeHeight;
        int endY = (i == Num_of_points - 1) ? binary.rows : startY + stripeHeight;
        endY = min(endY, binary.rows);

        Mat stripe = binary(Range(startY, endY), Range(0, binary.cols));
        Moments m = moments(stripe, true);

        int center_x = prev_center_x;
        if (m.m00 > 1e-5)
        {
            center_x = static_cast<int>(m.m10 / m.m00 + 0.5);
            prev_center_x = center_x;
        }
        else
        {
            int left = binary.cols;
            int right = -1;
            for (int y = startY; y < endY; ++y)
            {
                const uchar *row = binary.ptr<uchar>(y);
                int row_left = binary.cols;
                int row_right = -1;
                for (int x = 0; x < binary.cols; ++x)
                {
                    if (row[x] == 255)
                    {
                        row_left = x;
                        break;
                    }
                }
                for (int x = binary.cols - 1; x >= 0; --x)
                {
                    if (row[x] == 255)
                    {
                        row_right = x;
                        break;
                    }
                }
                if (row_right >= 0)
                {
                    left = min(left, row_left);
                    right = max(right, row_right);
                }
            }
            if (right >= 0)
            {
                center_x = (left + right) / 2;
                prev_center_x = center_x;
            }
        }

        center_x = max(0, min(center_x, binary.cols - 1));
        int center_y = startY + (endY - startY) / 2;

        result[2 * i] = center_x;
        result[2 * i + 1] = center_y;
    }

    try
    {
        Mat binary_color;
        cvtColor(binary, binary_color, COLOR_GRAY2BGR);
        for (int i = 0; i < Num_of_points; i++)
        {
            circle(binary_color, Point(result[2 * i], result[2 * i + 1]), 5, Scalar(0, 255, 0), -1);
            if (i > 0)
            {
                line(binary_color,
                     Point(result[2 * i - 2], result[2 * i - 1]),
                     Point(result[2 * i], result[2 * i + 1]),
                     Scalar(255, 0, 0), 2);
            }
        }
        imshow("LocateLine03 Result", binary_color);
    }
    catch (const cv::Exception &e)
    {
        std::cout << "LocateLine03 draw error:" << e.what() << std::endl;
    }

    return result;
}