#ifndef CCUSTCOM_H
#define CCUSTCOM_H
/*
*类功能：串口操作基类
*功能：
*       1.打开串口
*       2.关闭串口
*       3.获取串口当前状态
*
*/
#include "custdefine.h"

class CCustCom : public QObject
{
    Q_OBJECT
public:
    CCustCom();
    /*
*功能：打开串口
*参数：
*      _dev_name    串口描述文件
*      _baud_rate       波特率
*      _data_bits   数据位
*      _stop_bits   停止位
*      _patity_bits 校验位
*返回值：
*        true  打开成功
*        false 打开失败
*/
    bool Open_Com(QString _dev_name, int _baud_rate, int _data_bits, int _stop_bits, char _patity_bit, DEVICEINFO *_pDeviceInfo);
    /*
*功能：关闭串口
*参数：
*返回值：
*        true 关闭成功
*        false 关闭失败
*/
    int Close_Com();
    /*
*功能：判断串口是否已经打开
*返回值：
*        true  串口已经打开
*        false 串口未被打开
*/
    inline bool Is_Open() {return q_open_flag;}
protected:
    //串口开启标识，true:开启，false:关闭
    bool q_open_flag;
    //串口设备描述符，唯一
    int q_fd;

    /*
*功能：设置串口
*参数：
*       _speed       波特率
*       _data_bits   数据位
*       _stop_bits   停止位
*       _patity_bit  校验位
*返回值：
*        true  设置成功
*        false 设置失败
*/
    int set_com(int _baud_rate, int _data_bits, int _stop_bits, char _patity_bit);
    //设备信息结构体指针。
    DEVICEINFO *mp_DeviceInfo;
    struct termios q_term; //提供了一个常规的终端接口，用于控制非同步通信端口

    static const int speed_arr[15];
    static const int name_arr[15];
};
#endif // CCUSTCOM_H
