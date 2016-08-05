/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "algVehicleLink_priv.h"

#pragma DATA_ALIGN(gAlgVehicleLink_tskStack, 32)
#pragma DATA_SECTION(gAlgVehicleLink_tskStack, ".bss:taskStackSection")
UInt8 gAlgVehicleLink_tskStack[ALGVEHICLE_LINK_OBJ_MAX][ALGVEHICLE_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gAlgVehicleLink_processTskStack, 32)
#pragma DATA_SECTION(gAlgVehicleLink_processTskStack, ".bss:taskStackSection")
UInt8 gAlgVehicleLink_processTskStack[ALGVEHICLE_LINK_OBJ_MAX][ALGVEHICLE_LINK_TSK_STACK_SIZE];

AlgVehicleLink_Obj gAlgVehicleLink_obj[ALGVEHICLE_LINK_OBJ_MAX];

#define SYSTEM_DEBUG_ALGVEHICLE

/* AlgVehicle link create */

/* ===================================================================
 *  @func     AlgVehicleLink_create                                               
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
Int32 AlgVehicleLink_create(AlgVehicleLink_Obj * pObj,
                    AlgVehicleLink_CreateParams * pPrm)
{
    Int32 status = FVID2_SOK;
    Int32 frameId;
    FVID2_Frame *pFrame;
	UInt32 chId;
	System_LinkQueInfo *pOutQueInfo;
	System_LinkChInfo *pInChInfo, *pOutChInfo;	
	
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the link info from prev link */
    status = System_linkGetInfo(pObj->createArgs.inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pObj->createArgs.inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pObj->createArgs.inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* No of channels assumed is > 1 */
    UTILS_assert(pObj->inQueInfo.numCh == ALGVEHICLE_LINK_THPLATEID_MAX_CH);
	
    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);
	
	/* Create the process buffers */
	status = Utils_bufCreate(&pObj->processFrameQue, FALSE, FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Allocate memory for the buffers */
    pObj->bufferFrameFormat.width      = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.height     = pObj->inQueInfo.chInfo[0].height;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    pObj->bufferFrameFormat.pitch[0]   = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.pitch[1]   = pObj->inQueInfo.chInfo[0].width;

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
                                 pObj->outFrames,          // pFrame
                                 ALGVEHICLE_LINK_MAX_OUT_FRAMES);  // numFrames
    UTILS_assert(status == FVID2_SOK);
	
    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < ALGVEHICLE_LINK_MAX_OUT_FRAMES; frameId++)
    {
        pFrame = &pObj->outFrames[frameId];
		pObj->frameInfo[frameId].rtChInfoUpdate = TRUE;
		pObj->frameInfo[frameId].rtChInfo.width = pObj->inQueInfo.chInfo[0].width;
		pObj->frameInfo[frameId].rtChInfo.height = pObj->inQueInfo.chInfo[0].height;
		pObj->frameInfo[frameId].rtChInfo.pitch[0] = pObj->inQueInfo.chInfo[0].pitch[0];
		pObj->frameInfo[frameId].rtChInfo.pitch[1] = pObj->inQueInfo.chInfo[0].pitch[1];
		pObj->frameInfo[frameId].rtChInfo.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
		pFrame->appData = &pObj->frameInfo[frameId];		
		pObj->perFrameCfg[frameId].bufId = (frameId + 1);
		pFrame->perFrameCfg = (AlgVehicleLink_perFrameConfig*)&pObj->perFrameCfg[frameId];
#ifdef ALG_NO_PROCESS
        status = Utils_bufPutEmptyFrame(&pObj->outFrameQue, pFrame);
#else
		status = Utils_bufPutEmptyFrame(&pObj->processFrameQue, pFrame);
#endif
        UTILS_assert(status == FVID2_SOK);
    }	

	/* Populate the link's info structure */
    pObj->info.numQue = 1;
    pOutQueInfo = &pObj->info.queInfo[0];
    pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;
    for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];
        pOutChInfo = &pOutQueInfo->chInfo[chId];
        memcpy(pOutChInfo, pInChInfo, sizeof(System_LinkChInfo));
    }	

#ifdef SYSTEM_DEBUG_ALGVEHICLE
    Vps_printf(" %d: ALGVEHICLE LINK   : Create Done !!!\n", Clock_getTicks());
#endif

    return status;
}


/* AlgVehicle link delete */

/* ===================================================================
 *  @func     AlgVehicleLink_delete                                               
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
Int32 AlgVehicleLink_delete(AlgVehicleLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

	/* Free the frame memory */
	status = Utils_memFrameFree(&pObj->bufferFrameFormat,  		// pFormat
								pObj->outFrames,        	// pFrame
								ALGVEHICLE_LINK_MAX_OUT_FRAMES);    	// numFrames

	UTILS_assert(status == FVID2_SOK);

	/* Delete the buffers */
	status = Utils_bufDelete(&pObj->outFrameQue);
	UTILS_assert(status == FVID2_SOK);
	
	status = Utils_bufDelete(&pObj->processFrameQue);
	UTILS_assert(status == FVID2_SOK);	

