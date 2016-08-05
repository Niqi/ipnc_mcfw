/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "algVehicleLink_priv.h"
#include <mcfw/src_bios6/utils/utils_mem.h>

#define UTILS_ALGVEHICLE_GET_OUTBUF_SIZE()   (sizeof(AlgVehicleLink_ThPlateIdResult))

TDSPLprResualt *gTDSPLprResualt=NULL;
SharedRegion_SRPtr gSRPtrTDSPLprResualt = NULL;


static Int32 AlgVehicleLink_createOutObj(AlgVehicleLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;
    AlgVehicleLink_OutObj *pOutObj;
    Int queueId;

    for(queueId = 0; queueId < ALGVEHICLE_LINK_MAX_OUT_QUE; queueId++)
    {
        pOutObj = &pObj->outObj[queueId];    
        pOutObj->numAllocPools = 1;
    }

    return status;
}


Int32 AlgVehicleLink_algCreate(AlgVehicleLink_Obj * pObj, AlgVehicleLink_CreateParams * pPrm)
{
    Int32 status;

    Vps_printf(" %d: ALGVEHICLE : algCreate in progress !!!\n", Utils_getCurTimeInMsec());

    UTILS_MEMLOG_USED_START();
    memcpy(&pObj->thPlateIdAlg.createArgs, &pObj->createArgs.thPlateIdCreateParams, sizeof(AlgVehicleLink_ThPlateIdCreateParams));

    System_getEdmaCh(&pObj->copyEdmaHndlYplane);	
    System_getEdmaCh(&pObj->copyEdmaHndlUVplane);

    pObj->thPlateIdAlg.inQueInfo = &pObj->inQueInfo;

    if (pObj->createArgs.enableThPlateIdAlg)
    {
        status = AlgVehicleLink_ThPlateIdalgCreate(&pObj->thPlateIdAlg);
        UTILS_assert(status == FVID2_SOK);
        AlgVehicleLink_createOutObj(pObj);
    }    


    UTILS_MEMLOG_USED_END(pObj->memUsed);
    UTILS_MEMLOG_PRINT("ALGVEHICLE",
                       pObj->memUsed,
                       UTILS_ARRAYSIZE(pObj->memUsed));
    Vps_printf(" %d: ALGVEHICLE: algCreate Done !!!\n", Utils_getCurTimeInMsec());
    return FVID2_SOK;
}


Int32 AlgVehicleLink_algDelete(AlgVehicleLink_Obj * pObj)
{    
    Int32 status;

    Vps_printf(" %d: ALGVEHICLE : algDelete in progress !!!\n", Utils_getCurTimeInMsec());
    
    if (pObj->createArgs.enableThPlateIdAlg)
    {
        status = AlgVehicleLink_ThPlateIdalgDelete(&pObj->thPlateIdAlg);
        UTILS_assert(status == FVID2_SOK);
    }
    
    Vps_printf(" %d: ALGVEHICLE : algDelete Done !!!\n", Utils_getCurTimeInMsec());

	return FVID2_SOK;
}

static Int32 Valink_OSDInitial(void)
{
    Ptr buf = NULL;
    Ptr srHeap = NULL;
	srHeap = SharedRegion_getHeap(0);
	UTILS_assert(srHeap != NULL);

	buf = (TDSPLprResualt *)Memory_calloc(srHeap, sizeof(gTDSPLprResualt), 128, NULL);
	gTDSPLprResualt = (TDSPLprResualt *)buf;
	gSRPtrTDSPLprResualt = SharedRegion_getSRPtr(buf, 0);

	gTDSPLprResualt->payload   = 0x00000001;
	gTDSPLprResualt->frameAddr = 0x00000000;
	memset((void *)(&gTDSPLprResualt->lprResult), 0x00, sizeof(gTDSPLprResualt->lprResult));

	return FVID2_SOK;
}

static Int32 Valink_CreatOSDFrame(Int32 frameAddr,const TH_PlateIDResult *result)
{
	static Int32 bCreatPtr = FALSE;
	Int32 notifystatus;

	if(bCreatPtr == FALSE){
		bCreatPtr = TRUE;
		Valink_OSDInitial();
	}
	gTDSPLprResualt->frameAddr = frameAddr;
	strcpy(gTDSPLprResualt->lprResult.license, result->license);
	strcpy(gTDSPLprResualt->lprResult.color, result->color);	
	gTDSPLprResualt->lprResult.nColor = result->nColor;
	gTDSPLprResualt->lprResult.nType = result->nType;
	gTDSPLprResualt->lprResult.nRectLeftX = result->rcLocation.left;
	gTDSPLprResualt->lprResult.nRectLeftY = result->rcLocation.top;	
	gTDSPLprResualt->lprResult.nRectWidth = result->rcLocation.right- result->rcLocation.left;
	gTDSPLprResualt->lprResult.nRectHeight = result->rcLocation.bottom - result->rcLocation.top;	
	notifystatus = Notify_sendEvent(System_getSyslinkProcId(SYSTEM_PROC_HOSTA8),
			SYSTEM_IPC_NOTIFY_LINE_ID,
			SYSTEM_IPC_NOTIFY_EVENT_ID_APP,
			(Int32)gSRPtrTDSPLprResualt,
			TRUE);
	if(notifystatus != Notify_S_SUCCESS){
		return FVID2_EFAIL;
	}

	return FVID2_SOK;
}

