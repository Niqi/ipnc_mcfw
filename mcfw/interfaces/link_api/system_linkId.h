/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup SYSTEM_LINK_API

    @{
*/

/**
    \file system_linkId.h
    \brief  System Link ID's
*/

#ifndef _SYSTEM_LINK_ID_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_LINK_ID_H_

#define SYSTEM_PROC_DSP         (0)
#define SYSTEM_PROC_M3VIDEO     (1)
#define SYSTEM_PROC_M3VPSS      (2)
#define SYSTEM_PROC_HOSTA8      (3)
#define SYSTEM_PROC_MAX         (4)
#define SYSTEM_PROC_INVALID     (0xFFFF)

#define SYSTEM_MAKE_LINK_ID(p, x) (((p) <<28) | ((x) & 0x0FFFFFFF))
#define SYSTEM_GET_LINK_ID(x)     ((x) & 0x0FFFFFFF)
#define SYSTEM_GET_PROC_ID(x)     (((x) & ~0x0FFFFFFF)>>28)

#define VPSS_LINK(x)            SYSTEM_MAKE_LINK_ID(SYSTEM_PROC_M3VPSS , (x))
#define VIDEO_LINK(x)           SYSTEM_MAKE_LINK_ID(SYSTEM_PROC_M3VIDEO, (x))
#define DSP_LINK(x)             SYSTEM_MAKE_LINK_ID(SYSTEM_PROC_DSP    , (x))
#define HOST_LINK(x)            SYSTEM_MAKE_LINK_ID(SYSTEM_PROC_HOSTA8 , (x))



/* link ID's */

#define SYSTEM_LINK_ID_M3VPSS               VPSS_LINK(SYSTEM_LINK_ID_MAX-1)
#define SYSTEM_LINK_ID_M3VIDEO              VIDEO_LINK(SYSTEM_LINK_ID_MAX-1)
#define SYSTEM_LINK_ID_DSP                  DSP_LINK(SYSTEM_LINK_ID_MAX-1)
#define SYSTEM_LINK_ID_HOST                 HOST_LINK(SYSTEM_LINK_ID_MAX-1)


#define SYSTEM_LINK_ID_IPC_OUT_M3_0    (0)
#define SYSTEM_LINK_ID_IPC_OUT_M3_1    (1)

#define SYSTEM_LINK_ID_IPC_IN_M3_0     (2)
#define SYSTEM_LINK_ID_IPC_IN_M3_1     (3)

#define SYSTEM_LINK_ID_IPC_OUT_0       (4)
#define SYSTEM_LINK_ID_IPC_OUT_1       (5)

#define SYSTEM_LINK_ID_IPC_IN_0        (6)
#define SYSTEM_LINK_ID_IPC_IN_1        (7)

#define SYSTEM_LINK_ID_NULL_0          (8)
#define SYSTEM_LINK_ID_NULL_1          (9)
#define SYSTEM_LINK_ID_NULL_2          (10)

#define SYSTEM_LINK_ID_NULL_SRC_0      (11)
#define SYSTEM_LINK_ID_NULL_SRC_1      (12)
#define SYSTEM_LINK_ID_NULL_SRC_2      (13)

#define SYSTEM_LINK_ID_DUP_0           (14)
#define SYSTEM_LINK_ID_DUP_1           (15)
#define SYSTEM_LINK_ID_DUP_2           (16)
#define SYSTEM_LINK_ID_DUP_3           (17)
#define SYSTEM_LINK_ID_DUP_4           (18)

#define SYSTEM_LINK_ID_MERGE_0         (19)
#define SYSTEM_LINK_ID_MERGE_1         (20)
#define SYSTEM_LINK_ID_MERGE_2         (21)
#define SYSTEM_LINK_ID_MERGE_3         (22)
#define SYSTEM_LINK_ID_MERGE_4         (23)

#define SYSTEM_LINK_ID_IPC_FRAMES_OUT_0   (24)
#define SYSTEM_LINK_ID_IPC_FRAMES_OUT_1   (25)
#define SYSTEM_LINK_ID_IPC_FRAMES_OUT_2   (26)

#define SYSTEM_LINK_ID_IPC_FRAMES_IN_0    (27)
#define SYSTEM_LINK_ID_IPC_FRAMES_IN_1    (28)


#define SYSTEM_LINK_ID_IPC_BITS_OUT_0     (29)
#define SYSTEM_LINK_ID_IPC_BITS_OUT_1     (30)

#define SYSTEM_LINK_ID_IPC_BITS_IN_0      (31)
#define SYSTEM_LINK_ID_IPC_BITS_IN_1      (32)

#define SYSTEM_LINK_ID_SELECT_0           (33)
#define SYSTEM_LINK_ID_SELECT_1           (34)
#define SYSTEM_LINK_ID_SELECT_2           (35)
#define SYSTEM_LINK_ID_SELECT_3           (36)

#define SYSTEM_LINK_ID_VA                 (37)

#define SYSTEM_LINK_ID_MUX_0              (38)
#define SYSTEM_LINK_ID_MUX_1              (39)

#define SYSTEM_LINK_COMMON_LINKS_MAX_ID     (SYSTEM_LINK_ID_MUX_0 + 1)


#define SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0    VPSS_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_0)
#define SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1    VPSS_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_1)

#define SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0     VPSS_LINK(SYSTEM_LINK_ID_IPC_IN_M3_0 )
#define SYSTEM_VPSS_LINK_ID_IPC_IN_M3_1     VPSS_LINK(SYSTEM_LINK_ID_IPC_IN_M3_1 )

#define SYSTEM_VPSS_LINK_ID_IPC_OUT_0       VPSS_LINK(SYSTEM_LINK_ID_IPC_OUT_0   )
#define SYSTEM_VPSS_LINK_ID_IPC_OUT_1       VPSS_LINK(SYSTEM_LINK_ID_IPC_OUT_1   )

#define SYSTEM_VPSS_LINK_ID_IPC_IN_0        VPSS_LINK(SYSTEM_LINK_ID_IPC_IN_0    )
#define SYSTEM_VPSS_LINK_ID_IPC_IN_1        VPSS_LINK(SYSTEM_LINK_ID_IPC_IN_1    )

#define SYSTEM_VPSS_LINK_ID_IPC_BITS_IN_0  VPSS_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_0)
#define SYSTEM_VPSS_LINK_ID_IPC_BITS_IN_1  VPSS_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_1)

#define SYSTEM_VPSS_LINK_ID_IPC_BITS_OUT_0  VPSS_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_0)
#define SYSTEM_VPSS_LINK_ID_IPC_BITS_OUT_1  VPSS_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_1)

#define SYSTEM_VPSS_LINK_ID_NULL_0          VPSS_LINK(SYSTEM_LINK_ID_NULL_0      )
#define SYSTEM_VPSS_LINK_ID_NULL_1          VPSS_LINK(SYSTEM_LINK_ID_NULL_1      )
#define SYSTEM_VPSS_LINK_ID_NULL_2          VPSS_LINK(SYSTEM_LINK_ID_NULL_2      )
#define SYSTEM_VPSS_LINK_ID_NULL_3          VPSS_LINK(SYSTEM_LINK_ID_NULL_3      )

#define SYSTEM_VPSS_LINK_ID_NULL_SRC_0      VPSS_LINK(SYSTEM_LINK_ID_NULL_SRC_0  )
#define SYSTEM_VPSS_LINK_ID_NULL_SRC_1      VPSS_LINK(SYSTEM_LINK_ID_NULL_SRC_1  )
#define SYSTEM_VPSS_LINK_ID_NULL_SRC_2      VPSS_LINK(SYSTEM_LINK_ID_NULL_SRC_2  )

#define SYSTEM_VPSS_LINK_ID_DUP_0           VPSS_LINK(SYSTEM_LINK_ID_DUP_0       )
#define SYSTEM_VPSS_LINK_ID_DUP_1           VPSS_LINK(SYSTEM_LINK_ID_DUP_1       )
#define SYSTEM_VPSS_LINK_ID_DUP_2           VPSS_LINK(SYSTEM_LINK_ID_DUP_2       )
#define SYSTEM_VPSS_LINK_ID_DUP_3           VPSS_LINK(SYSTEM_LINK_ID_DUP_3       )
#define SYSTEM_VPSS_LINK_ID_DUP_4           VPSS_LINK(SYSTEM_LINK_ID_DUP_4       )

#define SYSTEM_VPSS_LINK_ID_MERGE_0         VPSS_LINK(SYSTEM_LINK_ID_MERGE_0     )
#define SYSTEM_VPSS_LINK_ID_MERGE_1         VPSS_LINK(SYSTEM_LINK_ID_MERGE_1     )
#define SYSTEM_VPSS_LINK_ID_MERGE_2         VPSS_LINK(SYSTEM_LINK_ID_MERGE_2     )
#define SYSTEM_VPSS_LINK_ID_MERGE_3         VPSS_LINK(SYSTEM_LINK_ID_MERGE_3     )
#define SYSTEM_VPSS_LINK_ID_MERGE_4         VPSS_LINK(SYSTEM_LINK_ID_MERGE_4     )

#define SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0  VPSS_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_0)
#define SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_1  VPSS_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_1)

#define SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0  VPSS_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_0)
#define SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_1  VPSS_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_1)
#define SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_2  VPSS_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_2)

#define SYSTEM_VPSS_LINK_ID_SELECT_0           VPSS_LINK(SYSTEM_LINK_ID_SELECT_0       )
#define SYSTEM_VPSS_LINK_ID_SELECT_1           VPSS_LINK(SYSTEM_LINK_ID_SELECT_1       )
#define SYSTEM_VPSS_LINK_ID_SELECT_2           VPSS_LINK(SYSTEM_LINK_ID_SELECT_2       )
#define SYSTEM_VPSS_LINK_ID_SELECT_3           VPSS_LINK(SYSTEM_LINK_ID_SELECT_3       )

#define SYSTEM_VPSS_LINK_ID_VA                  VPSS_LINK(SYSTEM_LINK_ID_VA)

#define SYSTEM_VPSS_LINK_ID_MUX_0               VPSS_LINK(SYSTEM_LINK_ID_MUX_0)
#define SYSTEM_VPSS_LINK_ID_MUX_1               VPSS_LINK(SYSTEM_LINK_ID_MUX_1)



#define SYSTEM_LINK_ID_CAPTURE                  VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+1)

#if (SYSTEM_GET_LINK_ID(SYSTEM_LINK_ID_CAPTURE) < (SYSTEM_LINK_COMMON_LINKS_MAX_ID + 1))
#error "System Core Private Link ID overlaps with common linkID. Update Core private link ID"
#endif

#define SYSTEM_LINK_ID_NSF_0                VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+2)
#define SYSTEM_LINK_ID_NSF_1                VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+3)

#define SYSTEM_LINK_ID_DEI_HQ_0             VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+4)
#define SYSTEM_LINK_ID_DEI_HQ_1             VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+5)

#define SYSTEM_LINK_ID_DEI_0                VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+6)
#define SYSTEM_LINK_ID_DEI_1                VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+7)

#define SYSTEM_LINK_ID_DISPLAY_0            VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+8)
#define SYSTEM_LINK_ID_DISPLAY_1            VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+9)
#define SYSTEM_LINK_ID_DISPLAY_2            VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+10)


#define SYSTEM_LINK_ID_GRPX_0               VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+11)
#define SYSTEM_LINK_ID_GRPX_1               VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+12)
#define SYSTEM_LINK_ID_GRPX_2               VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+13)

#define SYSTEM_LINK_ID_DISPLAY_FIRST        (SYSTEM_LINK_ID_DISPLAY_0)
#define SYSTEM_LINK_ID_DISPLAY_LAST         (SYSTEM_LINK_ID_DISPLAY_2)
#define SYSTEM_LINK_ID_DISPLAY_COUNT        ((SYSTEM_LINK_ID_DISPLAY_LAST - \
                                              SYSTEM_LINK_ID_DISPLAY_FIRST) + 1)

