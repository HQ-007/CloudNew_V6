#ifndef CUSTCAMERA_H
#define CUSTCAMERA_H
#include "custdefine.h"
#include <QThread>
#include <QString>
#include <iostream>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/types_c.h>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <stdarg.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
using namespace cv;
using namespace std;

#define BUFFER_SIZE 1024

class CUSTCameraThread;
class CUSTImagePorThread;
class CUSTCamera;

class CUSTCameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CUSTCameraThread(QObject *parent = 0);
public:
    CUSTCamera* Q_P_CUSTCamera;
    bool Q_Thread_Status;
    mutex mu;
protected:
    void run();
private:

signals:
    //void Capture();
};
class CUSTImagePorThread : public QThread
{
    Q_OBJECT
public:
    explicit CUSTImagePorThread(QObject *parent = 0);
public:
    CUSTCamera* Q_P_CUSTCamera;
    bool Q_Thread_Status;
    mutex p_mu;
protected:
    void run();
private:
    //void send_info(Mat src,int partition);
    void processImage(Mat);
    Mat correctImage(Mat);
    float get_cov(Mat);
    Mat get_mask(Mat);
    Mat process_mask(Mat);
signals:

};
class CUSTCamera : public QObject
{
    Q_OBJECT
    friend class CUSTCameraThread;
    friend class CUSTImagePorThread;
public:
    explicit CUSTCamera(QObject *parent = 0);
    ~CUSTCamera();
    DEVICEINFO *mp_DeviceInfo;
    SharedMemoryBlock *mp_Blocks;
    //queue<PICINFO> send_pic_data;
    queue<Mat> original_pic;
    void Start_Capture_Thread();
    bool Open_Camera(QString _dev_name, DEVICEINFO *_pDeviceInfo,SharedMemoryBlock *_pBlocks);
    void push_queue();
    bool get_frame();
    VideoCapture cv_cap;
    int q_get_image=0;
private:
    CUSTCameraThread *q_camera_thread;
    CUSTImagePorThread *q_image_process;
    Mat cv_frame;

signals:
    //void camera_alarm(int);

};

#endif // CUSTCAMERA_H
