#include "custgps.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
CUSTGPSThread:: CUSTGPSThread(QObject *parent):QThread(parent)
{
    Q_Thread_Status = true;
}

void  CUSTGPSThread::run()
{
    while (Q_Thread_Status)
    {
        if(Q_P_CUSTGPS->Read_GPS())
        {
//            qDebug("GPS read data success!");
            if(Q_P_CUSTGPS->Analysis_Data())
            {
//                qDebug("GPS analysis data success!");
                Q_P_CUSTGPS->q_invalid_data = 0;
            }
            else
            {
//                qDebug("GPS analysis data failed!");
                Q_P_CUSTGPS->q_invalid_data++;
                if(Q_P_CUSTGPS->q_invalid_data >= GPS_INVALID_DATA)
                {
//                    qDebug("GPS analysis invalid data: %d", Q_P_CUSTGPS->q_invalid_data);
                    //emit Q_P_CUSTGPS->gps_alarm(ERROR_GPS_INVALID_DATA);
                    Q_P_CUSTGPS->q_invalid_data = 0;
                }
            }
            Q_P_CUSTGPS->q_invalid_times = 0;
        }
        else
        {
//            qDebug("GPS read data failed!");
            Q_P_CUSTGPS->q_invalid_times++;
            if(Q_P_CUSTGPS->q_invalid_times >= GPS_INVALID_TIMES)
            {
//                qDebug("GPS read data invalid times: %d", Q_P_CUSTGPS->q_invalid_times);
                //emit Q_P_CUSTGPS->gps_alarm(ERROR_GPS_CONNECT_ERROR);
                Q_P_CUSTGPS->q_invalid_times = 0;
            }
        }
        msleep(1000);
    }
}

CUSTGPS::CUSTGPS()
{
    CLEAR(q_term);
    CLEAR(q_read_buffer);

    q_invalid_times = -1;
    q_invalid_data = -1;
    q_read_data_size = -1;

    q_p_custgpsthread = new CUSTGPSThread;
    q_p_custgpsthread->Q_P_CUSTGPS = this;

}
CUSTGPS::~CUSTGPS()
{
    q_p_custgpsthread->Q_Thread_Status = false;
    delete q_p_custgpsthread;
    q_p_custgpsthread = NULL;
}

void CUSTGPS::Start_Thread()
{
    q_p_custgpsthread->start();
}

bool CUSTGPS::Read_GPS()
{
    q_read_data_size = read(q_fd, &q_read_buffer, BUFFERSIZE);
    if(q_read_data_size > 0)
    {
        //qDebug("GPS read size is:%d", q_read_data_size);
        return true;
    }
    else
    {
        //qDebug("GPS read size is:%d", q_read_data_size);
        return false;
    }
}
void CUSTGPS::UTC_to_BJtime(UTCtime utc_time, int8_t timezone)
{
    int year, month, day, hour;
    int lastday = 0;					//last day of this month
    int lastlastday = 0;			//last day of last month

    year = utc_time.u_year;			 //utc time
    month = utc_time.u_month;
    day = utc_time.u_day;
    hour = utc_time.u_hour + timezone;
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        lastday = 31;
        lastlastday = 30;//这里应该添加上个月的天数
        if (month == 3) {
            if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))				//if this is lunar year
                lastlastday = 29;
            else
                lastlastday = 28;
        }
        if (month == 8)
            lastlastday = 31;
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11) {
        lastday = 30;
        lastlastday = 31;
    }
    else {
        lastlastday = 31;
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
            lastday = 29;
        else
            lastday = 28;
    }

    if (hour >= 24) {					// if >24, day+1
        hour -= 24;
        day += 1;

        if (day > lastday) { 		// next month,  day-lastday of this month
            day -= lastday;
            month += 1;

            if (month > 12) {		//	next year , month-12
                month -= 12;
                year += 1;
            }
        }
    }
    if (hour < 0) {										// if <0, day-1
        hour += 24;
        day -= 1;
        if (day < 1) {					  // month-1, day=last day of last month
            day = lastlastday;
            month -= 1;
            if (month < 1) { 			// last year , month=12
                month = 12;
                year -= 1;
            }
        }
    }
    mp_DeviceInfo->m_Year = year;
    mp_DeviceInfo->m_Month = month;
    mp_DeviceInfo->m_Day = day;
    mp_DeviceInfo->m_Hour = hour;
    mp_DeviceInfo->m_Minute = u_time.u_minute;
    mp_DeviceInfo->m_Second = u_time.u_second;
}
bool CUSTGPS::Analysis_Data()
{
    //ofstream outfile("1_GPS20211014.txt",ios::app);
    QString _gps_data = QString::fromStdString(std::string((const char*)q_read_buffer));
    //qDebug("GPS data is: %s", _gps_data.toStdString().c_str());
    int _begin = _gps_data.toStdString().find("$GNRMC");
    int _end = _gps_data.toStdString().find("$GNVTG");
    if(_end <= _begin)
      return true;  //??????????????
    QString _gprmc = QString::fromStdString(_gps_data.toStdString().substr(_begin + 7, _end - _begin - 7));
    if (_gprmc.length()<10)
        return false;
    //qDebug("GPS analysis data is: %s", _gprmc.toStdString().c_str());
    QStringList _gprmc_list = _gprmc.split(",");
    QString _cur_time = "";
    int  _iter = _gprmc_list.end() - _gprmc_list.begin();
    //cout<<"_iter:"<<_iter<<endl;
    if(_iter == 12)
    {
        if (_gprmc_list[1] == "A")
        {
            mp_DeviceInfo->m_GPSStatus = 'A';
        }
        sscanf(_gprmc_list[8].toStdString().c_str() ,"%2d%2d%2d",
                &u_time.u_day,&u_time.u_month,&u_time.u_year);
        sscanf(_gprmc_list[0].left(6).toStdString().c_str() ,"%2d%2d%2d",
                &u_time.u_hour,&u_time.u_minute,&u_time.u_second);
        UTC_to_BJtime(u_time,8);
        if (mp_DeviceInfo->m_Month || mp_DeviceInfo->m_Year)
        {
            QString _current_time = "";
            if(mp_DeviceInfo->m_Year <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Year);
            if(mp_DeviceInfo->m_Month <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Month);
            if(mp_DeviceInfo->m_Day <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Day);
            if(mp_DeviceInfo->m_Hour <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Hour);
            if(mp_DeviceInfo->m_Minute <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Minute);
            if(mp_DeviceInfo->m_Second <10)
                _current_time += "0";
            _current_time += QString::number(mp_DeviceInfo->m_Second);
            mp_DeviceInfo->m_QCurrentTime = _current_time;
            _cur_time = _current_time;
        }
        //qDebug()<<mp_DeviceInfo->m_QCurrentTime;
        mp_DeviceInfo->m_Lattitude = _gprmc_list[2].toDouble();
        printf("GPS:m_Lattitude:%0.5lf\n",mp_DeviceInfo->m_Lattitude);
        mp_DeviceInfo->m_Longitude = _gprmc_list[4].toDouble();
        printf("GPS:m_Longitude:%0.5lf\n",mp_DeviceInfo->m_Longitude);
        double tm_Lattitude,tm_Longitude;
        tm_Lattitude = _gprmc_list[2].toDouble() / 100.0;
        tm_Longitude = _gprmc_list[4].toDouble() / 100.0;
        return true;
    }
    else
        return false;
}
