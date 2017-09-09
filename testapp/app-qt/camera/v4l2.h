#ifndef V4L2_H
#define V4L2_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

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
#include <QBuffer>
#include <QImage>
#include <QByteArray>
#include <QCoreApplication>
#include <QTime>
#include <QThread>
#include <common.h>

class video
{
private:
    int fd;
    char sign3;

public:
    int ion_fd;
    int width;
    int height;
    int sensor_width;
    int sensor_height;
    int module;
    int clipper_id;
    int sensor_id;
    int video_id;

    int format;

    int out_index;
    int out_dq_index;
    int out_q_count;
    bool started_out;

    int capture_index;
    int count;

    struct nxp_vid_buffer bufs[MAX_BUFFER_COUNT];
    struct nxp_vid_buffer *buf;
    int init_video();
    void video_dqbuf();
    void video_qbuf();
    void release_video();
    int yuvtorgbO(unsigned char *rgb, unsigned int width,unsigned int height);
    int yuvtorgb(int y,int u,int v);

    //struct v4l2_buffer buf;
};


#endif // V4L2_H
