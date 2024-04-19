/*
 * Copyright (c) 2018 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */
#define LOG_TAG "imx290Cfg"

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
#include <cutils/properties.h>

#include "CamHalDebugLog.h"

#include "aml_isp_api.h"

#include "imx290_sdr_calibration.h"
#include "imx290_wdr_calibration.h"
#include "imx290_api.h"

#include "camera_data_saver.h"

#define MAX_SENSOR_NUM  2

typedef struct
{
    int  enWDRMode = 0;

    uint32_t  again_high_alg_value; // also for sdr again_reg_value;
    uint32_t  again_low_alg_value;
    uint32_t  inttime_long_alg_value; // also for sdr inttime_reg_value;
    uint32_t  inttime_short_alg_value;

    ALG_SENSOR_DEFAULT_S snsAlgInfo;
    struct media_entity  * sensor_ent;
} ISP_SNS_STATE_S;

static ISP_SNS_STATE_S *g_sensorPtr[MAX_SENSOR_NUM];

static int cam_id_check(int id)
{
    if (id >= MAX_SENSOR_NUM || id < 0)
        return -1;
    return 0;
}

void cmos_set_sensor_entity_imx290(int ViPipe, struct media_entity * sensor_ent, int wdr, int fps)
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

    g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[0] = 0xffff;
    g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0] = 0xffff;
    g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][0] = 0xffff;

    // now only support 30fps sdr and wdr;
    //g_sensorPtr[ViPipe]->snsAlgInfo.fps = fps;
}