#ifdef SYSTEM_DEBUG_ALGVEHICLE
    Vps_printf(" %d: ALGVEHICLE LINK   : Delete Done !!!\n", Clock_getTicks());
#endif

    return status;
}


Void AlgVehicleLink_tskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    AlgVehicleLink_Obj *pObj;
    UInt32 flushCmds[4];

    pObj = (AlgVehicleLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = AlgVehicleLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
    ackMsg = FALSE;

    Utils_tskSendCmd(&pObj->processTsk, SYSTEM_CMD_CREATE);

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);
                if(AlgVehicleLink_algCopyFrames(pObj) == FVID2_SOK)
                {
#ifndef ALG_NO_PROCESS
					Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_NEW_DATA);
#endif
                }
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_INIT_PLATEIDSDK:
                Utils_tskAckOrFreeMsg(pMsg, status);
                Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_INIT_PLATEIDSDK);
                break;  

            case ALGVEHICLE_LINK_THPLATEID_CMD_DEINIT_PLATEIDSDK:
                Utils_tskAckOrFreeMsg(pMsg, status);
                Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_DEINIT_PLATEIDSDK);
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_IMAGE_FORMAT:
                {
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].cImageFormat = params->cImageFormat;
                    pObj->thPlateIdAlg.chParams[0].bVertFlip = params->bVertFlip;
                    pObj->thPlateIdAlg.chParams[0].bDwordAligned = params->bDwordAligned;                
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_IMAGE_FORMAT);
                }
                break; 

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_PLATE_FORMAT:
                {
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].dFormat = params->dFormat;  
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_PLATE_FORMAT);
                }
                break;   

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOGTHRESHOLD:
                {                
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].nPlateLocate_Th = params->nPlateLocate_Th;  
                    pObj->thPlateIdAlg.chParams[0].nOCR_Th = params->nOCR_Th;  
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOGTHRESHOLD);
                }
                break;  

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_PROVINCE_ORDER:
                {
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    strcpy(&pObj->thPlateIdAlg.chParams[0].szProvince[0], &params->szProvince[0]);
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_PROVINCE_ORDER);
                }
                break;                

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENLARGE_MODE:
                {                
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].bEnlarge = params->bEnlarge;
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENLARGE_MODE);
                }
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_CONTRAST:
                {                
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].nContrast = params->nContrast;
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_CONTRAST);
                }
                break;     

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLE_LEAN_CORRECTION:
                {
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].bLeanCorrection= params->bLeanCorrection;
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLE_LEAN_CORRECTION);
                }
                break; 

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_SHADOW:
                {                
                    AlgVehicleLink_ThPlateIdChParams *params;
                    params = (AlgVehicleLink_ThPlateIdChParams *) Utils_msgGetPrm(pMsg);
                    pObj->thPlateIdAlg.chParams[0].bShadow= params->bShadow;
                    Utils_tskAckOrFreeMsg(pMsg, status);
                    Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_SHADOW);
                }
                break;                 

            case ALGVEHICLE_LINK_THPLATEID_CMD_PRINT_STATISTICS:
                Utils_tskAckOrFreeMsg(pMsg, status);
                Utils_tskSendCmd(&pObj->processTsk, ALGVEHICLE_LINK_THPLATEID_CMD_PRINT_STATISTICS);
                break;                  

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the va driver */
    AlgVehicleLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }
	
    Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_DELETE);

    return;
}


