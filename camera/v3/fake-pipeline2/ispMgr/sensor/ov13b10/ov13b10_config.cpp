/*
 * Copyright (c) 2018 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */
#define LOG_TAG "ov13b10Cfg"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

#include "CamHalDebugLog.h"

#include "aml_isp_api.h"

#include "ov13b10x10_sdr_calibration.h"
#include "ov13b10x10_wdr_calibration.h"
#include "ov13b10x36_sdr_calibration.h"
#include "ov13b10x36_wdr_calibration.h"

#include "ov13b10_api.h"

#define MAX_SENSOR_NUM  2

typedef struct
{
    int  enWDRMode;
    ALG_SENSOR_DEFAULT_S snsAlgInfo;
    struct media_entity  * sensor_ent;
} ISP_SNS_STATE_S;

static ISP_SNS_STATE_S   *g_sensorPtr[MAX_SENSOR_NUM];

static int cam_id_check(int id)
{
    if (id >= MAX_SENSOR_NUM || id < 0)
        return -1;
    return 0;
}

void cmos_set_sensor_entity_ov13b10(int ViPipe, struct media_entity * sensor_ent, int wdr, int fps)
{
    if (cam_id_check(ViPipe)) {
        CAMHAL_LOGE("invalid id %d", ViPipe);
        return;
    }

    if (g_sensorPtr[ViPipe]) {
        delete g_sensorPtr[ViPipe];
        g_sensorPtr[ViPipe] = 0;
    }

    g_sensorPtr[ViPipe] = (ISP_SNS_STATE_S*) malloc(sizeof(ISP_SNS_STATE_S));
    if (g_sensorPtr[ViPipe] == 0) {
        CAMHAL_LOGE("new isp sns state obj fail");
        return;
    }

    memset(g_sensorPtr[ViPipe], 0, sizeof(ISP_SNS_STATE_S));
    g_sensorPtr[ViPipe]->sensor_ent = sensor_ent;
    g_sensorPtr[ViPipe]->enWDRMode = wdr;
    //g_sensorPtr[ViPipe]->snsAlgInfo.fps = fps;
}

void cmos_get_sensor_calibration_ov13b10(int ViPipe, struct media_entity *sensor_ent, aisp_calib_info_t *calib)
{
    int32_t address = 0;
    int ret = v4l2_subdev_get_address(sensor_ent, &address);
    if (ret != 0) {
        CAMHAL_LOGE("v4l2_subdev_get_address fail");
    } else {
        if (address == 0x10) {
            CAMHAL_LOGD("sensor address 0x%x", address);
            if (g_sensorPtr[ViPipe]->enWDRMode == 1)
                Ov13b10WdrCalibration2::dynamic_wdr_calibrations_init_ov13b10(calib);
            else
                Ov13b10SdrCalibration2::dynamic_sdr_calibrations_init_ov13b10(calib);
        } else if (address == 0x36) {
            CAMHAL_LOGD("sensor address 0x%x", address);
            if (g_sensorPtr[ViPipe]->enWDRMode == 1)
                Ov13b10WdrCalibration::dynamic_wdr_calibrations_init_ov13b10(calib);
            else
                Ov13b10SdrCalibration::dynamic_sdr_calibrations_init_ov13b10(calib);
        } else {
            CAMHAL_LOGE("invalid sensor address 0x%x", address);
            if (g_sensorPtr[ViPipe]->enWDRMode == 1)
                Ov13b10WdrCalibration::dynamic_wdr_calibrations_init_ov13b10(calib);
            else
                Ov13b10SdrCalibration::dynamic_sdr_calibrations_init_ov13b10(calib);
        }
    }

    return;
}

#if defined(PREVIEW_DEWARP_ENABLE) || defined(PICTURE_DEWARP_ENABLE)
void cmos_get_sensor_gdc_parameter_ov13b10(struct sensorConfig *cfg, GDCInParam in_params,
                                              struct dewarp_params *dewarp_params)
{
    //todo remove to sensor files
    CAMHAL_LOGD("%s: E width %u height %u out width %u height %u",__FUNCTION__, in_params.i_width, in_params.i_height, in_params.o_width, in_params.o_height);
    struct input_param* in   = &dewarp_params->input_param;
    struct output_param* out = &dewarp_params->output_param;
    struct proj_param *proj  = &dewarp_params->proj_param[0];
    struct win_param *win    = &dewarp_params->win_param[0];

