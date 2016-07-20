/** ==================================================================
 *  @file   vaLink_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_c6xdsp/va/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "vaLink_priv.h"

#pragma DATA_ALIGN(gVaLink_tskStack, 32)
#pragma DATA_SECTION(gVaLink_tskStack, ".bss:taskStackSection")
UInt8 gVaLink_tskStack[VA_LINK_OBJ_MAX][VA_LINK_TSK_STACK_SIZE];

#pragma DATA_ALIGN(gVaLink_processTskStack, 32)
#pragma DATA_SECTION(gVaLink_processTskStack, ".bss:taskStackSection")
UInt8 gVaLink_processTskStack[VA_LINK_OBJ_MAX][VA_LINK_PROCESSTSK_STACK_SIZE];

VaLink_Obj gVaLink_obj[VA_LINK_OBJ_MAX];

#define VA_NO_PROCESS

/* Va link get info */
/* ===================================================================
 *  @func     VaLink_getInfo
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
Int32 VaLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* VA get full frames */
/* ===================================================================
 *  @func     VaLink_getFullFrames
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
Int32 VaLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    Int32 status;
    VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;
    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);
    UTILS_assert(status == FVID2_SOK);
    return FVID2_SOK;
}

/* Va put empty frames */
/* ===================================================================
 *  @func     VaLink_putEmptyFrames
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
Int32 VaLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList)
{
	Int32 status;
    VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;
#ifdef VA_NO_PROCESS
	status = Utils_bufPutEmpty(&pObj->outFrameQue, pFrameList);
#else
    status = Utils_bufPutEmpty(&pObj->processFrameQue, pFrameList);
#endif
    UTILS_assert(status == FVID2_SOK);
    return FVID2_SOK;
}


/* Va link create */
/* ===================================================================
 *  @func     VaLink_create
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
Int32 VaLink_create(VaLink_Obj * pObj,
                    VaLink_CreateParams * pPrm)
{
    Int32 frameId;
    Int32 status;
    FVID2_Frame *pFrame;
	UInt32 chId;
	System_LinkQueInfo *pOutQueInfo;
	System_LinkChInfo *pInChInfo, *pOutChInfo;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    pObj->receviedFrameCount = 0;

    /* Get the link info from prev link */
    status = System_linkGetInfo(pObj->createArgs.inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pObj->createArgs.inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pObj->createArgs.inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* No of channels assumed is 1 */
    UTILS_assert(pObj->inQueInfo.numCh == 1);

    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,FALSE,FALSE);
    UTILS_assert(status == FVID2_SOK);
	/* Create the process buffers */
	status = Utils_bufCreate(&pObj->processFrameQue,FALSE,FALSE);
    UTILS_assert(status == FVID2_SOK);

    /* Allocate memory for the buffers */
    pObj->bufferFrameFormat.width      = pObj->inQueInfo.chInfo[0].width;
    pObj->bufferFrameFormat.height     = pObj->inQueInfo.chInfo[0].height;
    pObj->bufferFrameFormat.dataFormat = pObj->inQueInfo.chInfo[0].dataFormat;
    pObj->bufferFrameFormat.pitch[0]   = pObj->inQueInfo.chInfo[0].pitch[0];
    pObj->bufferFrameFormat.pitch[1]   = pObj->inQueInfo.chInfo[0].pitch[1];

    status = Utils_memFrameAlloc(&pObj->bufferFrameFormat, // pFormat
                                 pObj->outFrames,          // pFrame
                                 VA_LINK_MAX_OUT_FRAMES);  // numFrames
    UTILS_assert(status == FVID2_SOK);

    /* Queue all the buffers to empty queue */
    for (frameId = 0; frameId < VA_LINK_MAX_OUT_FRAMES; frameId++)
    {
        pFrame = &pObj->outFrames[frameId];
		pObj->perFrameCfg[frameId].bufId = (frameId + 1);
		pFrame->perFrameCfg = (VaLink_perFrameConfig*)&pObj->perFrameCfg[frameId];
	#ifdef VA_NO_PROCESS
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

#ifdef SYSTEM_DEBUG_VA
	Vps_printf(" %d: VA   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Va link delete */

/* ===================================================================
 *  @func     VaLink_delete
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
Int32 VaLink_delete(VaLink_Obj * pObj)
{
    Int32 status;

	/* Free the frame memory */
	status = Utils_memFrameFree(&pObj->bufferFrameFormat,  		// pFormat
								pObj->outFrames,        		// pFrame
								VA_LINK_MAX_OUT_FRAMES);    	// numFrames

	UTILS_assert(status == FVID2_SOK);

	/* Delete the buffers */
	status = Utils_bufDelete(&pObj->outFrameQue);
	UTILS_assert(status == FVID2_SOK);
	status = Utils_bufDelete(&pObj->processFrameQue);
	UTILS_assert(status == FVID2_SOK);
#ifdef SYSTEM_DEBUG_VA
    Vps_printf(" %d: VA   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

static Int32 VaLink_processCopyFrames(VaLink_Obj *pObj)
{
	Int32 status = FVID2_SOK;
	UInt32 frameId = 0;
	UInt32 edmaWidth, edmaHeight;
	FVID2_Frame *pEmptyFrame,*pFullFrame;
    FVID2_FrameList frameList;
    System_LinkInQueParams *pInQueParams;

    /* Get the full frames from the previous link */
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);
	if (frameList.numFrames)
    {
		/* Apply SWOSD on the full frames */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
			/* Get the empty frame from out queue */
			//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
		#ifdef VA_NO_PROCESS
			status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
		#else
			status = Utils_bufGetEmptyFrame(&pObj->processFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
		#endif
			if ((status == FVID2_SOK) && (pEmptyFrame != NULL))
			{
				pFullFrame = frameList.frames[frameId];
				/* copy the frame */
				edmaWidth  = pObj->inQueInfo.chInfo[0].width;
				edmaHeight = pObj->inQueInfo.chInfo[0].height;
				/* copy YUV plane */
				DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId, 			// chId
									   VA_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
									   (UInt32)pFullFrame->addr[0][0], 				// srcAddr
									   (UInt32)pEmptyFrame->addr[0][0],				// dstAddr
									   edmaWidth,          							// edmaWidth
									   edmaHeight,         						// edmaHeight
									   edmaWidth,          							// srcLineOffset
									   edmaWidth);         							// dstLineOffset

				/* Trigger the edma transfer */
				DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId,
											(UInt32)pEmptyFrame->addr[0][0],
											edmaWidth,
											edmaHeight);

				DM81XX_EDMA3_setParams(pObj->copyEdmaHndl.dmaHndl.chId, 			// chId
									   VA_LINK_EDMA3_QUEUE_ID,  					// dmaQueue
									   (UInt32)pFullFrame->addr[0][1], 				// srcAddr
									   (UInt32)pEmptyFrame->addr[0][1],				// dstAddr
									   edmaWidth,          							// edmaWidth
									   edmaHeight/2,         						// edmaHeight
									   edmaWidth,          							// srcLineOffset
									   edmaWidth);         							// dstLineOffset
				/* Trigger the edma transfer */
				DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndl.dmaHndl.chId,
											(UInt32)pEmptyFrame->addr[0][1],
											edmaWidth,
											edmaHeight/2);

				pEmptyFrame->timeStamp = pFullFrame->timeStamp;
				//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
				/* Put the full buffer into full queue */
			#ifdef VA_NO_PROCESS
				status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
			#else
	            status = Utils_bufPutFullFrame(&pObj->processFrameQue, pEmptyFrame);
			#endif
				UTILS_assert(status == FVID2_SOK);
			}
        }
	#ifdef VA_NO_PROCESS
		System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);
	#endif
        /* Put the full buffers bacl to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }

	return status;
}


/* Va link task main function */

