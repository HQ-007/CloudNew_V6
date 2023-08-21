#include "custIrcamera.h"
static int cv_count = 0;
queue<Mat> origin_irpic;
//static int q_get_irimage=0;
CUSTIrCameraThread::CUSTIrCameraThread(QObject *parent):QThread(parent)
{
    //设定线程的状态
    Q_Thread_Status = true;
}
void onReciveFrameData(unsigned char* data)
{
    static int k = 0;
    if(k++ < 25)
    {
        return ;
    }
    else
    {
        k = 0;
    }
    IplImage * pImg = cvCreateImage(cvSize(640, 512), 8, 2);
    memcpy(pImg->imageData,data,640*512*2);
    IplImage * pImgGray2 = cvCreateImage(cvGetSize(pImg), 8, 3);
    cvCvtColor(pImg, pImgGray2, CV_YUV2BGR_YUYV);
    Mat rgb=cvarrToMat(pImgGray2);
    if(origin_irpic.size()>=100)
        origin_irpic.pop();
    origin_irpic.push(rgb.clone());
    sleep(1);
    cvReleaseImage(&pImg);
    cvReleaseImage(&pImgGray2);
}
void CUSTIrCameraThread::run()
{
    Q_P_CUSTIrCamera->pCamManager->SetDataCallback(&onReciveFrameData);
    while(Q_Thread_Status)
    {
        Q_P_CUSTIrCamera->get_frame();
        sleep(1);
    }
}

CUSTIrCamera::CUSTIrCamera(QObject *parent) : QObject(parent)
{
    pCamManager = new IrCamManager();
    q_irimage_thread = new CUSTIrCameraThread();
    q_irimage_thread->Q_P_CUSTIrCamera = this;

}
CUSTIrCamera::~CUSTIrCamera()
{
    q_irimage_thread->Q_Thread_Status = false;
    delete q_irimage_thread;
}

bool CUSTIrCamera::Open_IrCamera(QString _dev_name, DEVICEINFO *_pDeviceInfo)
{
    const char* devname = _dev_name.toStdString().c_str();
    mp_DeviceInfo = _pDeviceInfo;
    int res_w = 640,res_h = 512;
    int fps=50;
    cout<<"capture parameter:res="<<res_w<<"x"<<res_h<<endl;
    int ret = pCamManager->OpenDevice(devname,res_w,res_h,fps);
    if(ret<0)
    {
        cout<<"Can not link video device"<<endl; //add
        return false;
    }
    cout<<"Link video device successful"<<endl; //add
    return true;
}
Mat CUSTIrCamera::correctImage(Mat src)
{
    Mat cameraMatrix = (Mat_<float>(3, 3) <<
                        2.427488673296640e+02, 0.851313657370516, 3.142313719007926e+02,
                        0, 2.421268714714751e+02, 2.639390352014907e+02,
                        0, 0, 1);

    //k1,k2,p1,p2,k3
    Mat distCoeffs = (Mat_<float>(1, 5) << -0.278210929686695, 0.072831465652393, 0.001928302926122, 0.003504553282148, -0.007904289399026);
    cv::Size imageSize(640, 512);
    cv::Mat mapx = cv::Mat(imageSize, CV_32FC1);
    cv::Mat mapy = cv::Mat(imageSize, CV_32FC1);
    cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
    initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
    resize(src, src, Size(640, 512), INTER_CUBIC);
    Mat newimage = src.clone();
    remap(src, newimage, mapx, mapy, INTER_CUBIC);
    //imwrite("result1\\src\\" + to_string(i) + ".bmp", imageSource);
    //imwrite("result1\\dst\\" + to_string(i) + ".bmp", newimage);
    return newimage;
}
Mat get_texture(Mat img, int n)
{
    Mat dst = Mat::zeros(img.size(), CV_8UC1);
    int hs = floor(n / 2);
    for (int i = hs; i < img.rows - hs - 1; i++)
        for (int j = hs; j < img.cols - hs - 1; j++) {
            float mean = (img.at<uchar>(i - hs, j - hs) + img.at<uchar>(i, j - hs) + img.at<uchar>(i - hs, j) + img.at<uchar>(i, j )) / 4.0;
            dst.at<uchar>(i, j) = (int)sqrt(pow(img.at<uchar>(i, j) - mean, 2));
        }
    return dst;
}
Mat CUSTIrCamera::segmentationImage(Mat src)
{
    Mat gray,mask1,mask2,mask;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    Mat texture = get_texture(gray, 3);
    threshold(gray, mask1, 0, 255, THRESH_OTSU);
    threshold(texture, mask2, 0, 255, THRESH_OTSU);
    mask = mask1 + mask2;
    return mask;
}
void CUSTIrCamera::processImage(Mat src)
{
    if(cv_count % 30 == 0)
    {
        imwrite("/home/pi/CloudProject/CloudNew_V6/pic/i_" + to_string(cv_count) + ".png",src);
    }
    Mat corimg = correctImage(src.clone());
    Mat mask = segmentationImage(corimg.clone());
    double cover = 0.0;
    double count = 0.0;
    for (int i = 0; i < mask.rows; i++)
        for (int j = 0; j < mask.cols; j++)
        {
            if (mask.at<uchar>(i, j) == 255)
            {
                count++;
            }
        }
    //cout<<"count="<<count<<endl;
    cover = count / (mask.rows * mask.cols) * 100;
    mp_DeviceInfo->m_IrImgCoverage = cover;
    //cout<<"m_IrImgCoverage="<<cover<<endl;
    resize(src,src,Size(640,512),0,0,INTER_CUBIC);
    resize(mask,mask,Size(640,512),0,0,INTER_CUBIC);
    for(int i = 0;i < mask.rows;i++)
    {
        for(int j = 0;j < mask.cols;j++)
        {
            if(mask.at<uchar>(i,j) < 128)
            {
                corimg.at<Vec3b>(i,j)[0] =  corimg.at<Vec3b>(i,j)[0] / 2 * 2;
                corimg.at<Vec3b>(i,j)[1] =  corimg.at<Vec3b>(i,j)[1] / 2 * 2;
                corimg.at<Vec3b>(i,j)[2] =  corimg.at<Vec3b>(i,j)[2] / 2 * 2;
            }
            else
            {
                corimg.at<Vec3b>(i,j)[0] =  corimg.at<Vec3b>(i,j)[0] / 2 * 2 + 1;
                corimg.at<Vec3b>(i,j)[1] =  corimg.at<Vec3b>(i,j)[1] / 2 * 2 + 1;
                corimg.at<Vec3b>(i,j)[2] =  corimg.at<Vec3b>(i,j)[2] / 2 * 2 + 1;
            }
        }
    }
    mp_DeviceInfo->m_IrImg = corimg.clone();
    if(cv_count % 30 == 0)
    {
        imwrite("/home/pi/CloudProject/CloudNew_V6/pic/ir_" + to_string(cv_count) + ".png",mp_DeviceInfo->m_IrImg);
    }
    cv_count++;
}
void CUSTIrCamera::get_frame()
{
    if(!origin_irpic.empty())
    {
        //cout<<"q_get_irimage:"<<q_get_irimage<<endl;
        cv_frame = origin_irpic.front().clone();
        processImage(cv_frame);
        origin_irpic.pop();
    }
}
void CUSTIrCamera::Start_IrCapture_Thread()
{
    q_irimage_thread->start();

}
