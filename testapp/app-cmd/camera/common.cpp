//#define DEFAULT_WIDTH  640
//#define DEFAULT_HEIGHT 480
#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600

#define MAX_BUFFER_COUNT 4

#ifndef ALIGN
#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))
#endif

#define YUV_STRIDE_ALIGN_FACTOR     64
#define YUV_VSTRIDE_ALIGN_FACTOR    16

#define YUV_STRIDE(w)    ALIGN(w, YUV_STRIDE_ALIGN_FACTOR)
#define YUV_YSTRIDE(w)   (ALIGN(w/2, YUV_STRIDE_ALIGN_FACTOR) * 2)
#define YUV_VSTRIDE(h)   ALIGN(h, YUV_VSTRIDE_ALIGN_FACTOR)

#define CHECK_COMMAND(command) do { \
        int ret = command; \
        if (ret < 0) { \
            fprintf(stderr, "line %d error!!!\n", __LINE__); \
            return ret; \
        } \
    } while (0)


/*
#ifdef ANDROID
#include <ion/ion.h>
#else
#include "ion.h"
#endif
*/
#ifndef ANDROID
#define ALOGE(x...) fprintf(stderr, x)
#define ALOGD(x...) fprintf(stdout, x)
#endif

#include <ion.h>

int ion_alloc(int fd, size_t len, size_t align, unsigned int heap_mask,
        unsigned int flags, struct ion_handle **handle)
{
    int ret;
    struct ion_allocation_data data;
    data.len = len;
    data.align = align;
    data.heap_id_mask = heap_mask;
    data.flags = flags;

    ret = ioctl(fd, ION_IOC_ALLOC, &data);
    if (ret < 0)
        return ret;
    *handle = data.handle;
    return ret;
}

int ion_free(int fd, struct ion_handle *handle)
{
    struct ion_handle_data data;
    data.handle = handle;

    return ioctl(fd, ION_IOC_FREE, &data);
}

int ion_share(int fd, struct ion_handle *handle, int *share_fd)
{
    struct ion_fd_data data;
    data.handle = handle;
    int ret = ioctl(fd, ION_IOC_SHARE, &data);
    if (ret < 0)
        return ret;
    *share_fd = data.fd;
    if (*share_fd < 0) {
        ALOGE("share ioctl returned negative fd\n");
        return -EINVAL;
    }
    return ret;
}

int ion_alloc_fd(int fd, size_t len, size_t align, unsigned int heap_mask,
        unsigned int flags, int *handle_fd) {
    struct ion_handle *handle;
    int ret;

    ret = ion_alloc(fd, len, align, heap_mask, flags, &handle);
    if (ret < 0)
        return ret;
    ret = ion_share(fd, handle, handle_fd);
    ion_free(fd, handle);
    return ret;
}


unsigned int get_size(int format, int num, int width, int height)
{
    int size;

    switch (format) {
    case V4L2_PIX_FMT_YUYV:
        if (num > 0) return 0;
        size = (width * height) * 2;
        break;
    case V4L2_PIX_FMT_YUV420M:
#if 0
        if (num == 0) {
            size = width * height;
        } else {
            // size = (width * height) >> 2;
            size = (width * height) >> 1;
            // size = (width * height);
        }
#else
        if (num == 0) {
            // y
            size = YUV_YSTRIDE(width) * YUV_VSTRIDE(height);
        } else {
            // cb, cr
            size = YUV_STRIDE(width/2) * YUV_VSTRIDE(height/2);
        }
#endif
        break;
    case V4L2_PIX_FMT_YUV422P:
        if (num == 0) {
            size = width * height;
        } else {
            // size = (width * height) >> 1;
            size = (width * height) >> 1;
        }
        break;
    case V4L2_PIX_FMT_YUV444:
        size = width * height;
        break;
    default:
        size = width * height * 2;
        break;
    }

    return size;
}

int ion_get_phys(int fd, int buf_fd, unsigned long *phys)
{
    int ret;
    struct ion_custom_data data;
    struct nxp_ion_physical custom_data;

    custom_data.ion_buffer_fd = buf_fd;
    data.cmd = NXP_ION_GET_PHY_ADDR;
    data.arg = (unsigned long)&custom_data;

    ret = ioctl(fd, ION_IOC_CUSTOM, &data);
    if (ret) {
        printf("%s error: failed ION_IOC_CUSTOM\n", __func__);
        return ret;
    }
    *phys = custom_data.phys;
    return 0;
}

int alloc_buffers(int ion_fd, int count, struct nxp_vid_buffer *bufs, int width, int height, int format)
{
    int ret;
    //int size;
    int i, j;
    //int y_size = width * height;
    //int cb_size;
    //int cr_size;
    struct nxp_vid_buffer *buffer;
    int plane_num;

    if (format == V4L2_PIX_FMT_YUYV || format == V4L2_PIX_FMT_RGB565) {
        plane_num = 1;
    } else {
        plane_num = 3;
    }

    //printf("Allocation Buffer: count(%d), plane(%d)\n", count, plane_num);
    for (i = 0; i < count; i++) {
        buffer = &bufs[i];
        //printf("[Buffer %d] --->\n", i);
        for (j = 0; j < plane_num; j++) {
            buffer->sizes[j] = get_size(format, j, width, height);
            ret = ion_alloc_fd(ion_fd, (size_t)buffer->sizes[j], (size_t)0, ION_HEAP_NXP_CONTIG_MASK, 0, &buffer->fds[j]);
            if (ret < 0) {
                fprintf(stderr, "failed to ion_alloc_fd()\n");
                return ret;
            }
            buffer->virt[j] = (char *)mmap(NULL, buffer->sizes[j], PROT_READ | PROT_WRITE, MAP_SHARED, buffer->fds[j], 0);
            if (!buffer->virt[j]) {
                fprintf(stderr, "failed to mmap\n");
                return ret;
            }
            ret = ion_get_phys(ion_fd, buffer->fds[j], &buffer->phys[j]);
            if (ret < 0) {
                fprintf(stderr, "failed to get phys\n");
                return ret;
            }
            buffer->plane_num = plane_num;
            //printf("\tplane %d: fd(%d), size(%d), phys(0x%x), virt(0x%x)\n",
                    //j, buffer->fds[j], buffer->sizes[j], buffer->phys[j], buffer->virt[j]);
        }
    }

    return 0;
}
