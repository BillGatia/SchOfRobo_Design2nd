#include "./Operation/Operation.hpp"
#include "./BlackLine01/BlackLine.hpp"
#include "./TrafficLight02/TrafficLight02.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <deque>
#include "./QRCode/QRcode.hpp"
#include "./Uart/Uart.hpp"
#include <thread> //用于延时
#include <chrono>
#include <cstdlib>

using namespace cv;
using namespace std;

int main()
{
    Operation op01;
    Operation op02;
    // BlackLine bl;
    TrafficLight02 tl;

    // 可调参数：连通域最小面积
    tl.minContourArea_ = 500.0;
    // 可调参数：圆形度阈值
    tl.minCircularity_ = 0.5;

    // 可调参数：像素阈值与多数投票窗口
    const int kRedPixelThreshold = 2000;
    const int kGreenPixelThreshold = 1800;
    const int kVoteWindowSize = 1; // 3 或 5
    const int kVoteMinCount = 1;   // 多数阈值，窗口为5时建议3

    std::deque<int> voteHistory; // 0: none, 1: red, 2: green

    VideoCapture cap(0); // window要用1，linux要用2
    Mat frame;
    Mat frame_BL;
    Mat frame_TL;

    // 以下为树莓派二维码和串口部分
    QRcode qrCode01;
    int QRflag_main = 1;      // 1代表扫描完后播放get，2代表扫描完后播放put
    char QRLeftorRight = '0'; // 1代表左边，2代表右边
    Uart uart01;

    while (true)
    {
        cap >> frame;
        frame = frame(Rect(120, 0, 400, 300)); // 裁剪为640x200的区域
        resize(frame, frame, Size(640, 480));

        if (frame.empty())
            break;
        // imshow("Frame", frame);
        // waitKey(1);

        if (qrCode01.QRflag == 0) // 如果QRflag为0，表示需要识别二维码
        {
            // 识别二维码
            string QRresult = qrCode01.Read(frame);
            // std::string tempR = QRresult;

            if (!QRresult.empty())
            {
                QRresult = "a_" + QRresult + QRresult;

                QRLeftorRight = QRresult[3];

                const char *result = QRresult.c_str();
                uart01.Send(result);
                cout << result << endl;

                if (QRresult[2] == '1' && QRflag_main == 1) // 物体一抓取
                {
                    system("aplay -D hw:2,0 ../audios/item01_in.wav");
                    QRflag_main = 2;
                }
                else if (QRresult[2] == '2' && QRflag_main == 1) // 物体二抓取
                {
                    system("aplay -D hw:2,0 ../audios/item02_in.wav");
                    QRflag_main = 2;
                }
                else if (QRresult[3] == '1' && QRflag_main == 2) // 位置一（左）放置
                {
                    system("aplay -D hw:2,0 ../audios/item01_out.wav");
                    QRflag_main = 1;
                }
                else if (QRresult[3] == '2' && QRflag_main == 2) // 位置二（右）放置
                {
                    system("aplay -D hw:2,0 ../audios/item02_out.wav");
                    QRflag_main = 1;
                }
                // system("aplay -D hw:2,0 ../audios/first_get.wav");
            }

            //
            qrCode01.QRflag = 0; // 设置QRflag为0，为了可以一直识别二维码调试。
        }

        // op01.SetFrame(frame);
        op02.SetFrame(frame);
        // op01.Cut(0, 200, 640, 200);
        // frame_BL = op01.ReturnFrame();
        // op02.Cut(0, 0, 640, 200);
        frame_TL = op02.ReturnFrame();
        // rectangle(frame, Rect(0, 200, 640, 200), Scalar(0, 255, 0), 2);
        // rectangle(frame, Rect(0, 0, 640, 200), Scalar(255, 0, 0), 2);

        // 红绿灯检测，逻辑部分（使用 tl 对象，处理帧为 frame_TL）
        int *tl_result = tl.ReadCircleFiltered(frame_TL);
        if (tl_result != nullptr)
        {
            int state = 0;
            if (tl_result[0] > kRedPixelThreshold)
                state = 1;
            else if (tl_result[1] > kGreenPixelThreshold)
                state = 2;

            voteHistory.push_back(state);
            if (static_cast<int>(voteHistory.size()) > kVoteWindowSize)
                voteHistory.pop_front();

            int redVotes = 0, greenVotes = 0;
            for (int s : voteHistory)
            {
                if (s == 1)
                    redVotes++;
                else if (s == 2)
                    greenVotes++;
            }
            if (redVotes >= kVoteMinCount)
            {
                cout << "Red Light Detected " << tl_result[0] << endl;
                char bufR[] = "b_RRRR";
                const char *tempR = bufR;
                // uart01.Send(tempR);
            }
            else if (greenVotes >= kVoteMinCount)
            {
                cout << "Green Light Detected " << tl_result[1] << endl;
                char bufG[] = "b_GGGG";
                const char *tempG = bufG;
                uart01.Send(tempG);

                std::this_thread::sleep_for(std::chrono::milliseconds(4000));

                if (QRLeftorRight == '1' && QRflag_main == 2) // 现在在位置一（左边）
                {
                    system("aplay -D hw:2,0 ../audios/place01.wav");
                }
                else if (QRLeftorRight == '2' && QRflag_main == 2) // 现在在位置二（右边）
                {
                    system("aplay -D hw:2,0 ../audios/place02.wav");
                }
            }
            else
            {
                cout << "dont Detected  " << tl_result[0] << " || " << tl_result[1] << endl;
                char bufN[] = "b_NNNN";
                const char *tempN = bufN;
                // uart01.Send(tempN);
            }

            delete[] tl_result;
        }
        // 注意：该模块已经在cmake里面删除，记得要加回来。
        //  // 黑线检测，逻辑部分（使用 bl 对象，处理帧为 frame_BL）
        //  frame_BL = bl.Perprocess(frame_BL);
        //  int *bl_result = bl.LocateLine03(frame_BL, 5);
        //  if (bl_result != nullptr)
        //      delete[] bl_result;

        // imshow("Frame", frame);

        // if (waitKey(30) == 27) // 按 'q' 或 'ESC' 键退出
        //     break;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
