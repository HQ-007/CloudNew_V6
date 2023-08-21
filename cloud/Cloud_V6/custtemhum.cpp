#include "custtemhum.h"
//float CUSTTemHum::out_temp, CUSTTemHum::out_humi, CUSTTemHum::in_temp, CUSTTemHum::in_humi;
//int CUSTTemHum::fd;
//int CUSTTemHum::temhum_status;
CUSTTemHumThread::CUSTTemHumThread(QObject *parent):QThread(parent)
{
    //设定线程的状态
    Q_Thread_Status = true;
}
void CUSTTemHumThread::run()
{
    while(Q_Thread_Status)
    {
        //cout<<"hhhhh"<<endl;
        Q_P_CUSTTemHum->get_TemHum();
        if(Q_P_CUSTTemHum->temhum_status == 1)
        {
            Q_P_CUSTTemHum->control_Tem();
        }
        sleep(1);
    }
}

CUSTTemHum::CUSTTemHum(QObject *parent) : QObject(parent)
{
    q_temum_thread = new CUSTTemHumThread();
    q_temum_thread->Q_P_CUSTTemHum = this;
}
CUSTTemHum::~CUSTTemHum()
{
    q_temum_thread->Q_Thread_Status = false;
    delete q_temum_thread;
    close(fd);
}
bool CUSTTemHum::Open_TemHum(QString _dev_name, DEVICEINFO *_pDeviceInfo)
{
    const char* I2C_DEVICE = _dev_name.toStdString().c_str();
    mp_DeviceInfo = _pDeviceInfo;
    fd = open(I2C_DEVICE,O_RDWR);
    if(fd<0)
        return false;
    else
        return true;
}
uint16_t CUSTTemHum::_calc_crc16(const uint8_t *buf, size_t len)
{
    uint16_t crc = 0xFFFF;
    while(len--) {
        crc ^= (uint16_t) *buf++;
        for (unsigned i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
uint16_t CUSTTemHum::_combine_bytes(uint8_t msb, uint8_t lsb)
{
    return ((uint16_t)msb << 8) | (uint16_t)lsb;
}
int CUSTTemHum::am2321(float *out_temperature, float *out_humidity)
{
    uint8_t data[8];
    if (ioctl(fd, I2C_SLAVE, AM2321_ADDR) < 0)
        return 2;
    write(fd, NULL, 0);
    usleep(1000);
    data[0] = 0x03;
    data[1] = 0x00;
    data[2] = 0x04;
    if (write(fd, data, 3) < 0)
        return 3;
    usleep(1600);
    if (read(fd, data, 8) < 0)
        return 4;
    if (data[0] != 0x03 || data[1] != 0x04)
        return 9;
    uint16_t crcdata = _calc_crc16(data, 6);
    uint16_t crcread = _combine_bytes(data[7], data[6]);
    if (crcdata != crcread)
        return 10;
    uint16_t temp16 = _combine_bytes(data[4], data[5]);
    uint16_t humi16 = _combine_bytes(data[2], data[3]);
    if (temp16 & 0x8000){
        sign = 0;
        temp16 = temp16 & 0x7FFF;
   }
    if(sign == 1){
        *out_temperature = (float)temp16 / 10.0;
    }else{
        *out_temperature = (float)temp16 / 10.0 * -1;
    }
    *out_humidity = (float)humi16 / 10.0;
    return 0;
}
void CUSTTemHum::get_TemHum()
{
    if(mark==0)
    {
        mark = 1;
    }
    else
    {
        mark = 0;
    }
    digitalWrite(3, mark);
    ret = am2321(&temp, &humi);
    if (ret)
    {
        if(ret==3)
        {
            if(mark==0)
            {
                mark = 1;
            }
            else
            {
                mark = 0;
            }
        }
        else
        {
            in_temp = -100;
            in_humi = -100;
            out_temp = -100;
            out_humi = -100;
            //return false;
            temhum_status = 0;
        }
    }
    else
    {
        if(mark == 0)
        {
            in_temp = temp;
            in_humi = humi;
        }
        if(mark == 1)
        {
            out_temp = temp;
            out_humi = humi;
        }
        //return true;
        temhum_status = 1;
    }
    //cout<<"temhum_status:"<<temhum_status<<endl;
    sleep(1);
}
void CUSTTemHum::control_Tem()
{
    mp_DeviceInfo->m_InTemperature = in_temp;
    mp_DeviceInfo->m_InHumidity = in_humi;
    mp_DeviceInfo->m_OutTemperature = out_temp;
    mp_DeviceInfo->m_OutHumidity = out_humi;
    cout<<"in_Temperature:"<<mp_DeviceInfo->m_InTemperature<<endl;
    cout<<"in_Humidity:"<<mp_DeviceInfo->m_InHumidity<<endl;
    cout<<"out_Temperature:"<<mp_DeviceInfo->m_OutTemperature<<endl;
    cout<<"out_Humidity:"<<mp_DeviceInfo->m_OutHumidity<<endl;
    if(in_temp < 0)
    {
        cout<<"J7 7:high"<<endl;
        digitalWrite(7,HIGH);
    }
    else{
        cout<<"J7 7:low"<<endl;
        digitalWrite(7,LOW);
    }

    if(out_temp < 0)
    {
        cout<<"J8 1:high"<<endl;
        digitalWrite(1,HIGH);
    }
    else{
        cout<<"J8 1:low"<<endl;
        digitalWrite(1,LOW);
    }
}
void CUSTTemHum::Start_TemHum_Thread()
{
    q_temum_thread->start();
}
