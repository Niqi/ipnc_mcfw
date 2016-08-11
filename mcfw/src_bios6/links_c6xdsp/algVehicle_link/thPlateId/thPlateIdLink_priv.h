/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _THPLATEIDLINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _THPLATEIDLINK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algVehicleLink.h>

#include "thPlateIdAlg.h"

#define ALGVEHICLE_LINK_SETCONFIG_BITMASK_RECOG_AREA				(1)
#define ALGVEHICLE_LINK_SETCONFIG_BITMASK_TRIGG_INFO				(2)
#define ALGVEHICLE_LINK_SETCONFIG_BITMASK_PLATE_WIDTH				(3)
#define ALGVEHICLE_LINK_SETCONFIG_BITMASK_DEFAULT_PROVINCE			(4)
#define ALGVEHICLE_LINK_SETCONFIG_BITMASK_RESET_VALUE				(0xFFFFFFFF)



#define ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH          (SYSTEM_LINK_FRAMES_PER_CH)

#define ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES                 (ALGVEHICLE_LINK_THPLATEID_MAX_CH*ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH)


typedef struct AlgVehicleLink_ThPlateIdOutObj {
    Utils_BitBufHndl bufOutQue;
    UInt32 numAllocPools;
    Bitstream_Buf outBufs[ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES];
    UInt32 outNumBufs[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32 buf_size[UTILS_BITBUF_MAX_ALLOC_POOLS];
    UInt32 ch2poolMap[ALGVEHICLE_LINK_THPLATEID_MAX_CH];
} AlgVehicleLink_ThPlateIdOutObj;

/**
    \brief THPLATEID Channel parameters used to SET BEFORE CALLING PROCESS
*/
typedef struct
{
    UInt32 chId;

    UInt32 width;
    /**< Set the width (in pixels) of video frame that scd will process */

    UInt32 height;
    /**< Set the height (in pixels) of video frame that scd will process */

    UInt32 stride;
    /**< Set the video frame pitch/stride of the images in the video buffer*/

    void *curFrame;
    void *curFrameUV;
    void *pDspFrameBuf;
    /** Luma pointer to current frame */

    AlgLprPlateWidth plateWidth;
    char szProvince[16];	
    int nTrigMode;			// 识别结果输出触发模式
    int nTrigInterval;		// 相同车牌触发时间间隔	
    int nVehicleDirection;		// 车辆通过方向	
	
    unsigned char cImageFormat;
    unsigned char bVertFlip;
    unsigned char bDwordAligned;
    unsigned char bEnlarge;
    unsigned char nPlateLocate_Th;
    unsigned char nOCR_Th;
    unsigned char nContrast;
    unsigned char bLeanCorrection;
    unsigned char bShadow;
    //unsigned char bIsAutoSlope;
    //unsigned char nSlopeDetectRange;
    unsigned int dFormat;
    int pnMinFreeSRAM;
    int pnMinFreeSDRAM; 

    TH_RECT rcDetect;
    TH_RECT rcTrig;

} AlgVehicleLink_ThPlateIdchPrm;

typedef struct
{
	unsigned int chId;			// 通道ID
	char szProvince[16];		// 默认省份	
	int nTrigMode;			// 识别结果输出触发模式
	int nTrigInterval;		// 相同车牌触发时间间隔	
	int nPlateTypeSupt;			// 支持的车牌类型
	AlgLprPlateWidth plateWidth;
	int nVehicleDirection;		// 车辆通过方向
	TH_RECT recogArea;			// 识别区域//
	TH_RECT trigArea;			// 触发区域(虚拟线圈)
	
} AlgVehicleLink_ThPlateIdDynPrm;


typedef struct {
    UInt32 inFrameRecvCount;
    /**< input frame recevied from previous link */

    UInt32 inFrameUserSkipCount;
    /**< input frame rejected due mismatch in FID */

    UInt32 inFrameProcessCount;
    /**< input frames actually processed */
    
    UInt32 inFrameProcessTime;

    UInt32 statsStartTime;
    UInt32 maxProcessTime;
    UInt32 totalProcessTime;
    UInt32 processFrCnt;
    UInt32 totalFrameCount; 

    THPLATEIDALG_Result thPlateIdResult;

    Utils_frameSkipContext frameSkipCtx;
    /**< Data structure for frame skip to achieve expected output frame rate */    
} AlgVehicleLink_ThPlateIdChObj;

typedef struct AlgVehicleLink_ThPlateIdObj {
    System_LinkQueInfo * inQueInfo;

    AlgVehicleLink_ThPlateIdchPrm chParams[ALGVEHICLE_LINK_THPLATEID_MAX_CH];

    AlgVehicleLink_ThPlateIdChStatus thPlateIdChStat[ALGVEHICLE_LINK_THPLATEID_MAX_CH];

    AlgVehicleLink_ThPlateIdCreateParams createArgs;

    AlgVehicleLink_ThPlateIdChObj   chObj[ALGVEHICLE_LINK_THPLATEID_MAX_CH];

    AlgVehicleLink_ThPlateIdDynPrm thPlateIdDynParams[ALGVEHICLE_LINK_THPLATEID_MAX_CH];
    UInt32 setConfigBitMask;
    UInt32 getConfigFlag;	

    void *algHndl;
    /**< handle to ThPlateId algorithm */

} AlgVehicleLink_ThPlateIdObj;


Int32 AlgVehicleLink_ThPlateIdalgCreate(AlgVehicleLink_ThPlateIdObj * pObj);
Int32 AlgVehicleLink_ThPlateIdalgProcessData(AlgVehicleLink_ThPlateIdObj * pObj, FVID2_Frame *pFrame, Utils_BitBufHndl *bufOutQue);
Int32 AlgVehicleLink_ThPlateIdalgDelete(AlgVehicleLink_ThPlateIdObj * pObj);
Int32 AlgVehicleLink_ThPlateIdalgSetConfig(AlgVehicleLink_ThPlateIdObj * algObj);

Int32 AlgVehicleLink_ThPlateIdInit (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdDeInit (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetImageFormat (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetEnbalePlateFormat (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetRecogThreshold (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetProvinceOrder (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetEnlargeMode (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetContrast (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetEnableLeanCorrection (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetEnableShadow (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetRecogArea (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdAlgSetPlateWidth (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj);
Int32 AlgVehicleLink_ThPlateIdprintStatistics (AlgVehicleLink_ThPlateIdObj *pObj, Bool resetAfterPrint);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