#define SYSTEM_LINK_ID_SW_MS_MULTI_INST_0   VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+14)
#define SYSTEM_LINK_ID_SW_MS_MULTI_INST_1   VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+15)
#define SYSTEM_LINK_ID_SW_MS_MULTI_INST_2   VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+16)
#define SYSTEM_LINK_ID_SW_MS_MULTI_INST_3   VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+17)

#define SYSTEM_LINK_ID_SCLR_INST_0          VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+18)
#define SYSTEM_LINK_ID_SCLR_INST_1          VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+19)
#define SYSTEM_LINK_ID_SCLR_INST_2          VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+20)

#define SYSTEM_LINK_ID_SCLR_START           (SYSTEM_LINK_ID_SCLR_INST_0)
#define SYSTEM_LINK_ID_SCLR_END             (SYSTEM_LINK_ID_SCLR_INST_2)
#define SYSTEM_LINK_ID_SCLR_COUNT           (SYSTEM_LINK_ID_SCLR_END -         \
                                             SYSTEM_LINK_ID_SCLR_START) + 1
                                             
#define SYSTEM_LINK_ID_CAMERA               VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+22)
#define SYSTEM_LINK_ID_MJPEG                VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+23)

#define SYSTEM_LINK_ID_FD					VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+24)

#define SYSTEM_LINK_ID_SWOSD_0		        VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+25)
#define SYSTEM_LINK_ID_SWOSD_1              VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+26)

