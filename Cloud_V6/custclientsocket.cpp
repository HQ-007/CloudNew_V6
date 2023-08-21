#include "custclientsocket.h"
//queue<PICINFO> send_pic_data;
static int ir_count = 0;

CUSTClientSocket::CUSTClientSocket(QObject *parent) : QObject(parent)
{

}
void CUSTClientSocket::init_socket()
{
    /* 初始化目标服务器的网络信息 */
    socket_desc = 0;
    memset(&server_addr, 0x00, sizeof(server_addr));
    // Set socket_desc server information
    server_addr.sin_addr.s_addr = inet_addr(ip_addres.c_str() );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
}
bool CUSTClientSocket::create_socket()
{
    //cout<<"create_socket"<<endl;
    socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_desc == -1)
    {
        std::cerr << "Cannot create socket" << std::endl;
        //close(socket_desc);
        return false;
    }
    return true;
}
bool CUSTClientSocket::connect_to_server()
{
    //cout<<"connect_server"<<endl;
    if (::connect(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1 )
    {
        std::cerr<< "Cannot connect to the server " << ip_addres << ":" << port_number << " " << std::strerror(errno) << std::endl;
        //close(socket_desc);
        return false;
    }
    cout << "Success connect to the server " << ip_addres << ":" << port_number << std::endl;
    return true;
}

bool CUSTClientSocket::Open_PicSocket(string ip,int port,DEVICEINFO *_pDeviceInfo)
{
    mp_DeviceInfo = _pDeviceInfo;
    ip_addres = ip;
    port_number = port;
    return true;
}
void CUSTClientSocket::Start_Socket()
{
    //q_sendpic_thread->start();
    init_socket();
    create_socket();
    connect_to_server();
    /* 从服务器接收初始化的握手消息 */
    len = recv(socket_desc, buf_recv, BUFSIZ, 0);			//接收服务器端信息
    buf_recv[len] = '\0';
    printf("%s", buf_recv);									//打印服务器端的欢迎信息
    printf("Enter string to send: \n");

    /* 创建父子进程与服务器进行通信 */
    if((pid = fork()) < 0){
        printf("Fail to call fork()\n");
        return;
    }

    /* 父进程用来发送数据 */
    else if(pid > 0){
        while(1){
            //database pre send

            char *Buffer;
            unsigned long Buffer_Size=0;
            getPicSendBuffer(Buffer,Buffer_Size);
            if(!strcmp(Buffer, "quit")){
                kill(pid, SIGSTOP);
                break;
            }
            if(Buffer_Size == 7203888)
                sendPic(Buffer,Buffer_Size);
            sleep(60);
        }
    }
    /* 子进程用来接收数据 */
    else{
        while(1){
            memset(buf_recv, 0, sizeof(buf_recv));
            if((len = recv(socket_desc, buf_recv, BUFSIZ, 0)) > 0){
                printf("Recive from server: %s\n", buf_recv);
            }
            //

            usleep(200000);
        }
    }
    /* 关闭套接字 */
    shutdown(socket_desc, SHUT_RD);
    close(socket_desc);

}
void CUSTClientSocket::UTC_to_BJtime(UTC utc_time, int8_t timezone)
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
    mp_DeviceInfo->m_Minute = u_t.u_minute;
    mp_DeviceInfo->m_Second = u_t.u_second;
}
void CUSTClientSocket::getPicSendBuffer(char* &buffer,unsigned long &buffer_size)
{
    sleep(2);
    //cout<<123456789<<endl;
    //QString q_buffer;
    Mat vis_img = mp_DeviceInfo->m_VisImg;
    //Mat vis_img = imread("/home/pi/CloudProject/CloudNew_V5/testpic/vis.png");
    int vis_pSize = vis_img.cols * vis_img.rows * vis_img.channels();
    cout<<"vis_pSize:"<<vis_pSize<<endl;

    Mat ir_img = mp_DeviceInfo->m_IrImg;
    //Mat ir_img = imread("/home/pi/CloudProject/CloudNew_V6/testpic/ir.png");
    int ir_pSize = ir_img.cols * ir_img.rows * ir_img.channels();
    cout<<"ir_pSize:"<<ir_pSize<<endl;
    if(vis_pSize == 1920*1080*3 && ir_pSize == 640*512*3)
    {
        uchar* q_write_buffer = new uchar[vis_pSize + ir_pSize + 48];
        memset(q_write_buffer, 0, sizeof(q_write_buffer));
        q_write_buffer[0] = 0xFC;
        memcpy(&q_write_buffer[1],&mp_DeviceInfo->m_FrameLen,4);
        memcpy(&q_write_buffer[5],mp_DeviceInfo->m_DeviceID,6);
        if(mp_DeviceInfo->m_GPSStatus == 'A')
        {
            long tmp = mp_DeviceInfo->m_Longitude*100000;
            printf("tmpLongitude:%ld\n",tmp);
            memcpy(&q_write_buffer[11],&tmp,sizeof(long));
            tmp = mp_DeviceInfo->m_Lattitude*100000;
            printf("tmpm_Lattitude:%ld\n",tmp);
            memcpy(&q_write_buffer[15],&tmp,sizeof(long));
        }
        if(mp_DeviceInfo->m_GPSStatus == 'V')
        {
            //cout<<"local"<<endl;
            time_t now = time(0);
            tm *ltm = gmtime(&now);
            u_t.u_year = (ltm->tm_year + 1900) % 100;
            u_t.u_month = ltm->tm_mon + 1;
            u_t.u_day = ltm->tm_mday;
            u_t.u_hour = ltm->tm_hour;
            u_t.u_minute = ltm->tm_min;
            u_t.u_second = ltm->tm_sec;
            UTC_to_BJtime(u_t,8);
        }
        char curtime[6];
        string name_time;
        sprintf(curtime,"%c%c%c%c%c%c",(unsigned char)mp_DeviceInfo->m_Year,
                (unsigned char)mp_DeviceInfo->m_Month, (unsigned char)mp_DeviceInfo->m_Day,
                (unsigned char)mp_DeviceInfo->m_Hour,(unsigned char)mp_DeviceInfo->m_Minute,
                (unsigned char)mp_DeviceInfo->m_Second);
        name_time = const_cast<char *>(curtime);
        memcpy(&q_write_buffer[19],&curtime,6);
        int ot_intpart,ot_fracpart,it_intpart,it_fracpart,oh_intpart,oh_fracpart,ih_intpart,ih_fracpart;
        ot_intpart = (int)mp_DeviceInfo->m_OutTemperature;
        ot_fracpart = (mp_DeviceInfo->m_OutTemperature - ot_intpart) * 100;
        q_write_buffer[25] = (unsigned char)ot_intpart;
        q_write_buffer[26] = (unsigned char)ot_fracpart;

        it_intpart = (int)mp_DeviceInfo->m_InTemperature;
        it_fracpart = (mp_DeviceInfo->m_InTemperature - it_intpart) * 100;
        q_write_buffer[27] = (unsigned char)it_intpart;
        q_write_buffer[28] = (unsigned char)it_fracpart;

        oh_intpart = (int)mp_DeviceInfo->m_OutHumidity;
        oh_fracpart = (mp_DeviceInfo->m_OutHumidity - oh_intpart) * 100;
        q_write_buffer[29] = (unsigned char)oh_intpart;
        q_write_buffer[30] = (unsigned char)oh_fracpart;

        ih_intpart = (int)mp_DeviceInfo->m_OutHumidity;
        ih_fracpart = (mp_DeviceInfo->m_OutHumidity - ih_intpart) * 100;
        q_write_buffer[31] = (unsigned char)ih_intpart;
        q_write_buffer[32] = (unsigned char)ih_fracpart;
        memcpy(&q_write_buffer[33],mp_DeviceInfo->m_VisImgWidth,2);
        memcpy(&q_write_buffer[35],mp_DeviceInfo->m_VisImgHeight,2);
        memcpy(&q_write_buffer[37],mp_DeviceInfo->m_IrImgWidth,2);
        memcpy(&q_write_buffer[39],mp_DeviceInfo->m_IrImgHeight,2);
        cout<<"vis:("<<(int)q_write_buffer[33]*256+(int)q_write_buffer[34]<<","<<(int)q_write_buffer[35]*256+(int)q_write_buffer[36]<<")"<<endl;
        cout<<"ir:("<<(int)q_write_buffer[37]*256+(int)q_write_buffer[38]<<","<<(int)q_write_buffer[39]*256+(int)q_write_buffer[40]<<")"<<endl;


        int viscov_intpart,viscov_fracpart,ircov_intpart,ircov_fracpart,cov_intpart,cov_fracpart;
        viscov_intpart = (int)mp_DeviceInfo->m_VisImgCoverage;
        viscov_fracpart = (mp_DeviceInfo->m_VisImgCoverage - viscov_intpart)*100;
        q_write_buffer[41] = (unsigned char)viscov_intpart;
        q_write_buffer[42] = (unsigned char)viscov_fracpart;

        ircov_intpart = (int)mp_DeviceInfo->m_IrImgCoverage;
        ircov_fracpart = (mp_DeviceInfo->m_IrImgCoverage - ircov_intpart) * 100;
        q_write_buffer[43] = (unsigned char)ircov_intpart;
        q_write_buffer[44] = (unsigned char)ircov_fracpart;

        mp_DeviceInfo->m_Coverage = (mp_DeviceInfo->m_VisImgCoverage + mp_DeviceInfo->m_IrImgCoverage) / 2.0;
        cov_intpart = (int)mp_DeviceInfo->m_Coverage;
        cov_fracpart = (mp_DeviceInfo->m_Coverage - cov_intpart)*100;
        q_write_buffer[45] = (unsigned char)cov_intpart;
        q_write_buffer[46] = (unsigned char)cov_fracpart;
    //    memcpy(&q_write_buffer[47],mp_DeviceInfo->m_VisImg.data,vis_pSize);
    //    memcpy(&q_write_buffer[47 + vis_pSize],(uchar*)mp_DeviceInfo->m_IrImg.data,ir_pSize);
        memcpy(&q_write_buffer[47],vis_img.data,vis_pSize);
        memcpy(&q_write_buffer[47 + vis_pSize],(uchar*)ir_img.data,ir_pSize);
        uchar sum = 0;
        for (int k = 0;k <= vis_pSize + ir_pSize + 48 - 2;k++) // 以上所有值的累加和
        {
            sum += q_write_buffer[k];
        }
        q_write_buffer[vis_pSize + ir_pSize + 47] = sum & 0xFF;
        buffer = (char*)q_write_buffer;
        buffer_size = 48 + vis_pSize + ir_pSize;
    }

}

void CUSTClientSocket::sendPic(char* &Buffer,unsigned long &Buffer_Size)
{
    int total = 0;
    int psize = (int)Buffer_Size;
    while(total < psize)
    {
        int length = send(socket_desc, Buffer + total, psize - total, 0);
        if(length != psize){
            break;
        }
        cout<<"length="<<length<<endl;
        //sleep(1);
        total = total + length;
    }
    cout << "Image has been send! " << total << endl;
}
CUSTClientSocket::~CUSTClientSocket()
{
//    q_sendpic_thread->Q_Thread_Status = false;
//    delete q_sendpic_thread;

}