    dewarp_params->proc_param.replace_0 = 0;
    dewarp_params->proc_param.replace_1 = 128;
    dewarp_params->proc_param.replace_2 = 128;

    dewarp_params->proc_param.edge_0 = 0;
    dewarp_params->proc_param.edge_1 = 128;
    dewarp_params->proc_param.edge_2 = 128;
    dewarp_params->win_num = 1;
    in->width = in_params.i_width;
    in->height = in_params.i_height;
    in->offset_x = 0;
    in->offset_y = 0;
    in->fov = 120;

    dewarp_params->color_mode = YUV420_SEMIPLANAR;
    /*ROTATION_90 ROTATION_270 output need exchange width and height,input no need*/
    out->width = in_params.o_width;
    out->height = in_params.o_height;
    if (property_get_bool("vendor.camhal.use.dewarp.linear", true)) {
        proj[0].projection_mode = PROJ_MODE_LINEAR;
    } else {
        proj[0].projection_mode = PROJ_MODE_EQUIDISTANCE;
    }
    proj[0].pan = 0;
    proj[0].tilt = 0;
    proj[0].rotation = (int)in_params.rotation*90;
    proj[0].zoom = 1.01;
    proj[0].strength_hor = 1.0;
    proj[0].strength_ver = 1.0;

    win[0].win_start_x = 0;
    win[0].win_end_x = in_params.o_width - 1;
    win[0].win_start_y = 0;
    win[0].win_end_y = in_params.o_height - 1;
    win[0].img_start_x = 0;
    win[0].img_end_x = in_params.o_width - 1;
    win[0].img_start_y = 0;
    win[0].img_end_y = in_params.o_height - 1;
    win[0].mesh_x_len = 32;
    win[0].mesh_y_len = 32;

    dewarp_params->tile_x_step = 16;
    dewarp_params->tile_y_step = 16;
    dewarp_params->prm_mode = 0;
}
#endif

void cmos_clean_up_ov13b10(int ViPipe)
{
    if (cam_id_check(ViPipe)) {
        CAMHAL_LOGE("invalid id %d", ViPipe);
        return;
    }

    if (g_sensorPtr[ViPipe]) {
        free(g_sensorPtr[ViPipe]);
        g_sensorPtr[ViPipe] = 0;
    }
}

int cmos_get_ae_default_ov13b10(int ViPipe, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    CAMHAL_LOGD("cmos_get_ae_default\n");

    g_sensorPtr[ViPipe]->snsAlgInfo.active.width = 4208;
    g_sensorPtr[ViPipe]->snsAlgInfo.active.height = 3120;
    g_sensorPtr[ViPipe]->snsAlgInfo.fps = 30 * 256;
    g_sensorPtr[ViPipe]->snsAlgInfo.sensor_exp_number = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.bits = 10;

    //g_sensorPtr[ViPipe]->snsAlgInfo.sensor_gain_number = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.total.width = 0x498;
    g_sensorPtr[ViPipe]->snsAlgInfo.total.height = 0x0CC0;

    g_sensorPtr[ViPipe]->snsAlgInfo.lines_per_second = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height-8 )* g_sensorPtr[ViPipe]->snsAlgInfo.fps/256;
    g_sensorPtr[ViPipe]->snsAlgInfo.pixels_per_line = g_sensorPtr[ViPipe]->snsAlgInfo.total.width;

    if (g_sensorPtr[ViPipe]->enWDRMode == 1) {

        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 4 << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8)<< SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;

    } else {
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 4 << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
    }

    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_log2_max = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_log2_max = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_accuracy_fmt = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_accuracy = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_accuracy_fmt = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_accuracy = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_log2_max = 16179; //<< LOG2_GAIN_SHIFT;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_log2_max = 16179; //<< LOG2_GAIN_SHIFT; //2^3.95 = 15.45
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_accuracy_fmt = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_accuracy_fmt = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_log2 = 0x0<< LOG2_GAIN_SHIFT;
    g_sensorPtr[ViPipe]->snsAlgInfo.expos_lines = (0x3D2<<(LOG2_GAIN_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.again_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    g_sensorPtr[ViPipe]->snsAlgInfo.expos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.vsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.vvsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));

    g_sensorPtr[ViPipe]->snsAlgInfo.gain_apply_delay = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay = 0;
    CAMHAL_LOGD("cmos_get_ae_default++++++\n");

    memcpy(pstAeSnsDft, &g_sensorPtr[ViPipe]->snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));

    return 0;
}