#if defined(PREVIEW_DEWARP_ENABLE) || defined(PICTURE_DEWARP_ENABLE)
void cmos_get_sensor_gdc_parameter_imx290(struct sensorConfig *cfg, GDCInParam in_params,
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

void cmos_get_sensor_calibration_imx290(int ViPipe, struct media_entity *sensor_ent, aisp_calib_info_t *calib)
{
    if (g_sensorPtr[ViPipe]->enWDRMode == 1)
        Imx290WdrCalibration::dynamic_wdr_calibrations_init_imx290(calib);
    else
        Imx290SdrCalibration::dynamic_sdr_calibrations_init_imx290(calib);
}

void cmos_clean_up_imx290(int ViPipe)
{
    if (cam_id_check(ViPipe)) {
        CAMHAL_LOGE("invalid id %d", ViPipe);
        return;
    }

    if (property_get_bool("vendor.camhal.mipi.save_and_use_3a", false)) {
        android::CameraDataSaver::getInstance()->save(ViPipe, SENSOR_AGAIN_HIGH, (uint8_t *)&g_sensorPtr[ViPipe]->again_high_alg_value,  sizeof(uint32_t));
        android::CameraDataSaver::getInstance()->save(ViPipe, SENSOR_AGAIN_LOW, (uint8_t *)&g_sensorPtr[ViPipe]->again_low_alg_value,  sizeof(uint32_t));

        android::CameraDataSaver::getInstance()->save(ViPipe, SENSOR_EXPOSURE_LONG, (uint8_t *)&g_sensorPtr[ViPipe]->inttime_long_alg_value, sizeof(uint32_t));
        android::CameraDataSaver::getInstance()->save(ViPipe, SENSOR_EXPOSURE_SHORT, (uint8_t *)&g_sensorPtr[ViPipe]->inttime_short_alg_value,  sizeof(uint32_t));
        CAMHAL_LOGD("save again high %d low %d; exp long %d short %d",
           g_sensorPtr[ViPipe]->again_high_alg_value, g_sensorPtr[ViPipe]->again_low_alg_value,
           g_sensorPtr[ViPipe]->inttime_long_alg_value, g_sensorPtr[ViPipe]->inttime_short_alg_value);
    }

    if (g_sensorPtr[ViPipe]) {
        free(g_sensorPtr[ViPipe]);
        g_sensorPtr[ViPipe] = 0;
    }

}

int cmos_get_ae_default_imx290(int ViPipe, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    CAMHAL_LOGD("cmos_get_ae_default\n");

    // default initial value is for SDR;
    uint32_t again_high_alg_value = (0x02 << LOG2_GAIN_SHIFT);
    uint32_t inttime_sdr_alg_value = 0;

    uint32_t inttime_wdr_long_alg_value = 0;
    uint32_t inttime_wdr_short_alg_value = 0;

    g_sensorPtr[ViPipe]->snsAlgInfo.active.width = 1920;
    g_sensorPtr[ViPipe]->snsAlgInfo.active.height = 1080;

    // now only support 30fps sdr and wdr;
    g_sensorPtr[ViPipe]->snsAlgInfo.fps = 30*256;

    g_sensorPtr[ViPipe]->snsAlgInfo.sensor_gain_number = 1;

    if (g_sensorPtr[ViPipe]->enWDRMode == 1) {
        // wdr mode
        g_sensorPtr[ViPipe]->snsAlgInfo.sensor_exp_number = 2;
        g_sensorPtr[ViPipe]->snsAlgInfo.bits = 10;
        g_sensorPtr[ViPipe]->snsAlgInfo.total.width = 2028; // sync with HMAX 0x07EC = 2028
        g_sensorPtr[ViPipe]->snsAlgInfo.total.height = 1220; // sync with VMAX 0x04C4 = 1220
        g_sensorPtr[ViPipe]->snsAlgInfo.lines_per_second = g_sensorPtr[ViPipe]->snsAlgInfo.total.height * g_sensorPtr[ViPipe]->snsAlgInfo.fps / 256;
        g_sensorPtr[ViPipe]->snsAlgInfo.pixels_per_line = g_sensorPtr[ViPipe]->snsAlgInfo.total.width;

        // min exposure lines. short exposure min lines.
        // short exposure lines = RHS1 - (SHS1 + 1);
        // max SHS1 is SHS1 - 2; RHS1 - ((SHS1 - 2) + 1) = 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 1<<SHUTTER_TIME_SHIFT;

        // max exposure lines for short exposure frame.
        // SHS1 min value is 2; RHS1 is fixed to 205 (0xcd)
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (205 - 3) << SHUTTER_TIME_SHIFT;

        // long exposure lines: FSC - (SHS2 + 1)
        // min SHS2 is RHS1 + 2;
        // FSC - (SHS2 + 1) = FSC - ((RHS1 + 2) + 1)
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height*2 - (205 + 3)) << SHUTTER_TIME_SHIFT;

        // same as integration_time_max
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (205 - 3)<<SHUTTER_TIME_SHIFT;

        inttime_wdr_long_alg_value = ((2 * g_sensorPtr[ViPipe]->snsAlgInfo.total.height - (0x453 + 1))<< SHUTTER_TIME_SHIFT);
        inttime_wdr_short_alg_value = ((205 - (0x02 + 1)) << SHUTTER_TIME_SHIFT);

        if (property_get_bool("vendor.camhal.mipi.save_and_use_3a", false)) {
            // load from camera data saver. if load fail (for the first time after power on)
            // use the INIT_WDR_XX_REG_VALUE
            android::CameraDataSaver::getInstance()->load(ViPipe, SENSOR_AGAIN_HIGH, (uint8_t *)&again_high_alg_value, sizeof(uint32_t));
            android::CameraDataSaver::getInstance()->load(ViPipe, SENSOR_EXPOSURE_LONG, (uint8_t *)&inttime_wdr_long_alg_value, sizeof(uint32_t));
            android::CameraDataSaver::getInstance()->load(ViPipe, SENSOR_EXPOSURE_SHORT, (uint8_t *)&inttime_wdr_short_alg_value, sizeof(uint32_t));
        }
    } else {
        g_sensorPtr[ViPipe]->snsAlgInfo.sensor_exp_number = 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.bits = 12;
        g_sensorPtr[ViPipe]->snsAlgInfo.total.width = 4400; // should match sensor hmax register[0x301a-0x3018]
        g_sensorPtr[ViPipe]->snsAlgInfo.total.height = 1157; // should match sensor vmax register[0x301d-0x301c]
        g_sensorPtr[ViPipe]->snsAlgInfo.lines_per_second = g_sensorPtr[ViPipe]->snsAlgInfo.total.height * g_sensorPtr[ViPipe]->snsAlgInfo.fps / 256;
        g_sensorPtr[ViPipe]->snsAlgInfo.pixels_per_line = g_sensorPtr[ViPipe]->snsAlgInfo.total.width;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 1<<SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 2)<<SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 2)<<SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - 2)<<SHUTTER_TIME_SHIFT;

        inttime_sdr_alg_value = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height - (0x0181 + 1))<< SHUTTER_TIME_SHIFT;;

        if (property_get_bool("vendor.camhal.mipi.save_and_use_3a", false)) {
            // load from camera data saver. if load fail (for the first time after power on)
            // keep use the initial value.
            android::CameraDataSaver::getInstance()->load(ViPipe, SENSOR_AGAIN_HIGH, (uint8_t *)&again_high_alg_value, sizeof(uint32_t));
            android::CameraDataSaver::getInstance()->load(ViPipe, SENSOR_EXPOSURE_LONG, (uint8_t *)&inttime_sdr_alg_value, sizeof(uint32_t));
        }
    }

    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_log2 = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_log2_max = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_log2_max = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_accuracy_fmt = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_high_accuracy = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_accuracy_fmt = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.dgain_accuracy = 1;

    g_sensorPtr[ViPipe]->snsAlgInfo.again_log2_max = (72/6)<<(LOG2_GAIN_SHIFT);
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_log2_max = (72/6)<<(LOG2_GAIN_SHIFT);
    // again reg initial value is 0x02;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_log2 = again_high_alg_value;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_log2 = again_high_alg_value;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_accuracy_fmt = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_high_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_accuracy_fmt = 1;
    g_sensorPtr[ViPipe]->snsAlgInfo.again_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    if (g_sensorPtr[ViPipe]->enWDRMode == 1) {
        // calc from initial value; 2 * vmax - (SHS2 + 1);
        g_sensorPtr[ViPipe]->snsAlgInfo.expos_lines = inttime_wdr_long_alg_value;
        g_sensorPtr[ViPipe]->snsAlgInfo.expos_accuracy = (1<<(SHUTTER_TIME_SHIFT));

        // calc from initial value; RHS1 - (SHS1 + 1)
        g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_lines = inttime_wdr_short_alg_value;
        g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
        CAMHAL_LOGI("exp lines %d, short exp lined %d", (g_sensorPtr[ViPipe]->snsAlgInfo.expos_lines>>SHUTTER_TIME_SHIFT),
                (g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_lines>>SHUTTER_TIME_SHIFT));

    } else {
        // calc from initial value: vmax - (SHS1 + 1)
        g_sensorPtr[ViPipe]->snsAlgInfo.expos_lines = inttime_sdr_alg_value;
        g_sensorPtr[ViPipe]->snsAlgInfo.expos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
        g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_lines = (1<<(SHUTTER_TIME_SHIFT));
        g_sensorPtr[ViPipe]->snsAlgInfo.sexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    }
    g_sensorPtr[ViPipe]->snsAlgInfo.vsexpos_lines = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.vsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.vvsexpos_lines = (1<<(SHUTTER_TIME_SHIFT));
    g_sensorPtr[ViPipe]->snsAlgInfo.vvsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));

    g_sensorPtr[ViPipe]->snsAlgInfo.gain_apply_delay = 0;
    g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay = 0;
    CAMHAL_LOGD("cmos_get_ae_default++++++\n");

    memcpy(pstAeSnsDft, &g_sensorPtr[ViPipe]->snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));
    // set initial again and inttime reg value to sensor regs.
    {
        cmos_again_calc_table_imx290(ViPipe, &again_high_alg_value, &again_high_alg_value);

        if (g_sensorPtr[ViPipe]->enWDRMode == 1) {
            cmos_inttime_calc_table_imx290(ViPipe, inttime_wdr_long_alg_value, inttime_wdr_short_alg_value, 8, 8);
        } else {
            cmos_inttime_calc_table_imx290(ViPipe, inttime_sdr_alg_value, 8, 8, 8);
        }
        cmos_alg_update_imx290(ViPipe);
    }

    return 0;
}

