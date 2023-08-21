#ifndef V4L2CORE_H_INCLUDED
#define V4L2CORE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define VIDIOC_REQBUFS_COUNT 2
#define SAFEFREE(p) \
{\
    if(p)\
    {\
        free(p);\
        p=NULL;\
    }\
}

typedef enum {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
} io_method;

typedef enum {
        FMT_H264,
        FMT_YUV,
        FMT_MJPEG,
} fmt_type;

typedef struct buffer {
        void *                  start;
        size_t                  length;
} buffer;

typedef struct DataBuffer{
    unsigned char*      buf;
    size_t              size;
}DataBuffer;


typedef struct v4l2_dev_t{
    //device
    int fd;
    char *deviceName;

    //device mutex
    pthread_mutex_t mutex;

    //UVC
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	struct v4l2_format fmtack;
	struct v4l2_streamparm frameint;
	struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum frmsozeenum;
    struct v4l2_frmivalenum frmivalenum;

    //video
    unsigned int width;
    unsigned int height;
    unsigned int fps;
    fmt_type     fmtType;

    DataBuffer      data_buf;

    //capture
    io_method    io;
    buffer*      buffers;
    unsigned int n_buffers;

    unsigned int bcapture;

    //result data
    unsigned int breaddata;

} v4l2_dev_t;

unsigned int check_sum(unsigned char *a, int len);

int xioctl(int fd, int request, void* argp);

v4l2_dev_t* v4l2core_dev_open(const char* name);

void v4l2core_dev_clean(v4l2_dev_t *vd);

void v4l2core_dev_getinfo(v4l2_dev_t* vd);

int v4l2core_dev_init(v4l2_dev_t* vd,int res_w,int res_h,int fps);

void v4l2core_dev_uninit(v4l2_dev_t* vd);

int v4l2core_capture_start(v4l2_dev_t* vd);

void v4l2core_capture_loop(v4l2_dev_t* vd);

int v4l2core_capture_getframe(v4l2_dev_t* vd);

void v4l2core_capture_stop(v4l2_dev_t* vd);

//add
void enum_frame_inval(v4l2_dev_t *vd,uint32_t pixfmt,__u32 w,__u32 h);
#ifdef __cplusplus
}
#endif

#endif // V4L2CORE_H_INCLUDED
