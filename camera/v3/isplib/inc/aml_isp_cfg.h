/*
*
* SPDX-License-Identifier: GPL-2.0
*
* Copyright (C) 2020 Amlogic or its affiliates
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2.
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef __AML_ISP_CFG_H__
#define __AML_ISP_CFG_H__

#define AE_HISTOGRAM_SIZE 1024
#define FED_FLKR_STAT_MAX 1280
#define MAX_CHANNEL     (2)
#define LTM_STA_BIN_NUM (513)
#define LTM_STA_BLK_REG_NUM (96)
#define PST_STA_BIN_NUM_GLB 256
#define W_BLOCKNUM  32
#define H_BLOCKNUM  32
#define BLKREGNUM   96
#define PK_NODS     4
#define DHZ_NODES   5
#define CURV_NODES  6
#define TOL_PK_NUM      (BLKREGNUM * PK_NODS)
#define TOL_DHZ_NODS    (BLKREGNUM * DHZ_NODES)
#define TOL_CURV_NODS   (BLKREGNUM * CURV_NODES)
#define DNLP_STA_BIN_NUM 64

#define AF_STAT_BLKH_NUM    (17)
#define AF_STAT_BLKV_NUM    (15)
#define AE_STAT_BLKH_NUM    (17)
#define AE_STAT_BLKV_NUM    (15)
#define AWB_STAT_BLKH_NUM   (32)
#define AWB_STAT_BLKV_NUM   (24)
#define FW_DISP_CHN_MAX     (4)
#define FW_RECT_CHN_MAX     (8)

/**
  * @struct wb_zone_info_s
  * @brief white balance statistics info on one slice
  * @details
  * @note
  * @attention
  */
typedef struct wb_zone_info_s{
	uint16_t rg;/**< red gain (r*4096/g) */
	uint16_t bg;/**< blue gain (b*4096/g) */
	uint16_t avg_r;/**< average red value normalize to 0x10000  */
	uint16_t avg_g;/**< average green value normalize to 0x10000  */
	uint16_t avg_b;/**< average blue value normalize to 0x10000  */
	uint16_t avg_luma;/**< average blue value normalize to 0x10000  */
	uint32_t sum;/**< valid pixel sum on the slice */
} wb_zone_info_t;

typedef struct _isp_awb_stats_pack_t {
	uint32_t pack0;
	uint32_t pack1;
} isp_awb_stats_pack_t;

typedef struct _isp_awb_stats_mode0_unit_s
{
    uint16_t rg;
    uint16_t bg;
    uint32_t pix_sum;
}isp_awb_stats_mode0_unit_t;

typedef struct _isp_awb_stats_mode1_unit_s
{
    uint16_t avg_r;
    uint16_t avg_g;
    uint16_t avg_b;
    uint16_t avg_luma;
}isp_awb_stats_mode1_unit_t;

/**
  * @struct wb_stats_info_s
  * @brief white balance statistics info
  * @details
  * @note
  * @attention
  */

typedef struct wb_stats_info_s {
	isp_awb_stats_pack_t data[4*AWB_STAT_BLKH_NUM*AWB_STAT_BLKV_NUM];

	//Need not provid by driver
	wb_zone_info_t  zones[4][AWB_STAT_BLKH_NUM*AWB_STAT_BLKV_NUM];/**< all of WB zone statistic info */
	uint32_t        zones_rows;/**< valid rows of the WB zone table */
	uint32_t        zones_cols;/**< valid cols of the WB zone table */
	uint32_t        zones_num;/**< valid table count of the WB zone table */
	uint32_t        zones_size;/**< valid zone count of the WB zone table */
	uint32_t        zones_mode;/**< wb data mode 0: g/rb or rb/g 1: rgb_avg */
	uint32_t        rsv[3];
} wb_stats_info_t;

/**
  * @struct exps_stats_info_s
  * @brief Exposure statistic info
  * @details
  * @note
  * @attention
  */

typedef struct exps_stats_pack1_s {
	uint32_t pack0;
	uint32_t pack1;
	uint64_t pack2;
	uint64_t pack3;
} exps_stats_pack1_t;

typedef struct exps_stats_pack2_s {
	uint32_t pack0;
	uint32_t pack1;
} exps_stats_pack2_t;

typedef struct isp_exps_stats_pack1_s {
	exps_stats_pack1_t  data[AE_STAT_BLKH_NUM*AE_STAT_BLKV_NUM];
	uint32_t  rsv[2];
	uint32_t  hist[AE_HISTOGRAM_SIZE];/**< Exposure histogram statistic */
} isp_exps_stats_pack1_t;

typedef struct isp_exps_stats_pack2_s {
    exps_stats_pack2_t  data[AE_STAT_BLKH_NUM*AE_STAT_BLKV_NUM];
    uint32_t            rsv[2];
    uint32_t            hist[AE_HISTOGRAM_SIZE];/**< Exposure histogram statistic */
} isp_exps_stats_pack2_t;

typedef struct exps_zone_info_s{
    uint16_t blk_hist[4];/**< local block hist-5bin */
} exps_zone_info_t;

typedef struct exps_stats_info_s{
	union {
		uint32_t stats[((AE_STAT_BLKH_NUM * AE_STAT_BLKV_NUM * 3 + 1) * 2) + AE_HISTOGRAM_SIZE];
		isp_exps_stats_pack1_t stats1;
		isp_exps_stats_pack2_t stats2;
	};

	//Need not provid by driver
	exps_zone_info_t  zones[AE_STAT_BLKH_NUM*AE_STAT_BLKV_NUM];/**< all of exposure statistic info */
	uint32_t  zones_rows;/**< valid rows of the exposure statistic table */
	uint32_t  zones_cols;/**< valid cols of the exposure statistic table */
	uint32_t  zones_size;/**< valid zone count of the exposure statistic table */
	uint32_t  hist_sum;/**< Exposure histogram pixel total */
	uint32_t  rsv[2];
} exps_stats_info_t;

typedef struct af_zone_info_t {
	uint16_t i2_mat;
	uint16_t i4_mat;
	uint16_t c4_mat;
	uint16_t c4_exp;
	uint16_t i2_exp;
	uint16_t i4_exp;
} af_zone_info_t;

typedef struct af_zone_info2_t {
	uint16_t h0;
	uint16_t h1;
	uint16_t v0;
	uint16_t v1;
	uint16_t y;
	uint16_t y_cnt;
	uint8_t h_cnt0;
	uint8_t h_cnt1;
	uint8_t v_cnt0;
	uint8_t v_cnt1;
} af_zone_info2_t;

typedef struct af_stats_pack1_s {
	uint32_t pack0;
	uint32_t pack1;
} af_stats_pack1_t;

typedef struct af_stats_pack2_s {
	uint32_t pack0;
	uint32_t pack1;
	uint32_t pack2;
	uint32_t pack3;
} af_stats_pack2_t;

typedef struct isp_af_stats_pack1_s {
	af_stats_pack1_t data[AF_STAT_BLKH_NUM*AF_STAT_BLKV_NUM];
	uint32_t rsv[2];
} isp_af_stats_pack1_t;

typedef struct isp_af_stats_pack2_s {
	af_stats_pack2_t data[AF_STAT_BLKH_NUM*AF_STAT_BLKV_NUM];
} isp_af_stats_pack2_t;

