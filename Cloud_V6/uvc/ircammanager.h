#ifndef ARCAMMANAGER_H
#define ARCAMMANAGER_H

#include <stdint.h>
#include <vector>
#include "v4l2core.h"

struct TempData{
    unsigned char* data;
};
static unsigned char palettedata[1024*20]={0};

struct VideoData
{
    //视频帧帧号
    uint32_t frame_index;
    //解析长度(用于校验jpeg数据完整性，如果为0则表示没有长度信息)
    uint32_t ori_length;
    //实际数据长度
    uint32_t data_length;
    //数据buffer
    unsigned char* data;
};
static int mPaletteIndex=0;

typedef void (*FrameBufferCallback)(unsigned char* data);
typedef void (*VideoDataCallback)(unsigned char* data);
typedef void (*ELFVideoDataCallback)(unsigned char* data);
typedef void (*TempDataCallback)(unsigned short* data);

class IrCamManager
{
public:
    IrCamManager();

    //打开设备 0:成功 <0:失败
    int OpenDevice(const char* name,int w,int h,int fps);
    //关闭设备
    void CloseDevice();
    //设置原始数据回调函数
    void SetDataCallback(FrameBufferCallback func);
    //设置视频数据和温度数据回调（如果已经设置了原始数据回调，则视频和温度数据回调无效）
//    void SetDataCallback(VideoDataCallback videoCallback, TempDataCallback tempCallback);
//    void SetELFDataCallback(ELFVideoDataCallback elfVideoCallback, TempDataCallback tempCallback);
//    void SetELFPalette(int index);
private:
    v4l2_dev_t *vd;
    pthread_t thread_read;
};

#endif // ARCAMMANAGER_H
