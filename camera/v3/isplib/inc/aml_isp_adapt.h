/*
 * Copyright (C) 2019-2024 Amlogic, Inc. All rights reserved.
 *
 * All information contained herein is Amlogic confidential.
 *
 * T_s software is provided to you pursuant to Software License Agreement
 * (SLA) with Amlogic Inc ("Amlogic"). T_s software may be used
 * only in accordance with the terms of t_s agreement.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification is strictly pro_bit without prior written permission from
 * Amlogic.
 *
 * TMBPS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF TMBPS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AML_COMM_ISP_ADAPT_H__
#define __AML_COMM_ISP_ADAPT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CCM_MATRIX_SIZE                 (12)

#pragma pack(1)

typedef struct {
    uint16_t u16Rgain;
    uint16_t u16Grgain;
    uint16_t u16Gbgain;
    uint16_t u16Bgain;
    uint16_t u16Saturation;
    uint16_t u16ColorTemp;
    uint16_t u16ColorTempDiff;
    int32_t au32CCM[CCM_MATRIX_SIZE];
} aml_isp_wb_info_attr;

typedef struct {
    uint32_t ae_converged;
    uint32_t ae_slight_change;
    int32_t  ae_sys_expos_log2;
    uint32_t ae_sys_ratio;
    uint32_t ae_sys_min_ratio;
    uint32_t ae_sys_expos_full;
    uint32_t ae_sns_expos_lines;
    uint32_t ae_sns_sexpos_lines;
    uint32_t ae_sns_vsexpos_lines;
    uint32_t ae_sns_vvsexpos_lines;
    int32_t  ae_sns_expo_log2;
    int32_t  ae_sns_shuttime;
    int32_t  ae_sns_again;
    int32_t  ae_sns_hc_again;
    int32_t  ae_sns_dgain;
    int32_t  ae_sns_hc_dgain;
    int32_t  ae_isp_gain;
    int32_t  ae_total_gain;
    int32_t  ae_lowlight_enh_ratio;
    uint32_t ae_daylight;
} aml_isp_exp_info_attr;

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __AML_COMM_ISP_ADAPT_H__ */