/**
  * @struct af_stats_info_s
  * @brief AF statistic info
  * @details
  * @note
  * @attention
  */
typedef struct af_stats_info_s {
	union {
		uint32_t stats[AF_STAT_BLKH_NUM*AF_STAT_BLKV_NUM*4];
		isp_af_stats_pack1_t stat1;
		isp_af_stats_pack2_t stat2;
	};

	//Need not provid by driver
	union {
		af_zone_info_t  zone_mode0[AF_STAT_BLKH_NUM*AF_STAT_BLKV_NUM];
		af_zone_info2_t zone_mode1[AF_STAT_BLKH_NUM*AF_STAT_BLKV_NUM];
	};
	union {
		af_zone_info_t  glb_mode0;
		af_zone_info2_t glb_mode1;
	};
	uint16_t  zones_rows;/**< valid rows of the AF zone table */
	uint16_t  zones_cols;/**< valid cols of the AF zone table */
	uint16_t  zones_size;/**< valid zone count of the AF zone table */
	uint16_t  zones_mode;/**< valid zone mode */
	uint16_t  reserve[4];
} af_stats_info_t;

/**
  * @struct deflkr_stats_info_s
  * @brief de-flikcer statistic info
  * @details
  * @note
  * @attention
  */
typedef struct deflkr_stats_info_s{
	uint32_t data[FED_FLKR_STAT_MAX / 10 * 4];/**< each line average value of the input image of the current frame  */

	//Need not provid by driver
	uint32_t dif[FED_FLKR_STAT_MAX];
} deflkr_stats_info_t;

typedef struct post_stats_info_s {
	uint32_t data[2128];
} post_stats_info_t;

/**
  * @struct wdr_stats_info_s
  * @brief Wide Dynamic Range statistic
  * @details
  * @note
  * @attention
  */
typedef struct wdr_stats_info_s{
	uint32_t wdr_expcomb_slope_weight;
	uint32_t wdr_expcomb_ir_slope_weight;
	uint32_t fw_wdr_stat_sum[MAX_CHANNEL];
	uint32_t fw_wdr_stat_cnt[MAX_CHANNEL];
} wdr_stats_info_t;

/**
  * @struct ltm_stats_info_s
  * @brief Local Tone Mapping statistic info
  * @details
  * @note
  * @attention
  */
typedef struct ltm_stats_info_s{
	int32_t reg_ltm_stat_hblk_num;                             /**< u5, ltm processing region number of H, maximum to (LTM_STA_LEN_H-1)   (0~16) */
	int32_t reg_ltm_stat_vblk_num;                             /**< u5, ltm processing region number of V, maximum to (LTM_STA_LEN_V-1)   (0~16) */
	uint32_t ro_ltm_gmax_idx;                                   /**< u10, global lmax index */
	uint32_t ro_ltm_gmin_idx;                                   /**< u10, global lmin index */
	uint32_t ro_sum_log_4096_l;                                 /**< uint32_t, log_4096 sum lower , to calc log_avg_4096_blk in fw */
	uint32_t ro_sum_log_4096_h;                                 /**< u17, log_4096 sum higher , to calc log_avg_4096_blk in fw */
	uint32_t ro_ltm_histbuf[LTM_STA_BIN_NUM+1];                 /**< u24*514, global hist, 514 for ir case, 513 for mrgb case */
	int32_t ro_ltm_lmin_blk_in[LTM_STA_BLK_REG_NUM];          /**< u20*96, local lmin */
	int32_t ro_ltm_lmax_blk_in[LTM_STA_BLK_REG_NUM];          /**< u20*96, local lmax */
	uint32_t ro_ltm_sta_hst_blk_sum_h_init[LTM_STA_BLK_REG_NUM];/**< uint32_t*96, local hist sum high 17bits, prepare to calc log_avg_blk_4096 -> local reg_ltm_la_blk; */
	uint32_t ro_ltm_sta_hst_blk_sum_l_init[LTM_STA_BLK_REG_NUM];/**< uint32_t*96, local hist sum low  32bits, prepare to calc log_avg_blk_4096 -> local reg_ltm_la_blk; */
} ltm_stats_info_t;

/**
  * @struct lc_stats_info_s
  * @brief Local Contrast statistic info
  * @details
  * @note
  * @attention
  */
typedef struct lc_stats_info_s{
	uint32_t reg_lc_blk_hnum_both;
	uint32_t reg_lc_blk_vnum_both;
	uint32_t reg_lc_sta_hnum_both;
	uint32_t reg_lc_sta_vnum_both;
	int32_t ro_min_val_glb;
	int32_t ro_max_val_glb;
	int32_t hist_matrix[BLKREGNUM * 16];
	int32_t max_matrix[BLKREGNUM * 3];
	int32_t ro_pst_sta_hst[PST_STA_BIN_NUM_GLB]; //post_stat_info_t
	int32_t ro_wdr_stat_yblk_flt[W_BLOCKNUM * H_BLOCKNUM];
	int32_t reg_skin_cnt_region[BLKREGNUM]; //post_stat_info_t
	int32_t fw_ram_curve_nodes_in[TOL_CURV_NODS];
	int32_t fw_ram_pk_idxs_in[TOL_PK_NUM];
	int32_t fw_ram_pk_vals_in[TOL_PK_NUM];
} lc_stats_info_t;

/**
  * @struct dnlp_stats_info_s
  * @brief DNLP statistic info
  * @details
  * @note
  * @attention
  */
typedef struct dnlp_stats_info_s{
	//fw input control curves
	//hw input hist info
	uint32_t ro_dnlp_sta_hst[DNLP_STA_BIN_NUM];
	uint32_t ro_dnlp_pix_amount;                        //raw_hst_sum
	uint64_t ve_dnlp_luma_sum;
} dnlp_stats_info_t;

/**
  * @struct dehaze_stats_info_s
  * @brief Dehaze statistic info
  * @details
  * @note
  * @attention
  */
typedef struct dehaze_stats_info_s{
	uint32_t reg_dhz_blk_hnum_both;
	uint32_t reg_dhz_blk_vnum_both;
	int32_t fw_ram_dhz_nodes_in[TOL_DHZ_NODS];                    //uBL*384region*8cell

	uint32_t ro_min_val_glb;
	uint32_t ro_max_val_glb;
/* ro_post_stat no need provide here */
	uint32_t ro_dhz_sta_hst[PST_STA_BIN_NUM_GLB];     //uint32_tx256: LC global sta on 256 RGBMax bins;
} dehaze_stats_info_t;

typedef struct top_req_info_s{
	uint32_t raw_mode;
	uint32_t wdr_stat_en;
} top_req_info_t;

typedef struct awb_req_info_s{
	uint32_t awb_stat_hblk_num;
	uint32_t awb_stat_vblk_num;
	uint32_t awb_stat_luma_div_mode;
	uint32_t awb_stat_local_mode;
	uint32_t awb_stat_ratio_mode;
	uint32_t blc_ofst[5];
	uint32_t awb_stat_switch;
	uint32_t awb_stat_pack[2];
} awb_req_info_t;

