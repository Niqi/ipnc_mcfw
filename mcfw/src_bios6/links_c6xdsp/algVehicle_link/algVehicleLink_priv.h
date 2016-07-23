/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ALGVEHICLE_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALGVEHICLE_LINK_PRIV_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/links_c6xdsp/system/system_priv_c6xdsp.h>
#include <mcfw/interfaces/link_api/algVehicleLink.h>
#include <mcfw/src_bios6/links_c6xdsp/algVehicle_link/thPlateId/thPlateIdLink_priv.h>

#define ALG_NO_PROCESS

#define ALGVEHICLE_LINK_OBJ_MAX                     (SYSTEM_LINK_ID_ALGVEHICLE_COUNT)
#define ALGVEHICLE_LINK_MAX_OUT_FRAMES      		(SYSTEM_LINK_FRAMES_PER_CH)
#define ALGVEHICLE_LINK_EDMA3_QUEUE_ID_0			(0)
#define ALGVEHICLE_LINK_EDMA3_QUEUE_ID_1			(1)

typedef struct {
    Int16 bufId;
} AlgVehicleLink_perFrameConfig;

typedef struct AlgVehicleLink_OutObj {
    Utils_BitBufHndl bufOutQue;
    UInt32           numAllocPools;
} AlgVehicleLink_OutObj;

typedef struct AlgVehicleLink_Obj {
    UInt32 linkId;
    
    char name[32];
    
    Utils_TskHndl tsk;
    Utils_TskHndl processTsk;
    System_LinkInfo inTskInfo;
    System_LinkQueInfo inQueInfo;

    AlgVehicleLink_CreateParams createArgs;

    System_LinkInfo info;

    FVID2_Frame outFrames[ALGVEHICLE_LINK_MAX_OUT_FRAMES];
    AlgVehicleLink_perFrameConfig perFrameCfg[ALGVEHICLE_LINK_MAX_OUT_FRAMES];
    FVID2_Format bufferFrameFormat;
    Utils_BufHndl outFrameQue;  
    Utils_BufHndl processFrameQue;
    SystemCommon_edmaChPrm copyEdmaHndlYplane;
    SystemCommon_edmaChPrm copyEdmaHndlUVplane;

    AlgVehicleLink_OutObj outObj[ALGVEHICLE_LINK_MAX_OUT_QUE];

    AlgVehicleLink_ThPlateIdObj thPlateIdAlg;
    /**< handle to THPLATEID algorithm */    

    UInt32 memUsed[UTILS_MEM_MAXHEAPS];
} AlgVehicleLink_Obj;


/* Function prototypes */

void DM81XX_EDMA3_setParams(int chId, int dmaQueue, unsigned int srcAddr,
                            unsigned int dstAddr, unsigned short edmaWidth,
                            unsigned short edmaHeight, short srcLineOffset,
                            short dstLineOffset);

void DM81XX_EDMA3_triggerTransfer(int chId,
								unsigned int dstAddr,
                            	unsigned short edmaWidth,
                            	unsigned short edmaHeight);


Int32 AlgVehicleLink_algCreate(AlgVehicleLink_Obj * pObj, AlgVehicleLink_CreateParams * pPrm);
Int32 AlgVehicleLink_algProcessData(AlgVehicleLink_Obj * pObj);
Int32 AlgVehicleLink_algDelete(AlgVehicleLink_Obj * pObj);
Int32 AlgVehicleLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList);
Int32 AlgVehicleLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList);
Int32 AlgVehicleLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info);
Int32 AlgVehicleLink_algCopyFrames(AlgVehicleLink_Obj * pObj);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

