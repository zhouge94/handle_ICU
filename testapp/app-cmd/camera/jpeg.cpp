extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "jpeglib.h"
}

#include <nxp-v4l2.h>

#define JPEG_QUALITY 100 //图片质量
#define uchar unsigned char
int savejpg(uchar *pdata, char *jpg_file, int width, int height)
{  //分别为RGB数据，要保存的jpg文件名，图片长宽
    int depth = 3;
    JSAMPROW row_pointer[1];//指向一行图像数据的指针
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    cinfo.err = jpeg_std_error(&jerr);//要首先初始化错误信息
    //* Now we can initialize the JPEG compression object.
    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(jpg_file, "wb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", jpg_file);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;             //* image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = depth;    //* # of color components per pixel
    cinfo.in_color_space = JCS_RGB;     //* colorspace of input image
    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE ); //* limit to baseline-JPEG values
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height)
           {
            row_pointer[0] = (JSAMPROW)(pdata + cinfo.next_scanline * row_stride);//一行一行数据的传，jpeg为大端数据格式
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);//这几个函数都是固定流程
    fclose(outfile);

    return 0;
}

int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
        unsigned int pixel32 = 0;
        unsigned char *pixel = (unsigned char *)&pixel32;
        int r, g, b;
        r = y + (1.370705 * (v-128));
        g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
        b = y + (1.732446 * (u-128));
        if(r > 255) r = 255;
        if(g > 255) g = 255;
        if(b > 255) b = 255;
        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;
        pixel[0] = r ;
        pixel[1] = g ;
        pixel[2] = b ;
        return pixel32;
}

int yuvtorgb( unsigned char *rgb, unsigned int width, unsigned int height, struct nxp_vid_buffer *buf)
{
    unsigned int in, out = 0;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height / 2; in += 1)
    {

        y0 = *(buf->virt[0] + in * 2 );
        y1 = *(buf->virt[0] + in * 2 + 1);
        u  = *(buf->virt[1] + in);
        v  = *(buf->virt[2] + in);

        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}
