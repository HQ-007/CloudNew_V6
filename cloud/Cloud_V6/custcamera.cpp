#include "custcamera.h"
static int cv_count = 0;
CUSTCameraThread::CUSTCameraThread(QObject *parent):QThread(parent)
{
    //设定线程的状态
    Q_Thread_Status=true;
}
void CUSTCameraThread::run()
{
    //
    //    int save_flag = 1;
    while(Q_Thread_Status)
    {
        if(Q_P_CUSTCamera->get_frame())
        {
            mu.lock();
            //sleep(2);
            Q_P_CUSTCamera->push_queue();
            Q_P_CUSTCamera->q_get_image++;
            mu.unlock();
        }
        else
        {
            cout<<"摄像机故障"<<endl;
            //emit Q_P_CUSTCamera->camera_alarm(ERROR_CAMERA_CONNECT_ERROR);
        }
        sleep(1);
    }
}
CUSTImagePorThread::CUSTImagePorThread(QObject *parent):QThread(parent)
{
    //设定线程的状态
    Q_Thread_Status=true;
}
void CUSTImagePorThread::run()
{
    //cout<<q_get_image<<endl;
    while(Q_Thread_Status)
    {
        if(Q_P_CUSTCamera->q_get_image > 0)
        {
            p_mu.lock();
            Mat src = Q_P_CUSTCamera->original_pic.front();
            Q_P_CUSTCamera->original_pic.pop();
            Q_P_CUSTCamera->q_get_image--;
            processImage(src);
            p_mu.unlock();
        }
        sleep(1);
    }
}
Mat CUSTImagePorThread::correctImage(Mat src)
{
    Mat cameraMatrix = (Mat_<float>(3, 3) <<
                        6.217534326252483e+02, -0.040905710099330, 9.897132394898650e+02,
                        0, 6.204217922453229e+02, 5.300915746256371e+02,
                        0, 0, 1);

    //k1,k2,p1,p2,k3
    Mat distCoeffs = (Mat_<float>(1, 5) << -0.323462856577416, 0.095565961345248, 4.479987917179596e-04, -0.006309264611052, -0.011779507913652);
    cv::Size imageSize(1920, 1080);
    cv::Mat mapx = cv::Mat(imageSize, CV_32FC1);
    cv::Mat mapy = cv::Mat(imageSize, CV_32FC1);
    cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
    initUndistortRectifyMap(cameraMatrix, distCoeffs, R, cameraMatrix, imageSize, CV_32FC1, mapx, mapy);
    resize(src, src, Size(1920, 1080), INTER_CUBIC);
    Mat newimage = src.clone();
    remap(src, newimage, mapx, mapy, INTER_CUBIC);
    //imwrite("result1\\src\\" + to_string(i) + ".bmp", imageSource);
    //imwrite("result1\\dst\\" + to_string(i) + ".bmp", newimage);
    return newimage;
}
float CUSTImagePorThread::get_cov(Mat result)
{
    float count = 0.0;
    for(int i = 0;i < result.rows;i++)
    {
        for(int j = 0;j < result.cols;j++)
        {
            if(result.at<uchar>(i,j) >= 125)
                count++;
        }
    }
    float cover = count / (result.rows * result.cols);
    return cover;
}
Mat CUSTImagePorThread::process_mask(Mat src)
{
    Mat mask = Mat::zeros(Size(src.cols,src.rows),CV_8UC1);
    for(int i = 0;i < src.rows;i++)
    {
        for(int j = 0;j < src.cols;j++)
        {
            if(src.at<uchar>(i,j) < 125)
                mask.at<uchar>(i,j) = 0;
            else
                mask.at<uchar>(i,j) = 255;
        }
    }
    return mask;
}
Mat CUSTImagePorThread::get_mask(Mat src)
{
    Mat mask = Mat::zeros(Size(src.cols,src.rows),CV_8UC3);
    for(int i = 0;i < src.rows;i++)
    {
        for(int j = 0;j < src.cols;j++)
        {
            mask.at<Vec3b>(i,j)[0] = src.at<Vec3b>(i,j)[0] % 2 * 255;
            mask.at<Vec3b>(i,j)[1] = src.at<Vec3b>(i,j)[1] % 2 * 255;
            mask.at<Vec3b>(i,j)[2] = src.at<Vec3b>(i,j)[2] % 2 * 255;
        }
    }
    return mask;
}
void CUSTImagePorThread::processImage(Mat src)
{
    src = correctImage(src);
    Mat pic(Size(IMAGE_HEIGHT, IMAGE_WIDTH), CV_8UC3, cv::Scalar(255, 255, 255));
    if (src.size() != Size(IMAGE_HEIGHT, IMAGE_WIDTH)) {
        resize(src, src, Size(IMAGE_WIDTH, IMAGE_HEIGHT));
    }
    pthread_rwlock_wrlock(&Q_P_CUSTCamera->mp_Blocks->lock);
    memcpy(Q_P_CUSTCamera->mp_Blocks->data, src.data, IMAGE_SIZE);
    cout << "Wrote to memory." << endl;
    pthread_rwlock_unlock(&Q_P_CUSTCamera->mp_Blocks->lock);

    /***visSeg libtorch*****/
    const char* segmentationProgramFilePath = "/home/pi/CloudProject/VisSeg/bin/Debug/VisSeg";
    system(segmentationProgramFilePath);
    memcpy(pic.data, Q_P_CUSTCamera->mp_Blocks->data, IMAGE_SIZE);
    /**********************/

    cvtColor(pic,pic,COLOR_BGR2GRAY);
    Mat mask = process_mask(pic);
    resize(src,src,Size(1920,1080),0,0,INTER_CUBIC);
    resize(mask,mask,Size(1920,1080),0,0,INTER_CUBIC);
    Q_P_CUSTCamera->mp_DeviceInfo->m_VisImgCoverage = get_cov(mask);
    cout<<"cover="<<Q_P_CUSTCamera->mp_DeviceInfo->m_VisImgCoverage<<endl;
    for(int i = 0;i < mask.rows;i++)
    {
        for(int j = 0;j < mask.cols;j++)
        {
            if(mask.at<uchar>(i,j) < 128)
            {
                src.at<Vec3b>(i,j)[0] =  src.at<Vec3b>(i,j)[0] / 2 * 2;
                src.at<Vec3b>(i,j)[1] =  src.at<Vec3b>(i,j)[1] / 2 * 2;
                src.at<Vec3b>(i,j)[2] =  src.at<Vec3b>(i,j)[2] / 2 * 2;
            }
            else
            {
                src.at<Vec3b>(i,j)[0] =  src.at<Vec3b>(i,j)[0] / 2 * 2 + 1;
                src.at<Vec3b>(i,j)[1] =  src.at<Vec3b>(i,j)[1] / 2 * 2 + 1;
                src.at<Vec3b>(i,j)[2] =  src.at<Vec3b>(i,j)[2] / 2 * 2 + 1;
            }
        }
    }
    Q_P_CUSTCamera->mp_DeviceInfo->m_VisImg = src;
    imwrite("/home/pi/CloudProject/CloudNew_V6/pic/v_" + to_string(cv_count) + ".png",src);
    imwrite("/home/pi/CloudProject/CloudNew_V6/pic/vm_" + to_string(cv_count) + ".png",get_mask(src));
    sleep(30);
    cv_count++;
}

