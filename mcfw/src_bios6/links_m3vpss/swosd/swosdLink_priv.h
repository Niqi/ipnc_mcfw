/** ==================================================================
 *  @file   swosdLink_priv.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/swosd/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SWOSD_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SWOSD_LINK_PRIV_H_

#include <mcfw/src_bios6/links_m3vpss/system/system_priv_m3vpss.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/src_bios6/links_m3vpss/alg/sw_osd/swosd_priv.h>
#include <mcfw/src_bios6/links_m3vpss/alg/common/ti_draw.h>
#include <mcfw/src_bios6/links_m3vpss/camera/cameraLink_priv.h>

#define SWOSD_LINK_OBJ_MAX			(1)

typedef struct TagSWOSD_Char {
    Int32   rows;
    Int32   width;
    Int32   pitch;
    Int8  	buffer[45*45];
    Int8  	pixel_mode;
    Int8  	len;
    Int8  	bmp_num;
    Int8  	text_len;
    Int8  	flag;
    Int32   bitmap_left;
    Int32   bitmap_top;
    Long  advance_x;
    Long  advance_y;
    Int32 textPosX;
    Int32 textPosY;
    Int32 fontsize;
    UInt32 ackNum;
}TSWOSD_Char;

typedef enum
{
	OSD_BMP_TYPE,		/* bmp类型 */
	OSD_AUX_TYPE,		/* 抓拍bmp类型 */
	OSD_INVAL_TYPE		/* 无效类型 */
}EOsd_Type;

typedef struct osd_Bmp_Global_Info_s {
	EOsd_Type   osdType;
	Int32       recvFinishFlag;				/* 接收完成标志 */
	UInt32  	timeInfo[7];				/* 数字时间信息 */
	TSWOSD_Char *bmpTimeInfo;					/* bmp时间信息 */
	TSWOSD_Char *auxBmpTimeInfo;
	TSWOSD_Char *thirdChannelBmpInfo;
	TSWOSD_Char *osdInfo;					/* 主码流字符串图片信息 */
	TSWOSD_Char *auxOsdInfo;				/* 最后保存的辅码流的图片信息指针 */
}TOsd_Global_Info;

typedef struct {
    Utils_TskHndl tsk;
    SwosdLink_CreateParams createArgs;

    System_LinkInfo info;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;

    Utils_BufHndl outFrameQue;

    UInt32 minLatency;
    /* minimum capture/source to SW Mosaic output latency in msecs */

    UInt32 maxLatency;
    /* maximum capture/source to SW Mosaic output latency in msecs */
} SwosdLink_Obj;

/* ===================================================================
 *  @func     DM812X_FD_Draw                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DM812X_FD_Draw(int streamId, Ptr frameAddrY, Ptr frameAddrUV,
                     UInt32 frameWidth, UInt32 frameHeight,
                     UInt32 frameDataFormat, UInt32 framePitch,
                     UInt32 codingFormat);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