//#define SYSTEM_LINK_ID_VSTAB_0         		VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+27)
//#define SYSTEM_LINK_ID_VSTAB_1         		VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+28)

#define SYSTEM_LINK_ID_VNF					VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+29)

#define SYSTEM_LINK_ID_ISP_0				VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+30)

#define SYSTEM_LINK_ID_GLBCE				VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+31)

#define SYSTEM_LINK_ID_WDR					VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+32)

#define SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_0  VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID + 33)
#define SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_1  VPSS_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID + 34)

#define SYSTEM_LINK_ID_MP_SCLR_START        (SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_0)
#define SYSTEM_LINK_ID_MP_SCLR_END          (SYSTEM_VPSS_LINK_ID_MP_SCLR_INST_1)
#define SYSTEM_LINK_ID_MP_SCLR_COUNT        (SYSTEM_LINK_ID_MP_SCLR_END -         \
                                             SYSTEM_LINK_ID_MP_SCLR_START) + 1

#define SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0   VIDEO_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_0)
#define SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_1   VIDEO_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_1)

#define SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0    VIDEO_LINK(SYSTEM_LINK_ID_IPC_IN_M3_0 )
#define SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1    VIDEO_LINK(SYSTEM_LINK_ID_IPC_IN_M3_1 )

