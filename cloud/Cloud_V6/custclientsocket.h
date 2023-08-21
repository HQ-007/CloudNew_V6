#ifndef CUSTCLIENTSOCKET_H
#define CUSTCLIENTSOCKET_H
#include "custdefine.h"
#include "custcamera.h"
#include "custdb.h"
#include <QThread>
#include <QString>
#include <signal.h>
#include <sys/socket.h> // Core BSD socket functions and data structures
#include <sys/fcntl.h>  // for the non-blocking socket
#include <arpa/inet.h>  // for manipulating IP addresses, for inet_addr()
#include <unistd.h>     // for close()
#include <iostream>
#include <vector>
#include <string>
#include <cerrno>       // for errno
#include <cstring>      // for std::memset() and std::strerror()
#include <QObject>
#include <QMetaType>
#include <QFile>
using namespace std;
class CUSTClientSocket;

class CUSTClientSocket : public QObject
{
    Q_OBJECT
    friend class CUSTSendPicThread;
public:
    explicit CUSTClientSocket(QObject *parent = 0);
    ~CUSTClientSocket();
    DEVICEINFO *mp_DeviceInfo;
    struct UTC
    {
        int u_year;
        int u_month;
        int u_day;
        int u_hour;
        int u_minute;
        int u_second;
    };
    UTC u_t;

    bool Open_PicSocket(string ip,int port,DEVICEINFO *_pDeviceInfo);
    void init_socket();
    bool create_socket();//
    bool connect_to_server();//
    void Start_Socket();

    void getPicSendBuffer(char* &buffer, unsigned long &buffer_size);
    void sendPic(char* &Buffer,unsigned long &Buffer_Size);
    void UTC_to_BJtime(UTC utc_time, int8_t timezone);
//    //void DB_sendPicData(QString _pic_path,int _partition);
//    void DB_sendPicData(QString _curtime,double _longitude,double _lattitude,
//                     float _outtem,float _intem,float _outhum,float _inhum,double _viscov,
//                     double _ircov,double _cov,QString _vispath,QString _irpath);

private:
    string ip_addres;
    int port_number;
    int socket_desc;
    pid_t pid;
    int len;
    //bool conn;
    struct sockaddr_in server_addr;
    //FILE* S_file;
    FILE* pS_file;
    CUSTDB Q_M_DB;
    char buf_recv[BUFFER_SIZE];

};

#endif // CUSTCLIENTSOCKET_H