Void AlgVehicleLink_processTskMain(struct Utils_TskHndl *pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    AlgVehicleLink_Obj *pObj;
    UInt32 flushCmds[4];

    pObj = (AlgVehicleLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = AlgVehicleLink_algCreate(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);

                status = AlgVehicleLink_algProcessData(pObj);
                UTILS_assert(status == FVID2_SOK);
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_INIT_PLATEIDSDK:
                Utils_tskAckOrFreeMsg(pMsg, status);                    
                status = AlgVehicleLink_ThPlateIdInit(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;  

            case ALGVEHICLE_LINK_THPLATEID_CMD_DEINIT_PLATEIDSDK:
                Utils_tskAckOrFreeMsg(pMsg, status);                    
                status = AlgVehicleLink_ThPlateIdDeInit(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_IMAGE_FORMAT:
                Utils_tskAckOrFreeMsg(pMsg, status);                
                status = AlgVehicleLink_ThPlateIdAlgSetImageFormat(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break; 

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_PLATE_FORMAT:
                Utils_tskAckOrFreeMsg(pMsg, status);                
                status = AlgVehicleLink_ThPlateIdAlgSetEnbalePlateFormat(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;   

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOGTHRESHOLD: 
                Utils_tskAckOrFreeMsg(pMsg, status);                
                status = AlgVehicleLink_ThPlateIdAlgSetRecogThreshold(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;  

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_PROVINCE_ORDER:
                Utils_tskAckOrFreeMsg(pMsg, status);
                status = AlgVehicleLink_ThPlateIdAlgSetProvinceOrder(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;                

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENLARGE_MODE:
                Utils_tskAckOrFreeMsg(pMsg, status);                    
                status = AlgVehicleLink_ThPlateIdAlgSetEnlargeMode(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_CONTRAST:
                Utils_tskAckOrFreeMsg(pMsg, status);
                status = AlgVehicleLink_ThPlateIdAlgSetContrast(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;     

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLE_LEAN_CORRECTION:
                Utils_tskAckOrFreeMsg(pMsg, status);
                status = AlgVehicleLink_ThPlateIdAlgSetEnableLeanCorrection(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break; 

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_SHADOW:
                Utils_tskAckOrFreeMsg(pMsg, status);
                status = AlgVehicleLink_ThPlateIdAlgSetEnableShadow(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;         

            case ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOG_REGION:
                Utils_tskAckOrFreeMsg(pMsg, status);
                status = AlgVehicleLink_ThPlateIdAlgSetRecogRegion(&pObj->thPlateIdAlg);
                UTILS_assert(status == FVID2_SOK);
                break;          

            case ALGVEHICLE_LINK_THPLATEID_CMD_PRINT_STATISTICS:
                Utils_tskAckOrFreeMsg(pMsg, status);                
                status = AlgVehicleLink_ThPlateIdprintStatistics(&pObj->thPlateIdAlg, TRUE);
                UTILS_assert(status == FVID2_SOK);
                break;                  

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    AlgVehicleLink_algDelete(pObj);

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}


Int32 AlgVehicleLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    AlgVehicleLink_Obj *pObj;
    UInt32 objId;

	UInt32 procId = System_getSelfProcId();

    for (objId = 0; objId < ALGVEHICLE_LINK_OBJ_MAX; objId++)
    {
        pObj = &gAlgVehicleLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->linkId = SYSTEM_MAKE_LINK_ID(procId,SYSTEM_LINK_ID_ALGVEHICLE_0) + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames   = AlgVehicleLink_getFullBufs;
        linkObj.linkPutEmptyFrames  = AlgVehicleLink_putEmptyBufs;
        linkObj.getLinkInfo         = AlgVehicleLink_getInfo;

        sprintf(pObj->name, "ALGVEHICLE%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = Utils_tskCreate(&pObj->tsk,
                                 AlgVehicleLink_tskMain,
                                 ALGVEHICLE_LINK_TSK_PRI,
                                 gAlgVehicleLink_tskStack[objId],
                                 ALGVEHICLE_LINK_TSK_STACK_SIZE, pObj, pObj->name);
        UTILS_assert(status == FVID2_SOK);

        sprintf(pObj->name, "ALGVEHICLEPROCESS%d   ", objId);
        status = Utils_tskCreate(&pObj->processTsk,
                                 AlgVehicleLink_processTskMain,
                                 ALGVEHICLE_LINK_PROCESS_TSK_PRI,
                                 gAlgVehicleLink_processTskStack[objId],
                                 ALGVEHICLE_LINK_PROCESS_TSK_STACK_SIZE, pObj, pObj->name);
        UTILS_assert(status == FVID2_SOK);        

        
    }

    return status;
}


Int32 AlgVehicleLink_deInit()
{
    UInt32 objId;
    AlgVehicleLink_Obj *pObj;

    for (objId = 0; objId < ALGVEHICLE_LINK_OBJ_MAX; objId++)
    {
        pObj = &gAlgVehicleLink_obj[objId];

        Utils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}


Int32 AlgVehicleLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    AlgVehicleLink_Obj *pObj = (AlgVehicleLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}


Int32 AlgVehicleLink_getFullBufs(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    Int32 status = FVID2_SOK;
    AlgVehicleLink_Obj *pObj = (AlgVehicleLink_Obj *) pTsk->appData;
    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);
    return status;
}


Int32 AlgVehicleLink_putEmptyBufs(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList)
{
	Int32 status = FVID2_SOK;
    AlgVehicleLink_Obj *pObj = (AlgVehicleLink_Obj *) pTsk->appData;
#ifdef ALG_NO_PROCESS
	status = Utils_bufPutEmpty(&pObj->outFrameQue, pFrameList);
#else
	status = Utils_bufPutEmpty(&pObj->processFrameQue, pFrameList);
#endif
    UTILS_assert(status == FVID2_SOK);
    return status;
}

