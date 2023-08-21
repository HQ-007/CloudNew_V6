#include "ircammanager.h"
#include <iostream>
FrameBufferCallback cb_frame;
VideoDataCallback   cb_video;
ELFVideoDataCallback cb_elf_video;
TempDataCallback cb_temp;

static void get_frame_idx_len(const unsigned char *pic_ptr, unsigned int *index, unsigned int *length){
    unsigned int frame_idx = 0;
    unsigned int frame_len = 0;
    if (pic_ptr[2] == 0xFF && pic_ptr[3] == 0xE1) {
        if (pic_ptr[4] == 0x00 && pic_ptr[5] == 0x12)
            frame_idx = (pic_ptr[10] & 0xff) << 24 | (pic_ptr[11] & 0xff) << 16 |
            (pic_ptr[12] & 0xff) << 8 | (pic_ptr[13] & 0xff) << 0;
        else if (pic_ptr[4] == 0x00 && pic_ptr[5] == 0x16) {
            frame_idx = (pic_ptr[10] & 0xff) << 24 | (pic_ptr[11] & 0xff) << 16 |
                (pic_ptr[12] & 0xff) << 8 | (pic_ptr[13] & 0xff) << 0;
            frame_len = *(unsigned int*)(pic_ptr + 22);
        }
    }
    (*index) = frame_idx;
    (*length) = frame_len;
}

static int get_data_type(const unsigned char *data_ptr, int length){
    if (data_ptr[0] == 0xFF && data_ptr[1] == 0x55 && data_ptr[2] == 0xFF && data_ptr[3] == 0xAA){
        return 2;
    }
    else{
        return 1;
    }
}

void *uvc_read_task(void* arg){
    v4l2_dev_t* vd = (v4l2_dev_t*)arg;
    if(!vd){
        pthread_exit(NULL);
    }
    int ret;
    int data_type;
    while(vd->bcapture){
        pthread_mutex_lock(&vd->mutex);
        if(!vd->bcapture){
            pthread_mutex_unlock(&vd->mutex);
            break;
        }
        ret = v4l2core_capture_getframe(vd);
        pthread_mutex_unlock(&vd->mutex);
        if(ret<0)
            continue;
        if(cb_frame){
            cb_frame(vd->data_buf.buf);
        }
        else if(cb_video&&cb_temp){
            int width=vd->width/2;
            int height=vd->height;
            unsigned char* video_data=new unsigned char[width*height*2];
            unsigned short* temp_data=new unsigned short[width*height*2];
            for(int i =0; i<height; i++)
            {
                memcpy(video_data+i*width*2,vd->data_buf.buf+i*width*4, width*2);
            }
            for(int i = 0; i<height; i++)
            {
                memcpy((unsigned char*)temp_data+i*width*2, vd->data_buf.buf+i*width*4+width*2, width*2);
            }
            cb_video(video_data);
            cb_temp(temp_data);
            delete[] video_data;
            delete [] temp_data;
        }
        else if(cb_elf_video&&cb_temp)
        {
            int width=vd->width/2;
            int height=vd->height;
            unsigned char* video_data=new unsigned char[width*height*4];
            unsigned short* temp_data=new unsigned short[width*height*2];
            for(int i =0; i<height; i++)
            {
                memcpy(video_data+i*width*2,vd->data_buf.buf+i*width*4, width*2);
            }
            for(int i = 0; i<height; i++)
            {
                memcpy((unsigned char*)temp_data+i*width*2, vd->data_buf.buf+i*width*4+width*2, width*2);
            }

            if(mPaletteIndex==0)
            {
                for(int j=0;j<height;j++)
                {
                    for(int i=0;i<width*2;i++)
                    {
                        video_data[j*width*4+i*2+1]=0x80;
                    }
                }
                cb_elf_video(video_data);
            }
            else if(mPaletteIndex==1)
            {
                for(int j=0;j<height;j++)
                {
                    for(int i=0;i<width*2;i++)
                    {
                        video_data[j*width*4+i*2]=255-video_data[j*width*4+i*2];
                        video_data[j*width*4+i*2+1]=0x80;
                    }
                }
                cb_elf_video(video_data);
            }
            else
            {
                unsigned char* out_palette_data=new unsigned char[width*height*4];
                memset(out_palette_data,0,sizeof (out_palette_data));
                for(int j=0;j<height;j++)
                {
                    for(int i=0;i<width*2;i++)
                    {
                        int iY=j*width*2+i*2;
                        out_palette_data[iY]=palettedata[mPaletteIndex*1024+video_data[iY]*4+2];
                        if(i%2==0)
                        {
                            out_palette_data[iY+1]=palettedata[mPaletteIndex*1024+video_data[iY]*4+1];
                        }
                        else
                        {
                            out_palette_data[iY+1]=palettedata[mPaletteIndex*1024+video_data[iY]*4];
                        }
                    }
                }                                
                cb_elf_video(out_palette_data);
                delete [] out_palette_data;
            }
            cb_temp(temp_data);
            delete[] video_data;
            delete [] temp_data;
        }
    }
    pthread_exit(NULL);
}

IrCamManager::IrCamManager():vd(NULL)
{
    cb_frame = NULL;
    cb_video = NULL;
    cb_temp=NULL;
    char cPath[1024];
    strcpy(cPath,"/home/pi/CloudProject/CloudNew_V6/dat/outPaletteFTII.dat");
    std::cout<<"cPath:"<<cPath<<std::endl;
    FILE *fp=fopen(cPath,"rb");
    for(int i=0;i<1024*20;i++)
    {
        fscanf(fp,"%c",&palettedata[i]);
    }
}

int IrCamManager::OpenDevice(const char* name,int w,int h,int fps)
{
    int ret = 0;
    int rc = 0;
    if(vd){
        CloseDevice();
    }
    vd = v4l2core_dev_open(name);
    if(vd==NULL){
        ret = -1;
    }
    rc = v4l2core_dev_init(vd,w,h,fps);
    rc = v4l2core_capture_start(vd);
    if(rc<0){
        ret = -2;
    }
    vd->bcapture = 1;
    rc = pthread_create(&thread_read,NULL,uvc_read_task,(void*)vd);
    if(rc){
        ret = -3;
    }
    if(ret<0){
        CloseDevice();
    }
    return ret;
}

void IrCamManager::CloseDevice()
{
    std::cout<<"111"<<std::endl;
    if(vd==NULL){
        std::cout<<"222"<<std::endl;
        return;}
    pthread_mutex_lock(&vd->mutex);
    vd->bcapture = 0;
    pthread_mutex_unlock(&vd->mutex);
    v4l2core_capture_stop(vd);
    v4l2core_dev_uninit(vd);
    v4l2core_dev_clean(vd);
    vd = NULL;
    std::cout<<"333"<<std::endl;
    return;
}

void IrCamManager::SetDataCallback(FrameBufferCallback func)
{
    cb_frame = func;
}

