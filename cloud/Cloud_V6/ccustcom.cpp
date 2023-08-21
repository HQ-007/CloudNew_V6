#include "ccustcom.h"

const int CCustCom::speed_arr[15] =
{B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,  B38400, B19200, B9600, B4800, B2400, B1200, B300};

const int CCustCom::name_arr[15] = {115200,38400,  19200,  9600,  4800,  2400,  1200,  300,  38400,  19200,  9600, 4800, 2400, 1200,  300};
CCustCom::CCustCom()
{
    q_open_flag = false;
    q_fd = -10;
}
bool CCustCom::Open_Com(QString _dev_name, int _baud_rate, int _data_bits, int _stop_bits, char _patity_bit, DEVICEINFO *_pDeviceInfo)
{
    mp_DeviceInfo = _pDeviceInfo; //
    q_fd = open(_dev_name.toStdString().c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if (-1 ==q_fd)
    {
        qDebug("Can't Open Serial Port");
        return -1;
    }
    qDebug("Open Serial Successed!");
    set_com(_baud_rate,  _data_bits,  _stop_bits,  _patity_bit); // set the parameter for COM
    q_open_flag = true;
    return 0;
}
int CCustCom::Close_Com()
{
    q_open_flag = false;
    if(-1 == close(q_fd))
    {
        return -1;
    }
    return 0;
}
// private function.

int CCustCom::set_com(int _baud_rate, int _data_bits, int _stop_bits, char _patity_bit)
{
    if(tcgetattr(q_fd, &q_term) != 0)
    {
        qDebug("Set speed can't get attr!");
        return -1;
    }

    tcflush(q_fd, TCIOFLUSH);

    for (int i = 0; i < 15;  i++)
    {
        if(_baud_rate == name_arr[i])
        {
            cfsetispeed(&q_term, speed_arr[i]);
            cfsetospeed(&q_term, speed_arr[i]);
        }
    }

    q_term.c_cflag &= ~CSIZE;

    switch (_data_bits)
    {
    case 7:
        q_term.c_cflag |= CS7;
        break;
    case 8:
        q_term.c_cflag |= CS8;
        break;
    default:
        qDebug("Set data bits unsupported data size!\n");
        return -1;
    }

    switch (_stop_bits)
    {
    case 1:
        q_term.c_cflag &= ~CSTOPB;
        break;
    case 2:
        q_term.c_cflag |= CSTOPB;
        break;
    default:
        qDebug("Set stop bits unsupported stop bits!\n");
        return -1;
    }

    switch (_patity_bit)
    {
    case 'n':
    case 'N':
        q_term.c_cflag &= ~PARENB;   /* Clear parity enable */
        q_term.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        q_term.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
        q_term.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        q_term.c_cflag |= PARENB;     /* Enable parity */
        q_term.c_cflag &= ~PARODD;   /* 转换为偶效验*/
        q_term.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        q_term.c_cflag &= ~PARENB;
        q_term.c_cflag &= ~CSTOPB;break;
    default:
        qDebug("Set parity bit unsupported parity!\n");
        return -1;
    }

    if (_patity_bit != 'n')
        q_term.c_iflag |= INPCK ;
    //清bit位  关闭字符映射 0x0a 0x0d
    q_term.c_iflag &= ~(INLCR|ICRNL);
    //清bit位  关闭流控字符 0x11 0x13
    q_term.c_iflag &= ~(IXON);

    //需要注意的是:
    //如果不是开发终端之类的，只是串口传输数据，而不需要串口来处理，那么使用原始模式(Raw Mode)方式来通讯，设置方式如下：
    q_term.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    q_term.c_oflag  &= ~OPOST;   /*Output*/

    tcflush(q_fd,TCIFLUSH);
    q_term.c_cc[VTIME]=100;//设置超时10秒
    q_term.c_cc[VMIN] = 0; /* Update the options and do it NOW */

    if(tcsetattr(q_fd, TCSANOW, &q_term) != 0)
    {
        qDebug("Set speed can't set attr!");
        return -1;
    }
    return 0;
}