CUSTCamera::CUSTCamera(QObject *parent) : QObject(parent)
{
    q_camera_thread=new CUSTCameraThread(parent);
    q_camera_thread->Q_P_CUSTCamera=this;

    q_image_process = new CUSTImagePorThread(parent);
    q_image_process->Q_P_CUSTCamera = this;

}

CUSTCamera::~CUSTCamera()
{
    cv_cap.release();
    q_camera_thread->Q_Thread_Status = false;
    q_image_process->Q_Thread_Status = false;

    delete q_camera_thread;
    delete q_image_process;
}
bool CUSTCamera::Open_Camera(QString _dev_name, DEVICEINFO *_pDeviceInfo,SharedMemoryBlock *_pBlocks)
{
    mp_Blocks = _pBlocks;
    int dev_num = atoi(_dev_name.toStdString().c_str());
    mp_DeviceInfo = _pDeviceInfo;
    cout<<dev_num<<endl;
    //cout<<"adsafafd"<<endl;
    cv_cap = VideoCapture(dev_num);
    cv_cap.set(CAP_PROP_FRAME_WIDTH,1920);
    cv_cap.set(CAP_PROP_FRAME_HEIGHT,1080);
    //cv_cap.set(CAP_PROP_FPS,15);
    //mp_PicInfo = _Picinfo;
    if(cv_cap.isOpened())
    {
        cout<<"camera opens success"<<endl;
        return true;
    }
    else
    {
        cout<<"camera opens fails"<<endl;
        return false;
    }
}

void CUSTCamera::Start_Capture_Thread()
{
    this->q_camera_thread->start();
    sleep(1);
    this->q_image_process->start();
}

bool CUSTCamera::get_frame()
{
    cv_cap >> cv_frame;
    if(cv_frame.empty())
    {
        cout<<"frame empty"<<endl;
        return false;
    }
    return true;
}
void CUSTCamera::push_queue()
{
    if(original_pic.size()>=100)
        original_pic.pop();
    original_pic.push(cv_frame.clone());
}