Int32 AlgVehicleLink_algProcessData(AlgVehicleLink_Obj * pObj)
{
    UInt32 status = FVID2_SOK;
	Int32 procesFrames = 0;	
    AlgVehicleLink_ThPlateIdObj * pAlgObj;
    FVID2_Frame *pFullFrame;
	System_FrameInfo *pInFrameInfo = NULL;
    UInt64 start,end;

    pAlgObj = &pObj->thPlateIdAlg;

    do
    {
        /* Get the frame from the full queue */
        status = Utils_bufGetFullFrame(&pObj->processFrameQue, &pFullFrame, BIOS_NO_WAIT);

        if ((status == FVID2_SOK) && (pFullFrame != NULL))
        {
            /* THPLATEID ALG */
            if (pObj->createArgs.enableThPlateIdAlg)
            {
	            /* statist process time start */
	            start = Utils_getCurTimeInUsec();  

                status = AlgVehicleLink_ThPlateIdalgProcessData(pAlgObj, pFullFrame, &pObj->outObj[0].bufOutQue);
				
	            /* statist process time end */
	            end = Utils_getCurTimeInUsec();				

				if( FVID2_SOK == status)
				{
					//Vps_printf("\n THPLATEIDALG:frameAddr:0x%08x \n", (Int32)pFullFrame->addr[0][0]);
					if(pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0].nConfidence > 50)
					{
						pAlgObj->chObj[0].inFrameProcessCount++;
						if(1 == pAlgObj->chObj[0].inFrameProcessCount)//只输出第一个识别到的结果
						{
							pAlgObj->chObj[0].totalFrameCount = 0;
							Valink_CreatOSDFrame((Int32)pFullFrame->addr[0][0], 
													&(pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0]));

							pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;
							if(NULL != pInFrameInfo)
							{
								strcpy((char *)pInFrameInfo->license, (char *)pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0].license);								
							}
							
					        Vps_printf("\n THPLATEIDALG: Process frame, addr:%d, sn:%s, cl:%d, tp:%d, tm:%ld \n",
											pFullFrame->timeStamp,
					                        pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0].license,
					                        pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0].nColor,
					                        pAlgObj->chObj[0].thPlateIdResult.thPlateIdResultAll[0].nConfidence,
					                        (end - start)/1000);						
						}		
					}					
				}
				else
				{
					Vps_printf("\n THPLATEIDALG: Process fail! \n");
				}
            }
        
            if((end - start) > pAlgObj->chObj[0].maxProcessTime)
            {
                pAlgObj->chObj[0].maxProcessTime = (end - start);
            }
            pAlgObj->chObj[0].totalProcessTime += (end - start);
            pAlgObj->chObj[0].processFrCnt ++;
			pAlgObj->chObj[0].totalFrameCount ++;

			if(pAlgObj->chObj[0].totalFrameCount > 30)//30帧以内其它结果扔掉
			{
				pAlgObj->chObj[0].inFrameProcessCount = 0;
			}

            if(pAlgObj->chObj[0].processFrCnt >= 200)
            {
                Vps_printf(" %d: ALGVEHICLE: alg Avg Prc tm = %ld us/f, max:%ld !!!\n",
            			   Clock_getTicks(), pAlgObj->chObj[0].totalProcessTime/pAlgObj->chObj[0].processFrCnt,
            			   pAlgObj->chObj[0].maxProcessTime);	   
                pAlgObj->chObj[0].processFrCnt     = 0;
                pAlgObj->chObj[0].totalProcessTime = 0;
            }

			if((1 == pAlgObj->chObj[0].inFrameProcessCount)&&(1 == pAlgObj->chObj[0].totalFrameCount))
			{
	        	status = Utils_bufPutFullFrame(&pObj->outFrameQue, pFullFrame);
	        	UTILS_assert(status == FVID2_SOK);
				System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink, SYSTEM_CMD_NEW_DATA);
			}
			else
			{
	        	status = Utils_bufPutEmptyFrame(&pObj->processFrameQue, pFullFrame);
	        	UTILS_assert(status == FVID2_SOK);				
			}
			
			procesFrames++;
        }
    } while ((status == FVID2_SOK) && (pFullFrame != NULL));

	if(procesFrames){
	    /* send SYSTEM_CMD_NEW_DATA to next link */
		//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
	    //System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink, SYSTEM_CMD_NEW_DATA);
	}

    return FVID2_SOK;
}


