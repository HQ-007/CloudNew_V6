#ifndef CUSTIRCAMERA_H
#define CUSTIRCAMERA_H
#include "custdefine.h"
#include "uvc/ircammanager.h"
#include "uvc/v4l2core.h"
#include "/usr/local/include/opencv4/opencv2/opencv.hpp"
#include "/usr/local/include/opencv4/opencv2/opencv_modules.hpp"
#include "/usr/local/include/opencv4/opencv2/imgproc/imgproc_c.h"
#include <iostream>
#include <queue>
using namespace std;
class CUSTIrCameraThread;
class CUSTIrCamera;
class CUSTIrCameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CUSTIrCameraThread(QObject *parent = 0);
    CUSTIrCamera* Q_P_CUSTIrCamera;
    bool Q_Thread_Status;
protected:
    void run() override;
private:


signals:

};
class CUSTIrCamera : public QObject
{
    Q_OBJECT
    friend class CUSTIrCameraThread;
public:
    explicit CUSTIrCamera(QObject *parent = 0);
    ~CUSTIrCamera();
    DEVICEINFO *mp_DeviceInfo;
    IrCamManager* pCamManager;
    //queue<PICINFO> send_pic_data;
    void processImage(Mat);
    Mat correctImage(Mat);
    Mat segmentationImage(Mat);
    void Start_IrCapture_Thread();
    void get_frame();
    bool Open_IrCamera(QString _dev_name, DEVICEINFO *_pDeviceInfo);
private:
    Mat cv_frame;
    CUSTIrCameraThread *q_irimage_thread;

signals:
    //void camera_alarm(int);
};

#endif // CUSTIRCAMERA_H