#define SYSTEM_VIDEO_LINK_ID_IPC_OUT_0      VIDEO_LINK(SYSTEM_LINK_ID_IPC_OUT_0   )
#define SYSTEM_VIDEO_LINK_ID_IPC_OUT_1      VIDEO_LINK(SYSTEM_LINK_ID_IPC_OUT_1   )

#define SYSTEM_VIDEO_LINK_ID_IPC_IN_0       VIDEO_LINK(SYSTEM_LINK_ID_IPC_IN_0    )
#define SYSTEM_VIDEO_LINK_ID_IPC_IN_1       VIDEO_LINK(SYSTEM_LINK_ID_IPC_IN_1    )

#define SYSTEM_VIDEO_LINK_ID_IPC_BITS_IN_0  VIDEO_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_0)
#define SYSTEM_VIDEO_LINK_ID_IPC_BITS_IN_1  VIDEO_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_1)

#define SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0  VIDEO_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_0)
#define SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_1  VIDEO_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_1)


#define SYSTEM_VIDEO_LINK_ID_NULL_0         VIDEO_LINK(SYSTEM_LINK_ID_NULL_0      )
#define SYSTEM_VIDEO_LINK_ID_NULL_1         VIDEO_LINK(SYSTEM_LINK_ID_NULL_1      )
#define SYSTEM_VIDEO_LINK_ID_NULL_2         VIDEO_LINK(SYSTEM_LINK_ID_NULL_2      )