static int aisp_math_exp2( int64_t val, int32_t shift_in, int32_t shift_out )
{
    uint32_t fract_part = (((uint32_t)val) & ( ( 1 << shift_in ) - 1 ) );
    uint32_t int_part = ((uint32_t)val) >> shift_in;
    uint32_t res, tmp;
    uint32_t pow_lut[33] = {
    1073741824, 1097253708, 1121280436, 1145833280, 1170923762, 1196563654, 1222764986, 1249540052,
    1276901417, 1304861917, 1333434672, 1362633090, 1392470869, 1422962010, 1454120821, 1485961921,
    1518500250, 1551751076, 1585730000, 1620452965, 1655936265, 1692196547, 1729250827, 1767116489,
    1805811301, 1845353420, 1885761398, 1927054196, 1969251188, 2012372174, 2056437387, 2101467502,
    2147483648};
    if ( shift_in <= 5 ) {
        uint32_t lut_index = fract_part << ( 5 - shift_in );
        res = pow_lut[lut_index] >> ( 30 - shift_out - int_part );
        return res;
    } else {
        uint32_t lut_index = fract_part >> ( shift_in - 5 );
        uint32_t lut_fract = fract_part & ( ( 1 << ( shift_in - 5 ) ) - 1 );
        uint32_t a = pow_lut[lut_index];
        uint32_t b = pow_lut[lut_index + 1];
        res = ( (uint64_t)( b - a ) * lut_fract ) >> ( shift_in - 5 );
        tmp =  ( 30 - shift_out - int_part ) - 1;
        res = ( res + a + (1<<tmp) ) >> ( 30 - shift_out - int_part );

        return ((int64_t)res);
    }
}

void cmos_again_calc_table_ov13b10(int ViPipe, uint32_t *pu32AgainLin, uint32_t *pu32AgainDb)
{
    //CAMHAL_LOGD("cmos_again_calc_table: %d, %d\n", *pu32AgainLin, *pu32AgainDb);
    uint32_t again_reg;

    again_reg = aisp_math_exp2( *pu32AgainLin, SHUTTER_TIME_SHIFT, 8 );

    if (again_reg > 0x7fff) {
        again_reg = 0x7fff;
    }

    if (g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[0] != again_reg) {
        g_sensorPtr[ViPipe]->snsAlgInfo.u16GainCnt = g_sensorPtr[ViPipe]->snsAlgInfo.gain_apply_delay + 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[0] = again_reg;
    }

}

void cmos_dgain_calc_table_ov13b10(int ViPipe, uint32_t *pu32DgainLin, uint32_t *pu32DgainDb)
{
    //CAMHAL_LOGD("cmos_dgain_calc_table: %d, %d\n", *pu32DgainLin, *pu32DgainDb);
}

