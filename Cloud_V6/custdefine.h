#ifndef CUSTDEFINE_H
#define CUSTDEFINE_H

#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string>
#include <unistd.h>     /*Unix标准函数定义*/
#include <fstream>
#include <sstream>
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <queue>
#include <QObject>
#include <QThread>
#include <QStringList>
#include <QtDebug>
//#include <QLabel>
//#include <QLCDNumber>
//#include <QImage>
#include <QDateTime>
#include <QObject>
//#include <QImage>
//#include <QMainWindow>
//#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
//#include <QTableView>
//#include <QDialog>
//#include <QHeaderView>
//#include <QFont>
//#include <QMessageBox>
//#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <sys/shm.h>
using namespace cv;
#define CLEAR(x) memset(&(x), 0, sizeof(x))//清除内存
#define LIMIT(x) (((x<0?0:x)>255)?255:x)//像素值限制在255以内
#define IMAGEWIDTH 320//图像的宽度
#define IMAGEHEIGHT 240//图像的高度
#define BUFFERNUMBER 4
#define BUFFERSIZE 1024
#define PICBUFFERSIZE 500000
#define SENDBYTE 54

#define AVR_INVALID_DATA                     5
#define AVR_INVALID_TIMES                   15


#define GPS_INVALID_DATA                    5
#define GPS_INVALID_TIMES                  15

#define CAMERA_INVALID_FRAMES       100

#define GPRS_SERVER_CONNECT 0xFE



#define ERROR                                           0x00EE0000

#define ERROR_AVR                                   (ERROR | 0x00000100)
#define ERROR_GPS                                   (ERROR | 0x00000200)
#define ERROR_GPRS                                 (ERROR | 0x00000300)
#define ERROR_GPIO                                 (ERROR | 0x00000400)
#define ERROR_CAMERA                            (ERROR | 0x00000500)


#define ERROR_AVR_CONNECT_ERROR     (ERROR_AVR + 1)
#define ERROR_AVR_SEED_STUCK            (ERROR_AVR + 2)

#define ERROR_GPS_CONNECT_ERROR   (ERROR_GPS + 1)
#define ERROR_GPS_INVALID_DATA         (ERROR_GPS + 2)

#define ERROR_GPRS_CONNECT_ERROR (ERROR_GPRS + 1)
#define ERROR_GPRS_RECEIVE_ERROR    (ERROR_GPRS + 2)

#define ERROR_GPIO_CONNECT_ERROR (ERROR_GPIO + 1)

#define ERROR_CAMERA_CONNECT_ERROR (ERROR_CAMERA + 1)

#define DEBUG_VERSION

#define PI 3.1415926
#define SHM_KEY 1275
#define IMAGE_WIDTH  600
#define IMAGE_HEIGHT  600
#define IMAGE_CHANNELS  3
#define IMAGE_SIZE  IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_CHANNELS

struct SharedMemoryBlock {
    pthread_rwlock_t lock;
    pthread_rwlockattr_t attr;
    uchar data[IMAGE_SIZE];
};

struct DEVICEINFO
{
    unsigned char m_Head;
    int m_FrameLen;  //frame length
    unsigned char m_DeviceID[6];  //deviceID
    int m_Year;// only two number, eg. 15,16,17.
    int m_Month;
    int m_Day;
    int m_Hour;
    int m_Minute;
    int m_Second;
    char m_GPSStatus; // A validat V, invalidat
    double m_Longitude;
    double m_Lattitude;
    float m_OutTemperature;
    float m_InTemperature;
    float m_OutHumidity;
    float m_InHumidity;
    unsigned char m_VisImgWidth[2];
    unsigned char m_VisImgHeight[2];
    unsigned char m_IrImgWidth[2];
    unsigned char m_IrImgHeight[2];
    double m_VisImgCoverage; // 0-100
    double m_IrImgCoverage; // 0-100
    double m_Coverage; // 0-100
    Mat m_VisImg;
    Mat m_IrImg;
    QString m_QCurrentTime;
    unsigned char m_Checkout;


    DEVICEINFO()
    {
        m_Head = 0xFE;
        m_FrameLen = 7203888;
        memset(m_DeviceID, 0, 6 * sizeof(char));
        m_Year = 0;
        m_Month = 0;
        m_Day = 0;
        m_Hour = 0;
        m_Minute = 0;
        m_Second = 0;
        m_GPSStatus = 'V';
        m_Longitude = 0.0;
        m_Lattitude = 0.0;
        m_OutTemperature = 0.0;
        m_InTemperature = 0.0;
        m_OutHumidity = 0.0;
        m_InHumidity = 0.0;
        m_VisImgWidth[0] = 0x07;
        m_VisImgWidth[1] = 0x80;
        m_VisImgHeight[0] = 0x04;
        m_VisImgHeight[1] = 0x38;

        m_IrImgWidth[0] = 0x02;
        m_IrImgWidth[1] = 0x80;
        m_IrImgHeight[0] = 0x02;
        m_IrImgHeight[1] = 0x00;
        m_VisImgCoverage = 0; // 0-100
        m_IrImgCoverage = 0; // 0-100
        m_Coverage = 0 ; // 0-100
        m_VisImg = NULL;
        m_IrImg = NULL;
        m_QCurrentTime = "";
        m_Checkout = 0;

    }
};



#endif // CUSTDEFINE_H
