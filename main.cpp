#include "./Operation/Operation.hpp"
#include "./BlackLine01/BlackLine.hpp"
#include "./TrafficLight02/TrafficLight02.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "./QRCode/QRcode.hpp"
#include "./Uart/Uart.hpp"

using namespace cv;
using namespace std;

int main()
{
    Operation op01;
    Operation op02;
    // BlackLine bl;
    TrafficLight02 tl;

    VideoCapture cap(1); // window要用1，linux要用2
    Mat frame;
    Mat frame_BL;
    Mat frame_TL;

    // 以下为树莓派二维码和串口部分
    QRcode qrCode01;
    Uart uart01;

    while (true)
    {
        cap >> frame;
        resize(frame, frame, Size(640, 480));

        if (frame.empty())
            break;

        if (qrCode01.QRflag == 0) // 如果QRflag为0，表示需要识别二维码
        {
            // 识别二维码
            string QRresult = qrCode01.Read(frame);
            if (QRresult != "a_")
            {
                uart01.Send(QRresult.c_str());
                cout << QRresult.c_str() << endl;
            }

            //
            qrCode01.QRflag = 0; // 设置QRflag为0，为了可以一直识别二维码调试。
        }

        // op01.SetFrame(frame);
        op02.SetFrame(frame);
        // op01.Cut(0, 200, 640, 200);
        // frame_BL = op01.ReturnFrame();
        op02.Cut(0, 0, 640, 200);
        frame_TL = op02.ReturnFrame();
        // rectangle(frame, Rect(0, 200, 640, 200), Scalar(0, 255, 0), 2);
        rectangle(frame, Rect(0, 0, 640, 200), Scalar(255, 0, 0), 2);

        // 红绿灯检测，逻辑部分（使用 tl 对象，处理帧为 frame_TL）
        int *tl_result = tl.Read(frame_TL);
        if (tl_result != nullptr)
        {
            if (tl_result[0] > 2000)
            {
                cout << "Red Light Detected" << tl_result[0] << endl;
                char bufR[2] = {'R', '\0'};
                const char *tempR = bufR;
                uart01.Send(tempR);
            }
            else if (tl_result[1] > 2000)
            {
                cout << "Green Light Detected" << tl_result[1] << endl;
                char bufG[2] = {'G', '\0'};
                const char *tempG = bufG;
                uart01.Send(tempG);
            }
            else
            {
                cout << "dont Detected" << endl;
                char bufN[2] = {'N', '\0'};
                const char *tempN = bufN;
                uart01.Send(tempN);
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

        if (waitKey(30) == 27) // 按 'q' 或 'ESC' 键退出
            break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}