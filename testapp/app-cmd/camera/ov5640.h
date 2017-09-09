#ifndef _OV5640_H
#define _OV5640_H

typedef enum {
    nxp_v4l2_sensor0        = 0,
    nxp_v4l2_sensor1        = 1,
    nxp_v4l2_mipicsi        = 2,
    nxp_v4l2_clipper0       = 5, /* include camera sensor, mipicsi */
    nxp_v4l2_clipper1       = 8,
    nxp_v4l2_decimator0     = 11,
    nxp_v4l2_decimator1     = 14,
    nxp_v4l2_scaler         = 17,
    nxp_v4l2_deinterlacer   = 20,
    nxp_v4l2_mlc0           = 21,
    nxp_v4l2_mlc0_rgb       = 23,
    nxp_v4l2_mlc0_video     = 25,
    nxp_v4l2_mlc1           = 26,
    nxp_v4l2_mlc1_rgb       = 28,
    nxp_v4l2_mlc1_video     = 30,
    nxp_v4l2_resol          = 31,
    nxp_v4l2_hdmi           = 32,
    nxp_v4l2_tvout          = 33,
    nxp_v4l2_id_max,
} nxp_v4l2_id;





#endif
