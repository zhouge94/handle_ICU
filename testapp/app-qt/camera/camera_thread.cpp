#include "camera_thread.h"
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QDebug>
#include <common.h>

video_thread::video_thread():QThread()
{
    quit_flag = false;
    needCapture = 0;
}

video_thread::~video_thread()
{
    this->quit();
    quit_flag = true;
    this->wait();
}

void video_thread::run()
{
    m_video.init_video();//打开视频设备
    unsigned char *rgb0 = new unsigned char [WIDTH * HEIGHT * 2];//根据yuyv像素点和rgb像素所站内存比分配rgb

    while(!quit_flag)
    {
        m_video.video_dqbuf();
        printf("capture_index %d\n",m_video.capture_index);
        if(needCapture == 1){   //normal m2m,if click capture button,yuv 2 rgb then save photo
            //if(m_video.capture_index == 0){
                m_video.yuvtorgbO(rgb0, WIDTH, HEIGHT);//yuyv转换成RGB
                printf("m_video.yuvtorgbO OK\n");
                emit image_data(rgb0);

                needCapture = 0;
            //}

        }
        m_video.video_qbuf();
   }
    m_video.release_video();//在线程释放前释放设备
}

void video_thread::capture()
{
    printf("need capture ==1\n");
    needCapture = 1;
}