#define SYSTEM_VIDEO_LINK_ID_NULL_SRC_0     VIDEO_LINK(SYSTEM_LINK_ID_NULL_SRC_0  )
#define SYSTEM_VIDEO_LINK_ID_NULL_SRC_1     VIDEO_LINK(SYSTEM_LINK_ID_NULL_SRC_1  )
#define SYSTEM_VIDEO_LINK_ID_NULL_SRC_2     VIDEO_LINK(SYSTEM_LINK_ID_NULL_SRC_2  )

#define SYSTEM_VIDEO_LINK_ID_DUP_0          VIDEO_LINK(SYSTEM_LINK_ID_DUP_0       )
#define SYSTEM_VIDEO_LINK_ID_DUP_1          VIDEO_LINK(SYSTEM_LINK_ID_DUP_1       )
#define SYSTEM_VIDEO_LINK_ID_DUP_2          VIDEO_LINK(SYSTEM_LINK_ID_DUP_2       )
#define SYSTEM_VIDEO_LINK_ID_DUP_3          VIDEO_LINK(SYSTEM_LINK_ID_DUP_3       )

#define SYSTEM_VIDEO_LINK_ID_MERGE_0        VIDEO_LINK(SYSTEM_LINK_ID_MERGE_0     )
#define SYSTEM_VIDEO_LINK_ID_MERGE_1        VIDEO_LINK(SYSTEM_LINK_ID_MERGE_1     )
#define SYSTEM_VIDEO_LINK_ID_MERGE_2        VIDEO_LINK(SYSTEM_LINK_ID_MERGE_2     )
#define SYSTEM_VIDEO_LINK_ID_MERGE_3        VIDEO_LINK(SYSTEM_LINK_ID_MERGE_3     )
#define SYSTEM_VIDEO_LINK_ID_MERGE_4        VIDEO_LINK(SYSTEM_LINK_ID_MERGE_4     )

#define SYSTEM_VIDEO_LINK_ID_SELECT_0          VIDEO_LINK(SYSTEM_LINK_ID_SELECT_0       )
#define SYSTEM_VIDEO_LINK_ID_SELECT_1          VIDEO_LINK(SYSTEM_LINK_ID_SELECT_1       )
#define SYSTEM_VIDEO_LINK_ID_SELECT_2          VIDEO_LINK(SYSTEM_LINK_ID_SELECT_2       )
#define SYSTEM_VIDEO_LINK_ID_SELECT_3          VIDEO_LINK(SYSTEM_LINK_ID_SELECT_3       )

#define SYSTEM_LINK_ID_VENC_0               VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+1)
#if (SYSTEM_GET_LINK_ID(SYSTEM_LINK_ID_VENC_0) < (SYSTEM_LINK_COMMON_LINKS_MAX_ID + 1))
#error "System Core Private Link ID overlaps with common linkID. Update Core private link ID"
#endif
#define SYSTEM_LINK_ID_VENC_1               VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+2)


#define SYSTEM_LINK_ID_VENC_START           (SYSTEM_LINK_ID_VENC_0)
#define SYSTEM_LINK_ID_VENC_END             (SYSTEM_LINK_ID_VENC_1)
#define SYSTEM_LINK_ID_VENC_COUNT           (SYSTEM_LINK_ID_VENC_END -         \
                                             SYSTEM_LINK_ID_VENC_START) + 1

#define SYSTEM_LINK_ID_VDEC_0               VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+3)
#define SYSTEM_LINK_ID_VDEC_1               VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+4)

#define SYSTEM_LINK_ID_VSTAB_0         		VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+5)
#define SYSTEM_LINK_ID_VSTAB_1         		VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+6)

#define SYSTEM_LINK_ID_MCTNF         		VIDEO_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+7)