typedef struct ae_req_info_s {
	int32_t gtm_en;
	int32_t pst_gtm_en; // none
	int32_t gtm_lut_mode;
	int32_t pst_gamma_mode;
	int32_t gtm_lut_stp[8];
	int32_t gtm_lut_num[8];
	uint32_t ae_stat_switch;
	uint32_t ae_stat_local_mode;
	uint32_t ae_stat_hblk_num;
	uint32_t ae_stat_vblk_num;
	uint32_t ae_stat_glbpixnum;
	uint32_t roi0_pack0;
	uint32_t roi0_pack1;
	uint32_t roi1_pack0;
	uint32_t roi1_pack1;
} ae_req_info_t;

typedef struct af_req_info_s {
	uint32_t af_stat_hblk_num;
	uint32_t af_stat_vblk_num;
	uint32_t af_stat_sel;
	uint32_t af_glb_stat_pack0;
	uint32_t af_glb_stat_pack1;
	uint32_t af_glb_stat_pack2;
	uint32_t af_glb_stat_pack3;
} af_req_info_t;
typedef struct flkr_req_info_s{
	uint32_t flkr_binning_rs;
	uint32_t flkr_det_en;
	uint32_t flkr_ro_mode;
	uint32_t flkr_sta_pos;
} flkr_req_info_t;

typedef struct wdr_req_info_s{
	uint32_t wdr_en;
	uint32_t wdr_lexpratio_int64_0;
	uint32_t wdr_lexpratio_int64_1;
	uint32_t wdr_expcomb_blend_thd0;
	uint32_t wdr_expcomb_blend_thd1;
} wdr_req_info_t;

typedef struct ltm_req_info_s{
	uint32_t ltm_dark_floor_uint8_t;
	uint32_t ltm_expblend_thd0;
	uint32_t ltm_expblend_thd1;
	uint32_t ltm_min_factor_uint8_t;
	uint32_t ltm_max_factor_uint8_t;
	uint32_t ltm_bright_floor_u14;
} ltm_req_info_t;

typedef struct lc_req_info_s {
	uint32_t lc_lmtrat_minmax;

	uint32_t lc_blackbar_mute_en;   //u1 mute the black bar corresponding bin, 0: no mute, 1: mute enable; default=1
	uint32_t lc_blackbar_mute_thrd; //u1 mute the black bar corresponding bin, 0: no mute, 1: mute enable; default=1
	uint32_t lc_histvld_thrd;   //uint8_t threshold to compare to bin to get number of valid bins
	uint32_t lc_lmtrat_valid;   //uint8_t x/1024 of amount
	uint32_t lc_contrast_low;   //u12 contrast gain to the lc for dark side, normalized 256 as "1", set adaptive TODO
	uint32_t lc_contrast_hig;   //u12 contrast gain to the lc for bright side, normalized 256 as "1"
	uint32_t lc_cntstlmt_low[2] ;   //uint8_t limit for the contrast low, delta_low = MIN(delta_low, MIN( MAX((minBV-min_val)*scl_low/8, lmt_low[0]),lmt_low[1]))
	uint32_t lc_cntstlmt_hig[2] ;   //uint8_t limit for the contrast high,delta_hig = MIN(delta_hig, MIN( MAX((max_val-maxBV)*scl_hig/8, lmt_hig[0]),lmt_hig[1]))
	uint32_t lc_cntstscl_low;   //uint8_t scale for the contrast low, norm 8 as 1; delta_low = MIN(delta_low, MIN(MAX((minBV-min_val)*scl_low/8, lmt_low[0]),lmt_low[1]))
	uint32_t lc_cntstscl_hig;   //uint8_t scale for the contrast high,norm 8 as 1; delta_hig = MIN(delta_hig, MIN(MAX((max_val-maxBV)*scl_hig/8, lmt_hig[0]),lmt_hig[1]))
	uint32_t lc_cntstbvn_low;   //uint8_t scale to num_m as limit of min_val to minBV distance, to protect mono-color, default = 32; min_val= MAX(min_val, minBV- MAX(num_m-1,0)*bvn_low)
	uint32_t lc_cntstbvn_hig;   //uint8_t scale to num_m as limit of max_val to maxBV distance, to protect mono-color, default = 32; min_val= MIN(max_val, maxBV+ MAX(num_m-1,0)*bvn_low)
	uint32_t lc_num_m_coring;   //u4 coring to num_m, soft coring,default = 2;
	uint32_t lc_vbin_min;   //uint8_t 4x is min width of valid histogram bin num,
	uint32_t lc_ypkbv_ratio[4];   //uint8_tx4, x= ratio*(maxBv-minBv)+min_val as low bound of the ypkBV; normalized to 256 as 1, for pkBV as 0, 256,512, 784above ratio
	uint32_t lc_ypkbv_slope_lmt[2]  ;   //u6+uint8_t, min max slop for the curves to avoid artifacts, [0] for min_slope, [1] for max_slop, e.g.max_slope= limit*(pkBv-minBv)+min_val as high bound of the ypkBV; normalized to 32 as 1
	uint32_t lc_slope_max_face;    //uint8_t, maximum slope for the pkBin-maxBV range curve to do face protection, normalized to 32 as 1, default= 48

	uint32_t lc_yminval_lmt[16];   //u10x16, lmt_val = lmt[minBV(64:64:1023)], and yminV = MAX(yminV,lmt_val), for very dark region boost, default= [48, 80, 120, 60]
	uint32_t lc_ypkbv_lmt[16];   //u10x16, lmt_val = 4*lmt[pkBV(64:64:1023) or maxBV(64:64:1023)], and ypkBV = MAX(ypkBV,lmt[pkBV]), ymaxV = MAX(ymaxV,lmt[maxBV])&& above min_slop for very dark region boost, default= ...
	uint32_t lc_ymaxval_lmt[16];   //u10x16, lmt_val = 4*lmt[pkBV(64:64:1023) or maxBV(64:64:1023)], and ypkBV = MAX(ypkBV,lmt[pkBV]), ymaxV = MAX(ymaxV,lmt[maxBV])&& above min_slop for very dark region boost, default= ...

	//deblock
	uint32_t lc_pk_vld; //uint8_t threshold to compare to bin to get number of valid bins

	uint32_t lc_sta_blk_enable;
	uint32_t pst_sta_enable;
	uint32_t lc_stat_pack_num;
}lc_req_info_t;

typedef struct ofe_req_info_s{
	uint32_t bac_mode;
} ofe_req_info_t;

typedef struct dehaze_req_info_s{
	uint32_t dhz_minvval_lmt[16];
	uint32_t dhz_maxvval_lmt[16];
	uint32_t dhz_lmtrat_lowc;
	uint32_t dhz_lmtrat_higc;
	uint32_t dhz_dlt_rat;
	uint32_t dhz_cc_en;
	uint32_t dhz_hig_dlt_rat;
	uint32_t dhz_low_dlt_rat;
} dehaze_req_info_t;

typedef struct module_info_s{
	top_req_info_t top_req_info;
	awb_req_info_t awb_req_info;
	ae_req_info_t ae_req_info;
	af_req_info_t af_req_info;
	flkr_req_info_t flkr_req_info;
	wdr_req_info_t wdr_req_info;
	ltm_req_info_t ltm_req_info;
	lc_req_info_t lc_req_info;
	ofe_req_info_t ofe_req_info;
	dehaze_req_info_t dehaze_req_info;
} module_info_t;

