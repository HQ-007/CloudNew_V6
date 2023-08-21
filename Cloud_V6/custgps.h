#ifndef CUSTGPS_H
#define CUSTGPS_H

#include "custdefine.h"
#include "ccustcom.h"
class CUSTGPSThread;
class CUSTGPS;
class CUSTGPSThread : public QThread
{
    Q_OBJECT
public:
    explicit CUSTGPSThread(QObject *parent = 0);

private:

public:
    CUSTGPS* Q_P_CUSTGPS;
    bool Q_Thread_Status;
private:

protected:
    void run();
};

class CUSTGPS : public CCustCom
{
    Q_OBJECT
    friend class CUSTGPSThread;
public:
    explicit CUSTGPS();
    ~CUSTGPS();
    struct UTCtime
    {
        int u_year;
        int u_month;
        int u_day;
        int u_hour;
        int u_minute;
        int u_second;
    };
    UTCtime u_time;
    void Start_Thread();
    bool Read_GPS();
    bool Analysis_Data();
    void UTC_to_BJtime(UTCtime utc_time, int8_t timezone);
private:

public:
//    double Q_previous_coordinate_lng;
//    double Q_previous_coordinate_lat;

private:
    CUSTGPSThread * q_p_custgpsthread;
    uchar q_read_buffer[BUFFERSIZE];
    int q_invalid_times;
    int q_invalid_data;
    int q_read_data_size;

signals:
    //void gps_alarm(int);

};
#endif // CUSTGPS_H