#define SYSTEM_LINK_ID_VDEC_START           (SYSTEM_LINK_ID_VDEC_0)
#define SYSTEM_LINK_ID_VDEC_END             (SYSTEM_LINK_ID_VDEC_1)
#define SYSTEM_LINK_ID_VDEC_COUNT           (SYSTEM_LINK_ID_VDEC_END -         \
                                             SYSTEM_LINK_ID_VDEC_START) + 1

#define SYSTEM_DSP_LINK_ID_IPC_OUT_M3_0    DSP_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_0)
#define SYSTEM_DSP_LINK_ID_IPC_OUT_M3_1    DSP_LINK(SYSTEM_LINK_ID_IPC_OUT_M3_1)

#define SYSTEM_DSP_LINK_ID_IPC_IN_M3_0     DSP_LINK(SYSTEM_LINK_ID_IPC_IN_M3_0 )
#define SYSTEM_DSP_LINK_ID_IPC_IN_M3_1     DSP_LINK(SYSTEM_LINK_ID_IPC_IN_M3_1 )

#define SYSTEM_DSP_LINK_ID_IPC_OUT_0       DSP_LINK(SYSTEM_LINK_ID_IPC_OUT_0   )
#define SYSTEM_DSP_LINK_ID_IPC_OUT_1       DSP_LINK(SYSTEM_LINK_ID_IPC_OUT_1   )

#define SYSTEM_DSP_LINK_ID_IPC_IN_0        DSP_LINK(SYSTEM_LINK_ID_IPC_IN_0    )
#define SYSTEM_DSP_LINK_ID_IPC_IN_1        DSP_LINK(SYSTEM_LINK_ID_IPC_IN_1    )

#define SYSTEM_DSP_LINK_ID_NULL_0          DSP_LINK(SYSTEM_LINK_ID_NULL_0      )
#define SYSTEM_DSP_LINK_ID_NULL_1          DSP_LINK(SYSTEM_LINK_ID_NULL_1      )
#define SYSTEM_DSP_LINK_ID_NULL_2          DSP_LINK(SYSTEM_LINK_ID_NULL_2      )
#define SYSTEM_DSP_LINK_ID_NULL_3          DSP_LINK(SYSTEM_LINK_ID_NULL_3      )

#define SYSTEM_DSP_LINK_ID_NULL_SRC_0      DSP_LINK(SYSTEM_LINK_ID_NULL_SRC_0  )
#define SYSTEM_DSP_LINK_ID_NULL_SRC_1      DSP_LINK(SYSTEM_LINK_ID_NULL_SRC_1  )
#define SYSTEM_DSP_LINK_ID_NULL_SRC_2      DSP_LINK(SYSTEM_LINK_ID_NULL_SRC_2  )
//#define SYSTEM_DSP_LINK_ID_NULL_SRC_3      DSP_LINK(SYSTEM_LINK_ID_NULL_SRC_3  )

#define SYSTEM_DSP_LINK_ID_DUP_0           DSP_LINK(SYSTEM_LINK_ID_DUP_0       )
#define SYSTEM_DSP_LINK_ID_DUP_1           DSP_LINK(SYSTEM_LINK_ID_DUP_1       )
#define SYSTEM_DSP_LINK_ID_DUP_2           DSP_LINK(SYSTEM_LINK_ID_DUP_2       )
#define SYSTEM_DSP_LINK_ID_DUP_3           DSP_LINK(SYSTEM_LINK_ID_DUP_3       )

#define SYSTEM_DSP_LINK_ID_MERGE_0         DSP_LINK(SYSTEM_LINK_ID_MERGE_0     )
#define SYSTEM_DSP_LINK_ID_MERGE_1         DSP_LINK(SYSTEM_LINK_ID_MERGE_1     )
#define SYSTEM_DSP_LINK_ID_MERGE_2         DSP_LINK(SYSTEM_LINK_ID_MERGE_2     )
#define SYSTEM_DSP_LINK_ID_MERGE_3         DSP_LINK(SYSTEM_LINK_ID_MERGE_3     )
#define SYSTEM_DSP_LINK_ID_MERGE_4         DSP_LINK(SYSTEM_LINK_ID_MERGE_4     )