/* AlgVehicle link copy frames */

/* ===================================================================
 *  @func     AlgVehicleLink_algCopyFrames
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
static UInt64 start = 0;
static UInt64 end = 0;
Int32 AlgVehicleLink_algCopyFrames(AlgVehicleLink_Obj *pObj)
{
    Int32 status = FVID2_SOK;
    Int32 frameId;
    UInt32 edmaWidth, edmaHeight, edmaSrcOffset;
    FVID2_Frame *pEmptyFrame;
    FVID2_Frame *pFullFrame;
    FVID2_FrameList frameList;
    System_LinkInQueParams *pInQueParams;
    AlgVehicleLink_ThPlateIdObj *pAlgObj = &pObj->thPlateIdAlg;

    /* Get the full frames from the previous link */
    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);
	//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);

    if (frameList.numFrames)
    {
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
        	//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
            if(Utils_doSkipFrame(&pAlgObj->chObj[0].frameSkipCtx) == FALSE)
            {
            	//Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
                /* Get the AlgVehicle empty bufs if any */
#ifdef ALG_NO_PROCESS
				status = Utils_bufGetEmptyFrame(&pObj->outFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
#else
                status = Utils_bufGetEmptyFrame(&pObj->processFrameQue, &pEmptyFrame, BIOS_NO_WAIT);
#endif
                if ((status == FVID2_SOK) && (pEmptyFrame != NULL))
                {
                	end = start;
                	start = Utils_getCurTimeInUsec();
					if(0)
					{
						Vps_printf("Utils_bufGetEmptyFrame time %ld \n", (start - end)/1000);
					}
                    pFullFrame = frameList.frames[frameId];

                    /* copy the Yframe */
                    edmaWidth  = pObj->inQueInfo.chInfo[0].width;
                    edmaHeight = pObj->inQueInfo.chInfo[0].height;
                    edmaSrcOffset = pObj->inQueInfo.chInfo[0].pitch[0];

                    /* copy Y plane */
                    DM81XX_EDMA3_setParams(pObj->copyEdmaHndlYplane.dmaHndl.chId, 		// chId
                    					   ALGVEHICLE_LINK_EDMA3_QUEUE_ID_0,  			// dmaQueue
                    					   (UInt32)pFullFrame->addr[0][0], 				// srcAddr
                    					   (UInt32)pEmptyFrame->addr[0][0],				// dstAddr
                    					   edmaWidth,          							// edmaWidth
                    					   edmaHeight,					                    // edmaHeight
                    					   edmaSrcOffset,          							// srcLineOffset
                    					   edmaWidth);         							// dstLineOffset
                    /* Trigger the edma transfer */
                    DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndlYplane.dmaHndl.chId,
                    								(UInt32)pEmptyFrame->addr[0][0],
                    								edmaWidth,
													edmaHeight);

					/* copy UV plane */
                    DM81XX_EDMA3_setParams(pObj->copyEdmaHndlUVplane.dmaHndl.chId, 		// chId
                    					   ALGVEHICLE_LINK_EDMA3_QUEUE_ID_0,  			// dmaQueue
                    					   (UInt32)pFullFrame->addr[0][1], 				// srcAddr
                    					   (UInt32)pEmptyFrame->addr[0][1],				// dstAddr
                    					   edmaWidth,          							// edmaWidth
                    					   edmaHeight/2,         							// edmaHeight
                    					   edmaSrcOffset,          							// srcLineOffset
                    					   edmaWidth);         							// dstLineOffset
                                 

                    /* Trigger the edma transfer */
                    DM81XX_EDMA3_triggerTransfer(pObj->copyEdmaHndlUVplane.dmaHndl.chId,
                    								(UInt32)pEmptyFrame->addr[0][0],
                    								edmaWidth,
													edmaHeight/2);                 

                    pEmptyFrame->timeStamp = pFullFrame->timeStamp;

					pEmptyFrame->channelNum = pFullFrame->channelNum;

                    /* put the buffer into full queue */
#ifdef ALG_NO_PROCESS
					status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
#else					
                    status = Utils_bufPutFullFrame(&pObj->processFrameQue, pEmptyFrame);
#endif
                    UTILS_assert(status == FVID2_SOK);
                }
            }
        }

#ifdef ALG_NO_PROCESS
		System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);
#endif		

        /* Put the full buffers bacl to previous link */
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId, &frameList);
    }
	
    return status;
}