/* ===================================================================
 *  @func     VaLink_tskMain
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
Void VaLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the va driver */
    status = VaLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done   = FALSE;
    ackMsg = FALSE;

	Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_CREATE);

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done   = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                if(VaLink_processCopyFrames(pObj) == FVID2_SOK){
				#ifndef VA_NO_PROCESS
					Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_NEW_DATA);
				#endif
				}
                break;
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the va driver */
    VaLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

	Utils_tskSendCmd(&pObj->processTsk,SYSTEM_CMD_DELETE);

    return;
}

/* Va link task main function */

/* ===================================================================
 *  @func     VaLink_tskMain
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
Void VaLink_processTskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    Bool ackMsg,done;
    Int32 status = FVID2_SOK;
	VaLink_Obj *pObj = (VaLink_Obj *) pTsk->appData;

	if(cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg,FVID2_EFAIL);
        return;
    }

    /* Create the va alg instance */
    status = VaLink_algCreate(pObj);
    UTILS_assert(status == FVID2_SOK);

	Utils_tskAckOrFreeMsg(pMsg,status);

    done   = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk,&pMsg,BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
		{
            case SYSTEM_CMD_DELETE:
                done   = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
				VaLink_algProcessFrames(pObj);
                break;
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the va alg instance */
    status = VaLink_algDelete(pObj);
    UTILS_assert(status == FVID2_SOK);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg,status);
    }

    return;
}

/* Va Link init */

/* ===================================================================
 *  @func     VaLink_init
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
Int32 VaLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 vaId;

    VaLink_Obj *pObj;

    char tskName[32];

	UInt32 procId = System_getSelfProcId();

    for (vaId = 0; vaId < VA_LINK_OBJ_MAX; vaId++)
    {
        pObj = &gVaLink_obj[vaId];

        memset(pObj, 0, sizeof(*pObj));

		pObj->tskId = SYSTEM_MAKE_LINK_ID(procId,SYSTEM_LINK_ID_VA) + vaId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = VaLink_getFullFrames;
        linkObj.linkPutEmptyFrames = VaLink_putEmptyFrames;
        linkObj.getLinkInfo = VaLink_getInfo;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "VA%d", vaId);

        status = Utils_tskCreate(&pObj->tsk,
                                 VaLink_tskMain,
                                 VA_LINK_TSK_PRI,
                                 gVaLink_tskStack[vaId],
                                 VA_LINK_TSK_STACK_SIZE, pObj, tskName);
        UTILS_assert(status == FVID2_SOK);

		/* Create DMVAL process task */
		sprintf(tskName, "VAPROCESS%d", vaId);
        status = Utils_tskCreate(&pObj->processTsk,
                                 VaLink_processTskMain,
                                 VA_LINK_PROCESSTSK_PRI,
                                 gVaLink_processTskStack[vaId],
                                 VA_LINK_PROCESSTSK_STACK_SIZE,pObj,tskName);
        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* Va link de-init */

/* ===================================================================
 *  @func     VaLink_deInit
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
Int32 VaLink_deInit()
{
    UInt32 vaId;

    for (vaId = 0; vaId < VA_LINK_OBJ_MAX; vaId++)
    {
        Utils_tskDelete(&gVaLink_obj[vaId].tsk);
		Utils_tskDelete(&gVaLink_obj[vaId].processTsk);
    }

    return FVID2_SOK;
}