#define SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0  DSP_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_0)
#define SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_1  DSP_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_1)

#define SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0  DSP_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_0)
#define SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_1  DSP_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_1)
#define SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_2  DSP_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_2)

#define SYSTEM_DSP_LINK_ID_IPC_BITS_OUT_0  DSP_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_0)
#define SYSTEM_DSP_LINK_ID_IPC_BITS_OUT_1  DSP_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_1)

#define SYSTEM_DSP_LINK_ID_SELECT_0           DSP_LINK(SYSTEM_LINK_ID_SELECT_0       )
#define SYSTEM_DSP_LINK_ID_SELECT_1           DSP_LINK(SYSTEM_LINK_ID_SELECT_1       )
#define SYSTEM_DSP_LINK_ID_SELECT_2           DSP_LINK(SYSTEM_LINK_ID_SELECT_2       )
#define SYSTEM_DSP_LINK_ID_SELECT_3           DSP_LINK(SYSTEM_LINK_ID_SELECT_3       )

#define SYSTEM_DSP_LINK_ID_VA				  DSP_LINK(SYSTEM_LINK_ID_VA)

#define SYSTEM_LINK_ID_ALG_0               DSP_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+1)

#if (SYSTEM_GET_LINK_ID(SYSTEM_LINK_ID_ALG_0) < (SYSTEM_LINK_COMMON_LINKS_MAX_ID + 1))
#error "System Core Private Link ID overlaps with common linkID. Update Core private link ID"
#endif
#define SYSTEM_LINK_ID_ALG_1               DSP_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+2)

#define SYSTEM_LINK_ID_ALG_START           (SYSTEM_LINK_ID_ALG_0)
#define SYSTEM_LINK_ID_ALG_END             (SYSTEM_LINK_ID_ALG_1)
#define SYSTEM_LINK_ID_ALG_COUNT           (SYSTEM_LINK_ID_ALG_END -         \
                                             SYSTEM_LINK_ID_ALG_START) + 1

#define SYSTEM_LINK_ID_OSD_0               DSP_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+3)
#define SYSTEM_LINK_ID_RVM_0               DSP_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+4)

#define SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0  HOST_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_0)
#define SYSTEM_HOST_LINK_ID_IPC_BITS_IN_1  HOST_LINK(SYSTEM_LINK_ID_IPC_BITS_IN_1)

#define SYSTEM_HOST_LINK_ID_IPC_BITS_OUT_0  HOST_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_0)
#define SYSTEM_HOST_LINK_ID_IPC_BITS_OUT_1  HOST_LINK(SYSTEM_LINK_ID_IPC_BITS_OUT_1)

#define SYSTEM_HOST_LINK_ID_IPC_FRAMES_IN_0 HOST_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_0)
#define SYSTEM_HOST_LINK_ID_IPC_FRAMES_IN_1 HOST_LINK(SYSTEM_LINK_ID_IPC_FRAMES_IN_1)

#define SYSTEM_HOST_LINK_ID_IPC_FRAMES_OUT_0 HOST_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_0)
#define SYSTEM_HOST_LINK_ID_IPC_FRAMES_OUT_1 HOST_LINK(SYSTEM_LINK_ID_IPC_FRAMES_OUT_1)

#define SYSTEM_LINK_ID_GLBCE_SUPPORT_0         		HOST_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+4)
#define SYSTEM_LINK_ID_GLBCE_SUPPORT_1         		HOST_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID+5)

#define SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_BASE          HOST_LINK(SYSTEM_LINK_COMMON_LINKS_MAX_ID + 1)
#define SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_FROM_DSP      (SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_BASE + SYSTEM_PROC_DSP)
#define SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_FROM_VIDEOM3  (SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_BASE + SYSTEM_PROC_M3VIDEO)
#define SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_FROM_VPSSM3   (SYSTEM_HOST_LINK_ID_EXCEPTION_NOTIFY_BASE + SYSTEM_PROC_M3VPSS)

#define SYSTEM_LINK_ID_MAX                  (76)

#define SYSTEM_LINK_ID_INVALID              (0xFFFFFFFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
