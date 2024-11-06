#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
    string src = "nvarguscamerasrc sensor-id=0 ! \
        video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
        format=(string)NV12, framerate=(fraction)30/1 ! \
        nvvidconv flip-method=0 ! video/x-raw, \
        width=(int)640, height=(int)360, format=(string)BGRx ! \
        videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    
    VideoCapture source(src, CAP_GSTREAMER);
    
    if (!source.isOpened()){ cout << "Camera error" << endl; return -1; }
    
    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=192.168.0.250 port=8020 sync=false";
    
    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);
    
    if (!writer1.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    
    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=192.168.0.250 port=8030 sync=false";
    
    VideoWriter writer2(dst2, 0, (double)30, Size(640, 360), false);
    
    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    
    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=192.168.0.250 port=8040 sync=false";
    
    VideoWriter writer3(dst3, 0, (double)30, Size(640, 360), false);
    
    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    
    Mat frame, gray, bin;
    int64 startTick = getTickCount();
    
    while (true) {
        source >> frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }
        int64 currentTick = getTickCount();
        double elapsedSeconds = (currentTick - startTick) / getTickFrequency();
        string text = "Elapsed time: " + to_string(static_cast<int>(elapsedSeconds)) + " sec";
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 1.0;
        int thickness = 2;

        // 텍스트 위치 설정
        Point textOrg(10, 50);

        // 프레임에 텍스트 추가
        putText(frame, text, textOrg, fontFace, fontScale, Scalar(255, 255, 0), thickness);
        putText(gray, text, textOrg, fontFace, fontScale, Scalar(255, 255, 0), thickness);
        putText(bin, text, textOrg, fontFace, fontScale, Scalar(255, 255, 0), thickness);

        
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        threshold(gray, bin, 0, 255, THRESH_OTSU | THRESH_BINARY);
        writer1 << frame;
        writer2 << gray;
        writer3 << bin;
        waitKey(30);
    }
    return 0;
}