void cmos_again_calc_table_imx290(int ViPipe, uint32_t  *sns_hc_again, uint32_t *sns_again)
{
    //CAMHAL_LOGD("cmos_again_calc_table: %d, %d\n", *sns_hc_again, *sns_again);
    uint32_t again_reg;
    uint32_t u32AgainDb;

    g_sensorPtr[ViPipe]->again_high_alg_value = *sns_hc_again;
    g_sensorPtr[ViPipe]->again_low_alg_value = *sns_again;

    u32AgainDb = *sns_again;
    u32AgainDb = ((u32AgainDb*20)>>LOG2_GAIN_SHIFT);

    again_reg = (uint32_t)(u32AgainDb);
    if (again_reg > 720/3) //72dB, 0.3dB step.
        again_reg = 720/3;

    if (g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[0] != again_reg) {
        g_sensorPtr[ViPipe]->snsAlgInfo.u16GainCnt = g_sensorPtr[ViPipe]->snsAlgInfo.gain_apply_delay + 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[0] = again_reg;
    }

}

void cmos_dgain_calc_table_imx290(int ViPipe, uint32_t *pu32DgainLin, uint32_t *pu32DgainDb)
{
    //CAMHAL_LOGD("cmos_dgain_calc_table: %d, %d\n", *pu32DgainLin, *pu32DgainDb);
}

