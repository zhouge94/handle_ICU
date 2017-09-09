#include <iostream>
#include <nxp-v4l2.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "videodev2.h"
#include "v4l2-mediabus.h"
#include "videodev2_nxp_media.h"
#include "nxp_ion.h"
#include "common.cpp"
#include "jpeglib.h"
#include <signal.h>

using namespace std;

#define uchar unsigned char
extern int savejpg(uchar *pdata, char *jpg_file, int width, int height);
extern int convert_yuv_to_rgb_pixel(int y, int u, int v);
extern int yuvtorgb( unsigned char *rgb, unsigned int width, unsigned int height, struct nxp_vid_buffer *buf);

int ion_fd;
int width;
int height;
int sensor_width;
int sensor_height;
int module = 0;
int clipper_id 	= nxp_v4l2_clipper0;
int sensor_id 	= nxp_v4l2_sensor0;
int video_id 	= nxp_v4l2_mlc0_video;
int format      = V4L2_PIX_FMT_YUV422P;
struct V4l2UsageScheme s;

char fname[20];
char takePhoto = 0;
char stop = 0;

void ov5640_init()
{
    module = 1;
    width  = 800;
    height = 480;
    sensor_width = 1024;
    sensor_height = 768;

    s.useClipper1 = true;
    s.useDecimator1 = true;
    clipper_id = nxp_v4l2_clipper1;
    sensor_id = nxp_v4l2_sensor1;
    s.useMlc0Video = true;
}

void ov5645_init()
{
    module = 0;
    width  = 800;
    height = 480;
    sensor_width = 1280;
    sensor_height = 960;

    s.useClipper0   = true;
    s.useDecimator0 = true;
    clipper_id = nxp_v4l2_clipper0;
    sensor_id = nxp_v4l2_sensor0;
    s.useMlc0Video = true;
}

void start()
{
    unsigned char *rgb0 = new unsigned char [width * height * 3];//根据yuyv像素点和rgb像素所站内存比分配rgb
    v4l2_init(&s);
    v4l2_set_format(clipper_id, width, height, format);
    v4l2_set_crop(clipper_id, 0, 0, width, height);

    v4l2_set_format(sensor_id, sensor_width, sensor_height, V4L2_MBUS_FMT_YUYV8_2X8);
    v4l2_set_format(video_id, width, height, format);

    // setting destination position
    v4l2_set_crop(video_id, 0, 0, width, height);
    // setting source crop
    v4l2_set_crop_with_pad(video_id, 2, 0, 0, width, height); //psw 20150331

    v4l2_set_ctrl(video_id, V4L2_CID_MLC_VID_PRIORITY, 0);
    v4l2_set_ctrl(video_id, V4L2_CID_MLC_VID_COLORKEY, 0x0);
    v4l2_reqbuf(clipper_id, MAX_BUFFER_COUNT);
    v4l2_reqbuf(video_id, MAX_BUFFER_COUNT);

    struct nxp_vid_buffer bufs[MAX_BUFFER_COUNT];
    alloc_buffers(ion_fd, MAX_BUFFER_COUNT, bufs, width, height, format);

    int i;
    for (i = 0; i < MAX_BUFFER_COUNT; i++) {
        struct nxp_vid_buffer *buf = &bufs[i];
        printf("buf plane num: %d\n", buf->plane_num);
        v4l2_qbuf(clipper_id, buf->plane_num, i, buf, -1, NULL);
    }

    v4l2_streamon(clipper_id);

    int out_index = 0;
    int out_dq_index = 0;
    int out_q_count = 0;
    bool started_out = false;

    int capture_index = 0;
    int count = 0;

    while (!stop)
    {
        struct nxp_vid_buffer *buf = &bufs[capture_index];
        v4l2_dqbuf(clipper_id, buf->plane_num, &capture_index, NULL);
        v4l2_qbuf(video_id, buf->plane_num, out_index, buf, -1, NULL);

        if (count > 20 && takePhoto == 1)
        {
            yuvtorgb(rgb0, width, height, buf);
            savejpg(rgb0, fname, width, height);
            printf("take photo OK\n");
            break;
        }

        out_q_count++;
        out_index++;
        out_index %= MAX_BUFFER_COUNT;

        if (!started_out) {
            v4l2_streamon(video_id);
            started_out = true;
        }

        if (out_q_count >= 2) {
            v4l2_dqbuf(video_id, buf->plane_num, &out_dq_index, NULL);
            out_q_count--;
        }

        v4l2_qbuf(clipper_id, buf->plane_num, capture_index, buf, -1, NULL);
        count++;
    }

    v4l2_streamoff(video_id);
    v4l2_streamoff(clipper_id);

    v4l2_exit();
}

void menu_usage(void)
{
    char c;
    printf("[1] start camera ov5640\n");
//    printf("[2] start camera ov5645\n");
    printf("[2] ov5640 take photo\n");
//    printf("[4] ov5645 take photo\n");
    printf("[3] quit\n");
    printf("Enter your selection: ");

    c = getchar();
    printf("%c\n", c);
    switch (c)
    {
        case '1': //start camera ov5640
        {
            ov5640_init();
            start();
            break;
        }
		/*
        case '2': //start camera ov5645
        {
            ov5645_init();
            start();
            break;
        }
		*/
        case '2': //ov5640 take photo
        {
            printf("input the file name,end with Enter\n");
            scanf("%s",fname);
            printf("file name is %s.jpg",fname);
            strcat(fname,".jpg");
            takePhoto = 1;
            ov5640_init();
            start();
            break;
        }
		/*
        case '4': //ov5645 take photo
        {
            printf("input the file name,end with Enter\n");
            scanf("%s",fname);
            printf("file name is %s.jpg",fname);
            strcat(fname,".jpg");
            takePhoto = 1;
            ov5645_init();
            start();
            break;
        }
		*/
        case '3': //quit
        {
            exit(0);
            break;
        }
    }
}



void Stop(int signo)
{
    stop = 1;
}

int main(int argc,char *argv[])
{
    signal(SIGINT, Stop); //* 下面设置三个信号的处理方法

    ion_fd = open("/dev/ion",O_RDWR);
    if (ion_fd < 0) {
        printf("can't open ion!!!\n");
        return -EINVAL;
    }

    menu_usage();

    return 0;
}