typedef struct frame_info_s {
	int frm_cnt;/**< frame number for HW counter */
	int slice_num;
	int slice_ovlp;
	int reserved[1];
} frame_info_t;

/**
  * @struct aisp_stats_info_s
  * @brief all of statistic info on the ISP system
  * @details
  * @note
  * @attention
  */
typedef struct aisp_stats_info_s{
	frame_info_t frame_info;
	wb_stats_info_t wb_stats;/**< White Balance statistic info */
	exps_stats_info_t exps_stats;/**< Exposure statistic info */
	af_stats_info_t af_stats;/**< Auto Focus statistic info */
	deflkr_stats_info_t deflkr_stats;/**< De-flicker statistic info */
	post_stats_info_t post_stats;
	wdr_stats_info_t  wdr_stats;/**< Wide Dynamic Range statistic info */
	ltm_stats_info_t ltm_stats;/**< Local Tone Mapping statistic info */
	lc_stats_info_t lc_stats;/**< Local Contrast statistic info */
	dnlp_stats_info_t dnlp_stats;/**< DNLP statistic info */
	dehaze_stats_info_t dehaze_stats;/**< Dehaze statistic info */
	module_info_t module_info;/**<module required info>*/
} aisp_stats_info_t;

/********************** 3a **********************/
typedef struct aisp_dgain_cfg_s {
	uint32_t dg_gain[5];
	uint32_t idg_gain[5];
} aisp_dgain_cfg_t;

typedef struct aisp_wb_change_cfg_s {
	uint32_t wb_gain[5];
	uint32_t wb_limit[5];
	uint32_t ae_gain_grbgi[5];
	uint32_t ae_bl12_grbgi[5];
} aisp_wb_change_cfg_t;

typedef struct aisp_wb_luma_cfg_s {
	uint32_t awb_stat_blc20[4];
	uint32_t awb_stat_gain10[4];
	uint32_t awb_stat_satur_low;
	uint32_t awb_stat_satur_high;
} aisp_wb_luma_cfg_t;

typedef struct aisp_wb_triangle_cfg_s {
	uint32_t awb_stat_satur_vald;
	uint32_t awb_stat_rg_min;
	uint32_t awb_stat_rg_max;
	uint32_t awb_stat_bg_min;
	uint32_t awb_stat_bg_max;
	uint32_t awb_stat_rg_low;
	uint32_t awb_stat_rg_high;
	uint32_t awb_stat_bg_low;
	uint32_t awb_stat_bg_high;
} aisp_wb_triangle_cfg_t;

typedef struct aisp_wb_roi_cfg_s {
	uint32_t awb_xstart[2];
	uint32_t awb_xsize[2];
	uint32_t awb_ystart[2];
	uint32_t awb_ysize[2];
} aisp_wb_roi_cfg_t;

typedef struct aisp_expo_mode_cfg_s {
	uint8_t ae_stat_blk_weight[17*15];
	uint8_t reserved;
} aisp_expo_mode_cfg_t;

typedef struct aisp_awb_weight_cfg_s {
	uint32_t awb_stat_blk_weight[32*24];
} aisp_awb_weight_cfg_t;

/********************** base **********************/
typedef struct aisp_phase_setting_cfg_s {
	uint32_t raw_mode;
	uint32_t src_bit_depth;
	uint32_t raw_phslut[16];
	uint32_t losse_raw_phslut[16];
	uint32_t lossd_raw_phslut[16];
} aisp_phase_setting_cfg_t;

typedef struct aisp_mesh_cfg_s {
	uint32_t lns_mesh_xnum;
	uint32_t lns_mesh_ynum;
	uint32_t lns_mesh_alpmode;
	uint32_t lns_mesh_xlimit;
	uint32_t lns_mesh_ylimit;
	uint32_t lns_mesh_xscale;
	uint32_t lns_mesh_yscale;
	uint32_t lns_mesh_prtmode;
	uint32_t lns_mesh_lutnorm_grbg[4];
	uint32_t lns_mesh_alp[4];
} aisp_mesh_cfg_t;

typedef struct aisp_setting_fixed_cfg_s {
	uint32_t bac_hcoef[5];
	uint32_t bac_vcoef[5];
	uint32_t sqrt1_mode;
	uint32_t eotf1_mode;
	uint32_t wb_rate_rs;
	uint32_t curve_lc_en;
	uint32_t curve_dhz_en;
	uint32_t hsc_tap_num_0;
	uint32_t vsc_tap_num_0;
	uint32_t hsc_nor_rs_bits_0;
	uint32_t vsc_nor_rs_bits_0;
	uint32_t sqrt1_num[8];
	uint32_t sqrt1_stp[8];
	uint32_t eotf1_num[8];
	uint32_t eotf1_stp[8];
	uint32_t lns_center_xy[2];
	uint32_t wb_stats_local_mode;
} aisp_setting_fixed_cfg_t;

typedef struct aisp_lut_fixed_cfg_s {
	uint32_t pat_bar24rgb[2][3][24];
	uint32_t ae_stat_blk_weight[17*15];
	uint32_t awb_stat_blk_weight[32*24];
	uint32_t fpnr_corr_val[2][2048*5];
	uint32_t sqrt0_lut[33];
	uint32_t sqrt1_lut[129];
	uint32_t decmp0_lut[33];
	uint32_t decmp1_lut[129];
	uint32_t eotf0_lut[33];
	uint32_t eotf1_lut[129];
	uint32_t lns_rad_lut129[65*4];
	uint32_t lns_radext_lut129[65*8];
	uint32_t lns_mesh_lut[32*32*4];
	uint32_t pst_gamma_lut[129*4];
	uint32_t rgb_gamma_lut[129]; // none
	uint32_t gtm_lut[129];
	uint32_t CAC_table_Rx[1024];
	uint32_t CAC_table_Ry[1024];
	uint32_t CAC_table_Bx[1024];
	uint32_t CAC_table_By[1024];
	uint32_t ltm_lmin_blk[96]; //read
	uint32_t ltm_lmax_blk[96]; // read
	uint32_t ltm_histxptsbuf_blk[79];
	uint32_t ltm_ccrat_lut[63];
	uint32_t ltm_ccalp_lut[256];
	uint32_t lc_satur_lut[63];
	uint32_t dhz_sky_prot_lut[64];
	uint32_t ram_lcmap_nodes[96*6];
	uint32_t ram_dhzmap_nodes[96*5];
	uint32_t pps_h_luma_coef[33 * 8];
	uint32_t pps_v_luma_coef[33 * 8];
	uint32_t pps_h_chroma_coef[33 * 8];
	uint32_t pps_v_chroma_coef[33 * 8];
	uint32_t snr_lpf_phs_sel[96];
} aisp_lut_fixed_cfg_t;

typedef struct aisp_pat_cfg_s{
	uint32_t pat_fwin_en_0;
	uint32_t pat_fwin_en_1;
	uint32_t pat_xmode_0;
	uint32_t pat_xmode_1;
	uint32_t pat_ymode_0;
	uint32_t pat_ymode_1;
	uint32_t pat_xinvt_0;
	uint32_t pat_xinvt_1;
	uint32_t pat_yinvt_0;
	uint32_t pat_yinvt_1;
} aisp_pat_cfg_t;

