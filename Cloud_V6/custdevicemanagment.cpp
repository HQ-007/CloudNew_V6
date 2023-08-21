#include "custdevicemanagment.h"
#include <ctime>
CUSTDeviceManagment::CUSTDeviceManagment()
{
    //share memory
    int shmid = shmget((key_t)SHM_KEY, sizeof(SharedMemoryBlock), 0666 | IPC_CREAT);
    void *shm = shmat(shmid, (void*)0, 0);
    Block = (SharedMemoryBlock*) shm;
    pthread_rwlockattr_init(&Block->attr);
    pthread_rwlockattr_setpshared(&Block->attr, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init(&Block->lock, &Block->attr);
    //Q_M_DB.InitDatabase();
    char Scmd[256];
    char* cmd_file = "/home/pi/CloudProject/CloudNew_V6/scp.sh";
    char* user_name = "root";
    char* server_ip = "47.96.172.254";
    char* server_passwd = "qqwwee123?";
    char* server_path = "/home/yun/mask/*.png";
    char* local_path = "/home/pi/CloudProject/mask/";
    sprintf(Scmd, "%s %s@%s:%s %s \"%s\"",cmd_file,user_name,server_ip,server_path,local_path,server_passwd);
    int ret = system(Scmd);
    if(ret == 0)
    {
        cout<<"success"<<endl;
    }
    Q_M_P_DeviceInfo = new DEVICEINFO();
    //cout<<curtime<<endl;
    uchar device[6] = {0x01,0x10,0x04,0x18,0x00,0x01};
    memcpy(Q_M_P_DeviceInfo->m_DeviceID, device, 6);
    /****************/
    wiringPiSetup();
    pinMode(0,OUTPUT);       //GPIAO1_A1 J5
    digitalWrite(0,HIGH);
    pinMode(2,OUTPUT);       //GPIO1_A3 J6
    digitalWrite(2,HIGH);
    pinMode(7,OUTPUT);       //GPIO1_A0 J7
    digitalWrite(7,LOW);
    pinMode(1,OUTPUT);
    digitalWrite(1,LOW);   //GPIO1_C2 J8
    /****************/

//    time_t now = time(0);
//    tm *ltm = localtime(&now);
//    char work_id[6];
//    sprintf(work_id,"%c%c%c%c%c%c",(1900+ltm->tm_year) % 100 ,1 + ltm->tm_mon,ltm->tm_mday,
//            (ltm->tm_hour + 8) % 24,ltm->tm_min,ltm->tm_sec);
//    memcpy(Q_M_P_DeviceInfo->m_WorkID,work_id,6);
//    memcpy(Q_M_PicInfo->pm_WorkID,work_id,6);
}

bool CUSTDeviceManagment::Open_Device()
{
/*
设备	波特率	数据位	停止位	校验位	设备文件
GPS	9600	7	1	n	ttyAMA2
AVR	9600	8	1	n	ttyAMA3
GPRS	115200	8	1	n	ttyAMA4
GPIO					shensongji
Cam					    video9
*/
    Q_M_VisCamera.Open_Camera("10", Q_M_P_DeviceInfo,Block);
    //Q_M_IrCamera.Open_IrCamera("/dev/video12",Q_M_P_DeviceInfo);
    //Q_M_TemHum.Open_TemHum("/dev/i2c-2",Q_M_P_DeviceInfo);
    //Q_M_GPS.Open_Com("/dev/ttyUSB0",9600,7,1,'n', Q_M_P_DeviceInfo);
    Q_M_SocketPic.Open_PicSocket("47.96.172.254",5566,Q_M_P_DeviceInfo); //"47.96.172.254""192.168.3.67""8.142.68.234"
    return true;
}
void CUSTDeviceManagment::Start_Thread()
{
    cout<<"start"<<endl;
    Q_M_VisCamera.Start_Capture_Thread();
    //Q_M_IrCamera.Start_IrCapture_Thread();
    //Q_M_TemHum.Start_TemHum_Thread();
    //Q_M_GPS.Start_Thread();
    Q_M_SocketPic.Start_Socket();
    //sleep(2);
}
CUSTDeviceManagment::~CUSTDeviceManagment()
{
    delete Q_M_P_DeviceInfo;
}