void cmos_inttime_calc_table_imx290(int ViPipe, uint32_t pu32ExpL, uint32_t pu32ExpS, uint32_t pu32ExpVS, uint32_t pu32ExpVVS)
{
    //CAMHAL_LOGD("cmos_inttime_calc_table: %d, %d, %d, %d\n", pu32ExpL, pu32ExpS, pu32ExpVS, pu32ExpVVS);
    uint32_t shutter_time_lines = pu32ExpL >> SHUTTER_TIME_SHIFT;
    uint32_t shutter_time_line_each_frame = g_sensorPtr[ViPipe]->snsAlgInfo.total.height;

    uint32_t shutter_time_lines_short = pu32ExpS >> SHUTTER_TIME_SHIFT;

    g_sensorPtr[ViPipe]->inttime_long_alg_value = pu32ExpL;
    g_sensorPtr[ViPipe]->inttime_short_alg_value = pu32ExpS;

    //CAMHAL_LOGD("expo: %d, %d\n", shutter_time_lines, shutter_time_lines_short);
    if (g_sensorPtr[ViPipe]->enWDRMode == 0) {
        // sdr mode;
        shutter_time_lines = shutter_time_line_each_frame - shutter_time_lines - 1;

        // now shutter_time_lines is reg value.
        if (shutter_time_lines < 1)
            shutter_time_lines = 1;

        if (shutter_time_lines > (shutter_time_line_each_frame - 2))
            shutter_time_lines = (shutter_time_line_each_frame - 2);

    } else {
        shutter_time_lines_short = 205 - shutter_time_lines_short - 1;

        // now shutter_time_lines_short is reg value.
        if (shutter_time_lines_short < 2)
            shutter_time_lines_short = 2;

        if (shutter_time_lines_short > (205 - 2))
            shutter_time_lines_short = (205 - 2);

        shutter_time_lines = shutter_time_line_each_frame * 2  - shutter_time_lines - 1;

        // now shutter_time_lines is reg value.
        if (shutter_time_lines < (205 + 2))
            shutter_time_lines = (205 + 2);

        if (shutter_time_lines > (shutter_time_line_each_frame * 2 - 2))
            shutter_time_lines = (shutter_time_line_each_frame * 2 - 2);
    }

    if (g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0] != shutter_time_lines ||
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][0] != shutter_time_lines_short) {

        g_sensorPtr[ViPipe]->snsAlgInfo.u16IntTimeCnt = g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay + 1;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][0] = shutter_time_lines;
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][0] = shutter_time_lines_short;
    }
}