typedef struct aisp_top_cfg_s {
	uint32_t src_inp_chn;
	uint32_t wdr_inp_chn;
	uint32_t decmp_en;
	uint32_t inp_fmt_en;
	uint32_t bac_en;
	uint32_t fpnr_en;
	uint32_t ge_en_0;
	uint32_t ge_en_1;
	uint32_t dpc_en_0;
	uint32_t dpc_en_1;
	uint32_t pat_en_0;
	uint32_t pat_en_1;
	uint32_t og_en_0;
	uint32_t og_en_1;
	uint32_t lcge_enable;
	uint32_t pdpc_enable;
	uint32_t cac_en;
	uint32_t rawcnr_enable;
	uint32_t snr1_en;
	uint32_t mc_tnr_en;
	uint32_t tnr0_en;
	uint32_t cubic_cs_en;
	uint32_t sqrt_en;
	uint32_t eotf_en;
	uint32_t ltm_en;
	uint32_t gtm_en;
	uint32_t lns_mesh_en;
	uint32_t lns_rad_en;
	uint32_t wb_en;
	uint32_t blc_en;
	uint32_t nr_en;
	uint32_t pk_en;
	uint32_t dnlp_en;
	uint32_t dhz_en;
	uint32_t lc_en;;
	uint32_t bsc_en;
	uint32_t cnr2_en;
	uint32_t pst_gamma_en;
	uint32_t ccm_en;
	uint32_t dmsc_en;
	uint32_t cm0_en;
	uint32_t pst_tnr_lite_en;
	uint32_t amcm_en;
	uint32_t ae_stat_en;
	uint32_t awb_stat_en;
	uint32_t af_stat_en;
	uint32_t wdr_stat_en;
	uint32_t expstitch_mode;
	uint32_t pst_mux_mode;
	uint32_t flkr_sta_pos;
	uint32_t awb_stat_switch;
	uint32_t ae_stat_switch;
	uint32_t af_stat_switch;
	uint32_t ae_input_2ln;
	uint32_t flkr_stat_en;
	uint32_t flkr_sta_input_format;
	uint32_t cubic_en;
	uint32_t pnrmif_en;
	uint32_t nrmif_en;
} aisp_top_cfg_t;

typedef struct aisp_hlc_cfg_s {
	uint32_t hlc_en;
	uint32_t hlc_luma_thd;
	uint32_t hlc_luma_trgt;
} aisp_hlc_cfg_t;