void cmos_inttime_calc_table_ov13b10(int ViPipe, uint32_t pu32ExpL, uint32_t pu32ExpS, uint32_t pu32ExpVS, uint32_t pu32ExpVVS)
{
    //CAMHAL_LOGD("cmos_inttime_calc_table: %d, %d, %d, %d\n", pu32ExpL, pu32ExpS, pu32ExpVS, pu32ExpVVS);
    uint32_t shutter_time_lines = pu32ExpL >> SHUTTER_TIME_SHIFT;

    uint32_t shutter_time_lines_short = pu32ExpS >> SHUTTER_TIME_SHIFT;
    //CAMHAL_LOGD("init times = %d  \n",shutter_time_lines);
    if (g_sensorPtr[ViPipe]->enWDRMode == 0) {
        if (shutter_time_lines > g_sensorPtr[ViPipe]->snsAlgInfo.total.height )
            shutter_time_lines = g_sensorPtr[ViPipe]->snsAlgInfo.total.height;

        if (shutter_time_lines < 4)
            shutter_time_lines = 4;
    } else {
        if (shutter_time_lines_short < 4)
            shutter_time_lines_short = 4;
    }

    if (g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0] != shutter_time_lines || g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][0] != shutter_time_lines_short) {
        g_sensorPtr[ViPipe]->snsAlgInfo.u16IntTimeCnt = g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay + 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0] = shutter_time_lines;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][0] = shutter_time_lines_short;
    }
}

void cmos_fps_set_ov13b10(int ViPipe, float f32Fps, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    //CAMHAL_LOGD("cmos_fps_set: %f\n", f32Fps);
    struct v4l2_ext_control fpsCtrl;

    fpsCtrl.id = V4L2_CID_AML_ORIG_FPS;
    fpsCtrl.value = (int32_t)(f32Fps / 256);
    //CAMHAL_LOGD("--ov13b10-- fpsCtrl.value = %d\n",fpsCtrl.value);

    g_sensorPtr[ViPipe]->snsAlgInfo.total.height = ( 0x0CC0 * 30 )/fpsCtrl.value;

    g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8 ) << SHUTTER_TIME_SHIFT;
    g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8 ) << SHUTTER_TIME_SHIFT;
    g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 8 ) << SHUTTER_TIME_SHIFT;
    g_sensorPtr[ViPipe]->snsAlgInfo.lines_per_second = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height -8) * fpsCtrl.value;
    memcpy(pstAeSnsDft, &g_sensorPtr[ViPipe]->snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));

    v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &fpsCtrl, 1);
}

void cmos_alg_update_ov13b10(int ViPipe)
{
    uint32_t shutter_time_lines = 0;//, shutter_time_lines_short = 0;
    uint32_t i = 0;

    if ( g_sensorPtr[ViPipe]->snsAlgInfo.u16GainCnt || g_sensorPtr[ViPipe]->snsAlgInfo.u16IntTimeCnt ) {
        if ( g_sensorPtr[ViPipe]->snsAlgInfo.u16GainCnt ) {
            g_sensorPtr[ViPipe]->snsAlgInfo.u16GainCnt--;
            struct v4l2_ext_control gain;
            gain.id = V4L2_CID_GAIN;
            gain.value = g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[g_sensorPtr[ViPipe]->snsAlgInfo.gain_apply_delay];
            v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &gain, 1);
        }

        // -------- Integration Time ----------
        if ( g_sensorPtr[ViPipe]->snsAlgInfo.u16IntTimeCnt ) {
            g_sensorPtr[ViPipe]->snsAlgInfo.u16IntTimeCnt--;
            //shutter_time_lines = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay];
            shutter_time_lines = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0];
            if (g_sensorPtr[ViPipe]->enWDRMode == 0) {
                struct v4l2_ext_control expo;
                expo.id = V4L2_CID_EXPOSURE;
                expo.value = shutter_time_lines;
                v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &expo, 1);
            }

            if (g_sensorPtr[ViPipe]->enWDRMode) {
                //shutter_time_lines_short = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay];
                //ov13b10_write_register(ViPipe, 0x3020, shutter_time_lines_short & 0xff);
                //ov13b10_write_register(ViPipe, 0x3021, (shutter_time_lines_short>>8) & 0xff);
                //ov13b10_write_register(ViPipe, 0x3024, shutter_time_lines&0xff);
                //ov13b10_write_register(ViPipe, 0x3025, (shutter_time_lines>>8) & 0xff);
                //CAMHAL_LOGD("sensor expo: %d, %d\n", shutter_time_lines, shutter_time_lines_short);
            }
        }
    }

    for ( i = 3; i > 0; i --) {
        g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[i - 1];
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][i - 1];
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][i - 1];
    }

}