void cmos_fps_set_imx290(int ViPipe, float f32Fps, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    CAMHAL_LOGD("-imx290- f32Fps = %f, %d\n",f32Fps, (int32_t)(f32Fps / 256));
    uint32_t clk_cnt;
    struct v4l2_ext_control fpsCtrl;

    fpsCtrl.id = V4L2_CID_AML_ORIG_FPS;
    fpsCtrl.value = (int32_t)(f32Fps / 256);
    //CAMHAL_LOGD("-imx290- fpsCtrl.value = %d\n",fpsCtrl.value);

    clk_cnt = g_sensorPtr[ViPipe]->snsAlgInfo.fps * g_sensorPtr[ViPipe]->snsAlgInfo.total.height;
    g_sensorPtr[ViPipe]->snsAlgInfo.total.height = clk_cnt / f32Fps;
    g_sensorPtr[ViPipe]->snsAlgInfo.fps = f32Fps;

    v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &fpsCtrl, 1);

    // todo: update vmx?

    if (g_sensorPtr[ViPipe]->enWDRMode) {
        // wdr
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 1 << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = (205 - 3) << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = (g_sensorPtr[ViPipe]->snsAlgInfo.total.height * 2 - (205+3)) << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = (205 - 3) << SHUTTER_TIME_SHIFT;
    } else {
        // sdr
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_min = 1 << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_max = g_sensorPtr[ViPipe]->snsAlgInfo.total.height << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_long_max = g_sensorPtr[ViPipe]->snsAlgInfo.total.height << SHUTTER_TIME_SHIFT;
        g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_limit = g_sensorPtr[ViPipe]->snsAlgInfo.total.height << SHUTTER_TIME_SHIFT;
    }
    g_sensorPtr[ViPipe]->snsAlgInfo.lines_per_second = g_sensorPtr[ViPipe]->snsAlgInfo.total.height * fpsCtrl.value;
    memcpy(pstAeSnsDft, &g_sensorPtr[ViPipe]->snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));
}

void cmos_alg_update_imx290(int ViPipe)
{
    uint32_t shutter_time_lines = 0, shutter_time_lines_short = 0;
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
            shutter_time_lines = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay];
            if (g_sensorPtr[ViPipe]->enWDRMode == 0) {
                struct v4l2_ext_control expo;
                expo.id = V4L2_CID_EXPOSURE;
                expo.value = shutter_time_lines;
                // sdr mode; SHS1 for long exposure.
                //CAMHAL_LOGD("sdr shutter_time_lines 0x%x", expo.value);
                v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &expo, 1);
            }

            if (g_sensorPtr[ViPipe]->enWDRMode) {
                shutter_time_lines_short = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][g_sensorPtr[ViPipe]->snsAlgInfo.integration_time_apply_delay];
                struct v4l2_ext_control expo;
                expo.id = V4L2_CID_EXPOSURE;
                // wdr: SHS2 for long exposure; SHS1 for short exposure; differs from imx415;
                expo.value = (shutter_time_lines << 16) | shutter_time_lines_short;
                v4l2_subdev_set_ctrls(g_sensorPtr[ViPipe]->sensor_ent, &expo, 1);
                //CAMHAL_LOGD("shutter_time_lines %d (SHS2) shutter_time_lines_short %d(SHS1); 0x%x", shutter_time_lines, shutter_time_lines_short, expo.value);
            }
        }
    }

    for ( i = 3; i > 0; i --) {
        g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32AGain[i - 1];
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[0][i - 1];
        g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][i] = g_sensorPtr[ViPipe]->snsAlgInfo.u32Inttime[1][i - 1];
    }
}

