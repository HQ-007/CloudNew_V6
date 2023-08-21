#ifndef CUSTTEMHUM_H
#define CUSTTEMHUM_H
#include "custdefine.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPi.h>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <iostream>
using namespace std;
#define AM2321_ADDR 0x5c
class CUSTTemHumThread;
class CUSTTemHum;


class CUSTTemHumThread : public QThread
{
    Q_OBJECT
public:
    explicit CUSTTemHumThread(QObject *parent = 0);
    CUSTTemHum* Q_P_CUSTTemHum;
    bool Q_Thread_Status;
protected:
    void run() override;
private:


signals:

};
class CUSTTemHum : public QObject
{
    Q_OBJECT
    friend class CUSTTemHumThread;
public:
    explicit CUSTTemHum(QObject *parent = 0);
    ~CUSTTemHum();
    int temhum_status;//static
    DEVICEINFO *mp_DeviceInfo;
    int fd; //static
    float out_temp, out_humi, in_temp, in_humi; //static
    bool mark = 1;
    int sign = 1;
    float temp, humi;
    int ret;
    void get_TemHum();
    void control_Tem();
    uint16_t _calc_crc16(const uint8_t *buf, size_t len);//static
    uint16_t _combine_bytes(uint8_t msb, uint8_t lsb);//static
    int am2321(float *out_temperature, float *out_humidity);//static
    void Start_TemHum_Thread();
    bool Open_TemHum(QString _dev_name, DEVICEINFO *_pDeviceInfo);
private:
    CUSTTemHumThread *q_temum_thread;
signals:

};

#endif // CUSTTEMHUM_H