typedef struct aisp_cvr_cfg_s {
	uint32_t cvr_rect_en;
	uint32_t cvr_rect_hstart[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_vstart[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_hend[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_vend[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_val_y[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_val_u[FW_RECT_CHN_MAX];
	uint32_t cvr_rect_val_v[FW_RECT_CHN_MAX];
} aisp_cvr_cfg_t;
typedef struct aisp_base_cfg_s {
	aisp_phase_setting_cfg_t phase_cfg;
	aisp_setting_fixed_cfg_t fxset_cfg;
	aisp_lut_fixed_cfg_t fxlut_cfg;
} aisp_base_cfg_t;

/********************** correction **********************/
typedef struct aisp_ccm_cfg_s {
	uint32_t ccm_4x3matrix[3][4];
	uint32_t csc3_en;
	uint32_t csc1_offset_inp[3];
	uint32_t csc1_offset_oup[3];
	uint32_t csc1_3x3mtrx_rs;
	uint32_t csc1_3x3matrix[3][3];
} aisp_ccm_cfg_t;

typedef struct aisp_csc_cfg_s {
	uint32_t cm0_offset_inp[3];
	uint32_t cm0_offset_oup[3];
	uint32_t cm0_3x3mtrx_rs;
	uint32_t cm0_3x3matrix[3][3];
} aisp_csc_cfg_t;
typedef struct aisp_mesh_crt_cfg_s {
	uint32_t lns_mesh_alp[4];
	uint32_t rad_lut65[65];
} aisp_mesh_crt_cfg_t;

typedef struct aisp_lns_cfg_s {
	uint32_t lns_rad_strength;
	uint32_t lns_mesh_strength;
} aisp_lns_cfg_t;

typedef struct aisp_blc_cfg_s {
	uint32_t fe_bl_ofst[5];
	uint32_t blc_ofst[5];
	uint32_t eotf_pre_ofst;
	uint32_t eotf_pst_ofst;
	uint32_t idg_ofst;
	uint32_t dg_ofst;
	uint32_t sqrt_pre_ofst;
	uint32_t sqrt_pst_ofst;
} aisp_blc_cfg_t;


/********************** enhance **********************/
typedef struct aisp_ltm_cfg_s{
	uint32_t ltm_lmin_blk[96]; //read
	uint32_t ltm_lmax_blk[96]; //read
	uint32_t ltm_lr_u28;
	uint32_t ltm_expblend_thd0;
	uint32_t ltm_expblend_thd1;
	uint32_t ltm_gmin_total;
	uint32_t ltm_gmax_total;
	uint32_t ltm_glbwin_hstart;
	uint32_t ltm_glbwin_hend;
	uint32_t ltm_glbwin_vstart;
	uint32_t ltm_glbwin_vend;
	uint32_t ltm_lo_gm_u6;
	uint32_t ltm_hi_gm_u7;
	uint32_t ltm_pow_y_u20;
	uint32_t ltm_pow_divisor_u23;
} aisp_ltm_cfg_t;

typedef struct aisp_ltm_enhc_cfg_s{
	uint32_t ltm_cc_en;
	uint32_t ltm_dtl_ehn_en;
	uint32_t ltm_vs_gtm_alpha;
	uint32_t ltm_lmin_med_en;
	uint32_t ltm_lmax_med_en;
	uint32_t ltm_b2luma_alpha;
	uint32_t ltm_satur_lut[63];
} aisp_ltm_enhc_cfg_t;

typedef struct aisp_wdr_cfg_s {
	uint32_t wdr_motiondect_en;
	uint32_t wdr_mdetc_withblc_mode;
	uint32_t wdr_mdetc_chksat_mode;
	uint32_t wdr_mdetc_motionmap_mode;
	uint32_t wdr_mdeci_chkstill_mode;
	uint32_t wdr_mdeci_addlong;
	uint32_t wdr_mdeci_still_thd;
	uint32_t wdr_forcelong_en;
	uint32_t wdr_forcelong_thdmode;
	uint32_t wdr_expcomb_maxavg_mode;
	uint32_t wdr_expcomb_maxavg_ratio;
	uint32_t wdr_stat_flt_en;
	uint32_t wdr_lexpratio_int64[4];
	uint32_t wdr_lmapratio_int64[3][2];
	uint32_t wdr_lexpcomp_gr_int64[3];
	uint32_t wdr_lexpcomp_gb_int64[3];
	uint32_t wdr_lexpcomp_rg_int64[3];
	uint32_t wdr_lexpcomp_bg_int64[3];
	uint32_t wdr_lexpcomp_ir_int64[3];
	uint32_t wdr_mdetc_sat_gr_thd;
	uint32_t wdr_mdetc_sat_gb_thd;
	uint32_t wdr_mdetc_sat_rg_thd;
	uint32_t wdr_mdetc_sat_bg_thd;
	uint32_t wdr_mdetc_sqrt_again_rg;
	uint32_t wdr_mdetc_sqrt_again_g;
	uint32_t wdr_mdetc_sqrt_again_bg;
	uint32_t wdr_mdetc_sqrt_again_ir;
	uint32_t wdr_mdetc_sqrt_dgain_rg;
	uint32_t wdr_mdetc_sqrt_dgain_g;
	uint32_t wdr_mdetc_sqrt_dgain_bg;
	uint32_t wdr_mdetc_sqrt_dgain_ir;
	uint32_t wdr_mdetc_lo_weight[3];
	uint32_t wdr_mdetc_hi_weight[3];
	uint32_t wdr_mdetc_noisefloor_g[2];
	uint32_t wdr_mdetc_noisefloor_rg[2];
	uint32_t wdr_mdetc_noisefloor_bg[2];
	uint32_t wdr_mdeci_sexpstill_gr_lsthd[2];
	uint32_t wdr_mdeci_sexpstill_gb_lsthd[2];
	uint32_t wdr_mdeci_sexpstill_rg_lsthd[2];
	uint32_t wdr_mdeci_sexpstill_bg_lsthd[2];
	uint32_t wdr_flong2_thd0[2];
	uint32_t wdr_flong2_thd1[2];
	uint32_t wdr_flong1_thd0;
	uint32_t wdr_flong1_thd1;
	uint32_t wdr_expcomb_maxratio;
	uint32_t wdr_expcomb_blend_slope;
	uint32_t wdr_expcomb_blend_thd0;
	uint32_t wdr_expcomb_blend_thd1;
	uint32_t wdr_expcomb_ir_blend_slope;
	uint32_t wdr_expcomb_ir_blend_thd0;
	uint32_t wdr_expcomb_ir_blend_thd1;
	uint32_t wdr_expcomb_maxsat_gr_thd;
	uint32_t wdr_expcomb_maxsat_gb_thd;
	uint32_t wdr_expcomb_maxsat_rg_thd;
	uint32_t wdr_expcomb_maxsat_bg_thd;
	uint32_t wdr_expcomb_maxsat_ir_thd;
	uint32_t wdr_expcomb_slope_weight;
	uint32_t wdr_expcomb_ir_slope_weight;
	uint32_t wdr_expcomb_maxavg_winsize;
	uint32_t wdr_flong2_colorcorrect_en;
	uint32_t wdr_mdeci_fullmot_thd;
	uint32_t comb_expratio_int64[3];
	uint32_t comb_exprratio_int1024[2];
	uint32_t comb_g_lsbarrier[4];
	uint32_t comb_rg_lsbarrier[4];
	uint32_t comb_bg_lsbarrier[4];
	uint32_t comb_ir_lsbarrier[4];
	uint32_t comb_maxratio;
	uint32_t comb_shortexp_mode;
	uint32_t wdr_force_exp_en;
	uint32_t wdr_force_exp_mode;
} aisp_wdr_cfg_t;

typedef struct aisp_wdr_blc_cfg_s {
	uint32_t wdr_blacklevel_gr;
	uint32_t wdr_blacklevel_gb;
	uint32_t wdr_blacklevel_rg;
	uint32_t wdr_blacklevel_bg;
	uint32_t wdr_blacklevel_ir;
	uint32_t wdr_blacklevel_wdr;
} aisp_wdr_blc_cfg_t;

typedef struct aisp_wdr_fmt_cfg_s{
	uint32_t sensor_bitdepth;
} aisp_wdr_fmt_cfg_t;

typedef struct aisp_wb_enhc_cfg_s{
	uint32_t awb_gain_256[5];
} aisp_wb_enhc_cfg_t;

typedef struct aisp_lc_cfg_s {
	uint32_t lc_histvld_thrd;
	uint32_t lc_blackbar_mute_thrd;
	uint32_t lc_pk_vld;
	uint32_t lc_pk_no_trd_mrgn;
	uint32_t lc_pk_1stb_th;
	uint32_t ram_lcmap_nodes[96*6];
} aisp_lc_cfg_t;

typedef struct aisp_lc_enhc_cfg_s {
	uint32_t lc_en;
	uint32_t lc_cc_en;
	uint32_t lc_blkblend_mode;
	uint32_t lc_lmtrat_minmax;
	uint32_t lc_contrast_low;
	uint32_t lc_contrast_hig;
	uint32_t lc_cntstscl_low;
	uint32_t lc_cntstscl_hig;
	uint32_t lc_cntstbvn_low;
	uint32_t lc_cntstbvn_hig;
	uint32_t lc_ypkbv_slope_lmt_1;
	uint32_t lc_ypkbv_slope_lmt_0;
	uint32_t lc_ypkbv_ratio_2;
	uint32_t lc_ypkbv_ratio_1;
	uint32_t lc_satur_lut[63];
} aisp_lc_enhc_cfg_t;

typedef struct aisp_dnlp_cfg_s {
	uint32_t dnlp_ygrid[64];
	uint32_t rgb_gamma_gain[4]; // none
	uint32_t rgb_gamma_ofst[4]; // none
} aisp_dnlp_cfg_t;

typedef struct aisp_dhz_cfg_s{
	uint32_t ram_dhz_nodes[96*5];
	uint32_t dhz_atmos_light;
	uint32_t dhz_atmos_light_inver;
	uint32_t dhz_sky_prot_stre;
	uint32_t dhz_sky_prot_offset;
	uint32_t dhz_satura_ratio_sky;
} aisp_dhz_cfg_t;

typedef struct aisp_dhz_enhc_cfg_s {
	uint32_t dhz_dlt_rat;
	uint32_t dhz_hig_dlt_rat;
	uint32_t dhz_low_dlt_rat;
	uint32_t dhz_lmtrat_lowc;
	uint32_t dhz_lmtrat_higc;
	uint32_t dhz_cc_en;
	uint32_t dhz_sky_prot_en;
	uint32_t dhz_sky_prot_stre;
	uint32_t dhz_sky_prot_lut[64];
} aisp_dhz_enhc_cfg_t;

typedef struct aisp_peaking_cfg_s{
	uint32_t pk_debug_edge;
	uint32_t drtlpf_theta_min_idx_replace;
	uint32_t pk_motion_adp_en;
	uint32_t bp_final_gain;
	uint32_t hp_final_gain;
	uint32_t pre_flt_strength;
	uint32_t hp_motion_adp_gain_lut[8];
	uint32_t bp_motion_adp_gain_lut[8];
	uint32_t pk_cir_hp_con2gain[5];
	uint32_t pk_cir_bp_con2gain[5];
	uint32_t pk_drt_hp_con2gain[5];
	uint32_t pk_drt_bp_con2gain[5];
	uint32_t pkgain_vsluma_lut[9];
	uint32_t pk_os_up;
	uint32_t pk_os_down;
	uint32_t pre_bpc_margin;
	int32_t pk_bpf_vdtap05[3];
	int32_t pk_hpf_vdtap05[3];
	int32_t pk_bpf_hztap09[5];
	int32_t pk_hpf_hztap09[5];
	int32_t pkosht_vsluma_lut[9];
	int32_t pk_circ_bpf_2d5x7[3][4];
	int32_t pk_circ_hpf_2d5x7[3][4];
	uint32_t pk_osh_winsize;
	uint32_t pk_osv_winsize;
	uint32_t ltm_shrp_base_alpha;
	uint32_t ltm_shrp_r_u6;
	uint32_t ltm_shrp_s_uint8_t;
	uint32_t ltm_shrp_smth_lvlsft;
} aisp_peaking_cfg_t;

typedef struct aisp_cm2_cfg_s {
	uint32_t cm2_adj_satglbgain_via_y[9];
	uint32_t cm2_global_sat;
	uint32_t cm2_global_hue;
	uint32_t cm2_luma_contrast;
	uint32_t cm2_luma_brightness;
	uint32_t cm2_adj_luma_via_hue[32];
	uint32_t cm2_adj_sat_via_hs[3][32];
	uint32_t cm2_adj_satgain_via_y[5][32];
	uint32_t cm2_adj_hue_via_h[32];
	uint32_t cm2_adj_hue_via_s[5][32];
	uint32_t cm2_adj_hue_via_y[5][32];
} aisp_cm2_cfg_t;
/********************** restoration **********************/
typedef struct aisp_ge_cfg_s {
	uint32_t ge_stat_edge_thd[2];
	uint32_t ge_hv_thrd[2];
	uint32_t ge_hv_wtlut_0[4];
	uint32_t ge_hv_wtlut_1[4];
} aisp_ge_cfg_t;

typedef struct aisp_fpnr_cfg_s {
	uint32_t fpnr_corr_gain0[2][5];
	uint32_t fpnr_corr_gain1[2][5];
} aisp_fpnr_cfg_t;

typedef struct aisp_dpc_cfg_s {
	uint32_t dpc_avg_gain_l0[2];
	uint32_t dpc_avg_gain_h0[2];
	uint32_t dpc_avg_gain_l1[2];
	uint32_t dpc_avg_gain_h1[2];
	uint32_t dpc_avg_gain_l2[2];
	uint32_t dpc_avg_gain_h2[2];
	uint32_t dpc_cond_en[2];
	uint32_t dpc_max_min_bias_thd[2];
	uint32_t dpc_std_diff_gain[2];
	uint32_t dpc_std_gain[2];
	uint32_t dpc_avg_dev_offset[2];

	uint32_t dpc_cor_en[2];
	uint32_t dpc_avg_dev_mode[2];
	uint32_t dpc_avg_mode[2];
	uint32_t dpc_avg_thd2_en[2];
	uint32_t dpc_highlight_en[2];
	uint32_t dpc_correct_mode[2];
	uint32_t dpc_write_to_lut[2];
} aisp_dpc_cfg_t;

typedef struct aisp_tnr_cfg_s {
	uint32_t rad_tnr0_en;
	uint32_t ma_mix_ratio;
	uint32_t ma_mix_th_x0;
	uint32_t ma_mix_th_x1;
	uint32_t ma_mix_th_x2;
	uint32_t ma_mix_h_th_y0;
	uint32_t ma_mix_h_th_y1;
	uint32_t ma_mix_h_th_y2;
	uint32_t ma_mix_l_th_y0;
	uint32_t ma_mix_l_th_y1;
	uint32_t ma_mix_l_th_y2;
	uint32_t ma_sad_pdtl4_x0;
	uint32_t ma_sad_pdtl4_x1;
	uint32_t ma_sad_pdtl4_x2;
	uint32_t ma_sad_pdtl4_y0;
	uint32_t ma_sad_pdtl4_y1;
	uint32_t ma_sad_pdtl4_y2;
	uint32_t ma_adp_dtl_mix_th_nfl;
	uint32_t ma_sad_th_mask_gain[4];
	uint32_t ma_mix_th_mask_gain[4];
	uint32_t ma_np_lut16[16];
	uint32_t pst_tnr_alp_lut[8];
	uint32_t ma_tnr_sad_cor_np_gain;
	uint32_t ma_tnr_sad_cor_np_ofst;
	uint32_t me_sad_cor_np_gain;
	uint32_t me_sad_cor_np_ofst;
	uint32_t ma_mix_h_th_gain[4];
	uint32_t lut_meta_sad_2alpha[64];
	uint32_t mc_meta2alpha[8][8];
	uint32_t ma_sad_var_th_x0;
	uint32_t ma_sad_var_th_x1;
	uint32_t ma_sad_var_th_x2;
	uint32_t ma_sad_var_th_y_0;
	uint32_t ma_sad_var_th_y_1;
	uint32_t ma_sad_var_th_y_2;
	uint32_t me_meta_sad_th0[3];
	uint32_t me_meta_sad_th1[3];
	uint32_t ma_sad_luma_adj_x[4];
	uint32_t ma_sad_luma_adj_y[5];
	uint32_t ma_mix_th_iso_gain;
} aisp_tnr_cfg_t;

typedef struct aisp_snr_cfg_s {
	uint32_t snr_wt_lut[16];
	uint32_t snr_np_lut16[16];
	uint32_t me_np_lut16[16];
	uint32_t rawcnr_np_lut16[16];
	uint32_t snr_sad_cor_profile_adj;
	uint32_t snr_sad_cor_profile_ofst;
	uint32_t snr_sad_wt_sum_th[2];
	uint32_t snr_var_flat_th_x0;
	uint32_t snr_var_flat_th_x1;
	uint32_t snr_var_flat_th_x2;
	uint32_t snr_var_flat_th_y[3];
	uint32_t snr_sad_meta_ratio[4];
	uint32_t snr_wt_luma_gain[8];
	uint32_t snr_sad_meta2alp[8];
	uint32_t snr_mask_adj[8];
	uint32_t snr_meta_adj[8];
	uint32_t snr_cur_wt[8];
	uint32_t nry_strength;
	uint32_t nrc_strength;
	uint32_t snr_luma_adj_en;
	uint32_t snr_sad_wt_adjust_en;
	uint32_t snr_mask_en;
	uint32_t snr_meta_en;
	uint32_t rad_snr1_en;
	uint32_t snr_wt_var_adj_en;
	uint32_t snr_wt_var_meta_th;
	uint32_t snr_wt_var_th_x0;
	uint32_t snr_wt_var_th_x1;
	uint32_t snr_wt_var_th_x2;
	uint32_t snr_wt_var_th_y[3];
	uint32_t snr_grad_gain[5];
	uint32_t snr_sad_th_mask_gain[4];
	uint32_t snr_coring_mv_gain_x;
	uint32_t snr_coring_mv_gain_xn;
	uint32_t snr_coring_mv_gain_y[2];
} aisp_snr_cfg_t;

typedef struct aisp_rawcnr_cfg_s {
	uint32_t rawcnr_totblk_higfrq_en;
	uint32_t rawcnr_curblk_higfrq_en;
	uint32_t rawcnr_ishigfreq_mode;
	uint32_t rawcnr_sad_cor_np_gain;
	uint32_t rawcnr_meta_gain_lut[8];
	uint32_t rawcnr_higfrq_sublk_sum_dif_thd[2];
	uint32_t rawcnr_higfrq_curblk_sum_difnxn_thd[2];
	uint32_t rawcnr_thrd_ya_min;
	uint32_t rawcnr_thrd_ya_max;
	uint32_t rawcnr_thrd_ca_min;
	uint32_t rawcnr_thrd_ca_max;
	uint32_t rawcnr_sps_csig_weight5x5[25];
} aisp_rawcnr_cfg_t;

typedef struct aisp_cnr_cfg_s {
	uint32_t cnr2_satur_blk[1024];
	uint32_t cnr2_tdif_rng_lut[16];
	uint32_t cnr2_ydif_rng_lut[16];
	uint32_t cnr2_pfr_wind_h;
	uint32_t cnr2_pfr_wind_v;
	uint32_t cnr2_umargin_up;
	uint32_t cnr2_umargin_dw;
	uint32_t cnr2_vmargin_up;
	uint32_t cnr2_vmargin_dw;
	uint32_t cnr2_luma_osat_thd;
	uint32_t cnr2_fin_alp_mode;
	uint32_t cnr2_ctrst_xthd;
	uint32_t cnr2_adp_desat_vrt;
	uint32_t cnr2_adp_desat_hrz;
} aisp_cnr_cfg_t;

typedef struct aisp_dms_cfg_s{
	uint32_t plp_alp;
	uint32_t drt_ambg_mxerr_lmt_0;
	uint32_t drt_ambg_mxerr_lmt_1;
} aisp_dmsc_cfg_t;

typedef union {
	uint32_t  key;
	struct {
		uint32_t  aisp_tnr                 : 1 ;
		uint32_t  aisp_snr                 : 1 ;
		uint32_t  aisp_rawcnr              : 1 ;
		uint32_t  aisp_cnr                 : 1 ;
		uint32_t  aisp_dmsc                : 1 ;
		uint32_t  bitRsv                   : 27; /* H  ; [5:31] */
	};
}  aisp_nr_valid_ctrl;

typedef struct aisp_nr_cfg_s {
	aisp_nr_valid_ctrl   pvalid;
	aisp_tnr_cfg_t       tnr_cfg;
	aisp_snr_cfg_t       snr_cfg;
	aisp_rawcnr_cfg_t    rawcnr_cfg;
	aisp_cnr_cfg_t       cnr_cfg;
	aisp_dmsc_cfg_t      dmsc_cfg;
} aisp_nr_cfg_t;

typedef struct aisp_misc_cfg_s{
	aisp_pat_cfg_t pat_cfg;
	uint32_t tnr_bits;
} aisp_misc_cfg_t;

typedef struct _isp_hwreg_t {
	uint32_t addr;
	uint32_t val;
	uint32_t mask;
	uint32_t len;
} isp_hwreg_t;

typedef struct aisp_custom_cfg_s {
	isp_hwreg_t settings[200];
} aisp_custom_cfg_t;


typedef union {
	uint64_t  key;
	struct {
		uint64_t  aisp_dgain       : 1;
		uint64_t  aisp_wb_change   : 1;
		uint64_t  aisp_wb_luma     : 1;
		uint64_t  aisp_wb_triangle : 1;
		uint64_t  aisp_wb_roi      : 1;
		uint64_t  aisp_expo_mode   : 1;
		uint64_t  aisp_mesh        : 1;
		uint64_t  aisp_top         : 1;
		uint64_t  aisp_hlc         : 1;
		uint64_t  aisp_cvr         : 1;
		uint64_t  aisp_base        : 1;
		uint64_t  aisp_ge          : 1;
		uint64_t  aisp_fpnr        : 1;
		uint64_t  aisp_dpc         : 1;
		uint64_t  aisp_cm2         : 1;
		uint64_t  aisp_nr          : 1;
		uint64_t  aisp_ccm         : 1;
		uint64_t  aisp_csc         : 1;
		uint64_t  aisp_lns         : 1;
		uint64_t  aisp_blc         : 1;
		uint64_t  aisp_mesh_crt    : 1;
		uint64_t  aisp_ltm         : 1;
		uint64_t  aisp_ltm_enhc    : 1;
		uint64_t  aisp_gtm         : 1;
		uint64_t  aisp_wdr         : 1;
		uint64_t  aisp_wdr_blc     : 1;
		uint64_t  aisp_wdr_fmt     : 1;
		uint64_t  aisp_wb_enhc     : 1;
		uint64_t  aisp_lc          : 1;
		uint64_t  aisp_lc_enhc     : 1;
		uint64_t  aisp_dnlp        : 1;
		uint64_t  aisp_dhz         : 1;
		uint64_t  aisp_dhz_enhc    : 1;
		uint64_t  aisp_peaking     : 1;
		uint64_t  aisp_radial      : 1;
		uint64_t  aisp_misc        : 1;
		uint64_t  aisp_custom      : 1;
		uint64_t  bitRsv           : 27; /* H  ; [37:63] */
	};
}  aisp_param_ctrl;

typedef struct aisp_param_s{
	aisp_param_ctrl           pvalid;
	aisp_dgain_cfg_t          dgain;
	aisp_wb_change_cfg_t      wb_change;
	aisp_wb_luma_cfg_t        wb_luma;
	aisp_wb_triangle_cfg_t    wb_triangle;
	aisp_wb_roi_cfg_t         wb_roi;
	aisp_expo_mode_cfg_t      expo_mode;

	aisp_mesh_cfg_t           mesh_cfg;
	aisp_top_cfg_t            top_cfg;
	aisp_hlc_cfg_t            hlc_cfg;
	aisp_cvr_cfg_t            cvr_cfg;
	aisp_base_cfg_t           base_cfg;

	aisp_ge_cfg_t             ge_cfg;
	aisp_fpnr_cfg_t           fpnr_cfg;
	aisp_dpc_cfg_t            dpc_cfg;
	aisp_cm2_cfg_t            cm2_cfg;
	aisp_nr_cfg_t             nr_cfg;

	aisp_ccm_cfg_t            ccm_cfg;
	aisp_csc_cfg_t            csc_cfg;
	aisp_lns_cfg_t            lns_cfg;
	aisp_blc_cfg_t            blc_cfg;
	aisp_mesh_crt_cfg_t       mesh_crt_cfg;

	aisp_ltm_cfg_t            ltm_cfg;
	aisp_ltm_enhc_cfg_t       ltm_enhc_cfg;
	aisp_wdr_cfg_t            wdr_cfg;
	aisp_wdr_blc_cfg_t        wdr_blc;
	aisp_wdr_fmt_cfg_t        wdr_fmt;
	aisp_wb_enhc_cfg_t        wb_enhc;
	aisp_lc_cfg_t             lc_cfg;
	aisp_lc_enhc_cfg_t        lc_enhc_cfg;
	aisp_dnlp_cfg_t           dnlp_cfg;
	aisp_dhz_cfg_t            dhz_cfg;
	aisp_dhz_enhc_cfg_t       dhz_enhc_cfg;
	aisp_peaking_cfg_t        peaking_cfg;

	aisp_misc_cfg_t           misc_cfg;
	aisp_custom_cfg_t         custom_cfg;
} aisp_param_t;

#endif // __AML_ISP_CFG_H__
