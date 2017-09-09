#include "v4l2.h"
#include "common.h"
#include "common.cpp"

char flag = 0;


int video::init_video()
{

    video_id 	= nxp_v4l2_mlc0_video;

    format = V4L2_PIX_FMT_YUV422P;
    //format = V4L2_PIX_FMT_YUV444;
    //format = V4L2_PIX_FMT_YUYV;
    //format 		= V4L2_PIX_FMT_YUV420M;

    out_index = 0;
    out_dq_index = 0;
    out_q_count = 0;
    started_out = false;

    capture_index = 0;

    ion_fd = open("/dev/ion",O_RDWR);
    if (ion_fd < 0) {
        printf("can't open ion!!!\n");
        return 0;
    }
    if(flag == 0)
    {
        module = 1;
    }
    else
    {
        module = 0;
    }
    width  = WIDTH;
    height = HEIGHT;
    sensor_width = 1024;
    sensor_height = 768;

    struct V4l2UsageScheme s;
    memset(&s, 0, sizeof(s));

    if (module == 0) {
        s.useClipper0   = true;
        s.useDecimator0 = true;
        clipper_id = nxp_v4l2_clipper0;
        sensor_id = nxp_v4l2_sensor0;
    } else {
        s.useClipper1 = true;
        s.useDecimator1 = true;
        clipper_id = nxp_v4l2_clipper1;
        sensor_id = nxp_v4l2_sensor1;
    }
    s.useMlc0Video = true;

    printf("width %d, height %d, module %d\n", width, height, module);

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

    printf("alloc video\n");
    //struct nxp_vid_buffer bufs[MAX_BUFFER_COUNT];
    alloc_buffers(ion_fd, MAX_BUFFER_COUNT, bufs, width, height, format);
    printf("vid_buf: %p, %p, %p, %p\n", bufs[0].virt[0], bufs[1].virt[0], bufs[2].virt[0], bufs[3].virt[0]);

    int i;

    for (i = 0; i < MAX_BUFFER_COUNT; i++) {
        buf = &bufs[i];
        printf("buf plane num: %d\n", buf->plane_num);
        v4l2_qbuf(clipper_id, buf->plane_num, i, buf, -1, NULL);
    }

    v4l2_streamon(clipper_id);

    return 1;

}

void video::video_dqbuf()
{
    buf = &bufs[capture_index];
    v4l2_dqbuf(clipper_id, buf->plane_num, &capture_index, NULL);
}

void video::video_qbuf()
{
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
    v4l2_qbuf(video_id, buf->plane_num, out_index, buf, -1, NULL);
    v4l2_qbuf(clipper_id, buf->plane_num, capture_index, buf, -1, NULL);
}

void video::release_video()
{
    v4l2_streamoff(video_id);
    v4l2_streamoff(clipper_id);
    v4l2_exit();
}

int video::yuvtorgb(int y,int u,int v)
{

     unsigned int pixel24 = 0;
     unsigned char *pixel = (unsigned char *)&pixel24;
     int r, g, b,R,G,B;

     static long int ruv=0, guv=0, buv=0;

     if(sign3)
     {
         sign3 = 0;
         ruv = 1159*(v-128);
         guv = 380*(u-128) + 813*(v-128);
         buv = 2018*(u-128);
     }


     r = (1164*(y-16) + ruv) / 1000;
     g = (1164*(y-16) - guv) / 1000;
     b = (1164*(y-16) + buv) / 1000;

     if(r > 255) r = 255;
     if(g > 255) g = 255;
     if(b > 255) b = 255;
     if(r < 0) r = 0;
     if(g < 0) g = 0;
     if(b < 0) b = 0;

     B = (b >> 3) & 0x001F;
     G = ((g >> 2) << 5) & 0x07E0;
     R = ((r >> 3) << 11) & 0xF800;
     pixel[0] = (char)((R|G|B));
     pixel[1] = (char)((R|G|B)>>8);

     return pixel24;
}

int video::yuvtorgbO( unsigned char *rgb, unsigned int width,unsigned int height)
{
     //unsigned char *yuv = (unsigned char *)(framebuf[buf.index].start);
     unsigned int in, out;
     int y0, u, y1, v;
     unsigned int pixel24=0;
     unsigned char *pixel = (unsigned char *)&pixel24;
     unsigned int size = width * height;
     for(in = 0, out = 0; in < (size / 2); in += 1, out += 4)
     {
          //y0 = yuv[in+0];
          //u  = yuv[in+1];
          //y1 = yuv[in+2];
          //v  = yuv[in+3];
          y0 = *(buf->virt[0] + in * 2 );
          y1 = *(buf->virt[0] + in * 2 + 1);
          u  = *(buf->virt[1] + in);
          v  = *(buf->virt[2] + in);

          sign3 = 1;
          pixel24 =yuvtorgb(y0, u, v);
          rgb[out+0] = pixel[0];    //for QT
          rgb[out+1] = pixel[1];
          //rgb[out+2] = pixel[2];
          //rgb[out+0] = pixel[2];  //for iplimage
          //rgb[out+1] = pixel[1];
          //rgb[out+2] = pixel[0];

          //sign3 = true;
          pixel24 =yuvtorgb(y1, u, v);
          rgb[out+2] = pixel[0];
          rgb[out+3] = pixel[1];
          //rgb[out+5] = pixel[2];
          //rgb[out+3] = pixel[2];
          //rgb[out+4] = pixel[1];
          //rgb[out+5] = pixel[0];
     }

     return 0;
}
