#ifndef CUSTDEVICEMANAGMENT_H
#define CUSTDEVICEMANAGMENT_H
#include "custdefine.h"
#include "custcamera.h"
#include "custgps.h"
#include "ccustcom.h"
#include "custIrcamera.h"
#include "custtemhum.h"
#include "custclientsocket.h"

//#include "custdb.h"
class CUSTDeviceManagment
{
public:
    CUSTDeviceManagment();
    ~CUSTDeviceManagment();
    bool Open_Device();
    void Start_Thread();
    //void get_data();
    //void send_data();
private:
    CUSTCamera Q_M_VisCamera;
    CUSTIrCamera Q_M_IrCamera;
    CUSTTemHum Q_M_TemHum;
    CUSTClientSocket Q_M_SocketPic;
    //CUSTDB Q_M_DB;
    CUSTGPS Q_M_GPS;
    DEVICEINFO *Q_M_P_DeviceInfo; // global key struct for GPS, GPRS,etc.
    SharedMemoryBlock *Block;


    //queue<DEVICEINFO> send_data;
private slots:
    //void process_alarm(int _error);
};

#endif // CUSTDEVICEMANAGMENT_H
