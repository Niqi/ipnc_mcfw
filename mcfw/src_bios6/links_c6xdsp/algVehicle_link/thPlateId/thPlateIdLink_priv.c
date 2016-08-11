/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "thPlateIdLink_priv.h"
#include "ti/sdo/fc/dskt2/dskt2.h"
#include "ti/sdo/fc/rman/rman.h"
#include <mcfw/interfaces/common_def/ti_vsys_common_def.h>
#include <mcfw/src_bios6/utils/utils_mem.h>

static UInt8 gThPlateIdScratchId = 1;

static Int32 AlgVehicleLink_ThPlateIdalgSetChPrm(AlgVehicleLink_ThPlateIdchPrm    *pThPlateIdChPrm,
                            AlgVehicleLink_ThPlateIdChParams * params);

static Int32 AlgVehicleLink_ThPlateIdalgSetDynPrm(AlgVehicleLink_ThPlateIdDynPrm    *pThPlateIdChPrm,
                            AlgVehicleLink_ThPlateIdChParams * params);


Int32 AlgVehicleLink_ThPlateIdalgCreate(AlgVehicleLink_ThPlateIdObj * pObj)
{
    Int32               status, chId, ThPlateIdChId;
    THPLATEIDALG_createPrm       algCreatePrm;
    THPLATEIDALG_chPrm            chDefaultParams[1];
    AlgVehicleLink_ThPlateIdChParams *pChLinkPrm;
    AlgVehicleLink_ThPlateIdchPrm    *pThPlateIdChPrm;
    AlgVehicleLink_ThPlateIdDynPrm    *pThPlateIdDynPrm;	
    Int32 scratchId = gThPlateIdScratchId;    
    IALG_Fxns           *algFxns = (IALG_Fxns *)&THPLATEIDALG_TI_IALG;
    IRES_Fxns           *resFxns = &THPLATEIDALG_TI_IRES;

#ifdef SYSTEM_DEBUG_THPLATEID
    Vps_printf(" %d: THPLATEIDLINK    : Create in progress !!!\n",
                        Utils_getCurTimeInMsec());
#endif
	pObj->setConfigBitMask = 0;
	pObj->getConfigFlag = 0;

    pObj->chObj[0].inFrameProcessCount = 0;
    pObj->chObj[0].totalFrameCount = 0;
    pObj->chObj[0].maxProcessTime = 0;
    pObj->chObj[0].totalProcessTime = 0;
    pObj->chObj[0].processFrCnt= 0;

    if(pObj->createArgs.numBufPerCh == 0)
    {
        pObj->createArgs.numBufPerCh = ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH;
    }

    if(pObj->createArgs.numBufPerCh > ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH)
    {
        Vps_printf("\n THPLATEIDLINK: WARNING: User is asking for %d buffers per CH. But max allowed is %d. \n"
            " Over riding user requested with max allowed \n\n",
            pObj->createArgs.numBufPerCh, ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH);

        pObj->createArgs.numBufPerCh = ALGVEHICLE_LINK_THPLATEID_MAX_OUT_FRAMES_PER_CH;

    }

#ifdef SYSTEM_DEBUG_THPLATEID
    Vps_printf(" %d: THPLATEIDLINK    : Max WxH = %d x %d, Max CH = %d, FPS = %d !!!\n",
           Utils_getCurTimeInMsec(),
            pObj->createArgs.maxWidth,
            pObj->createArgs.maxHeight,
            pObj->createArgs.numCh,
            pObj->createArgs.outputFrameRate
        );
#endif

    if((pObj->createArgs.numValidChForTHPLATEID) > pObj->inQueInfo->numCh)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : Create ERROR - THPLATEID channels < InQueue Channels !!!\n",
               Utils_getCurTimeInMsec());
#endif
        return FVID2_EFAIL;
    }

    algCreatePrm.maxWidth    = pObj->createArgs.maxWidth;
    algCreatePrm.maxHeight   = pObj->createArgs.maxHeight;
    algCreatePrm.maxStride   = pObj->createArgs.maxStride;
    algCreatePrm.maxChannels = pObj->createArgs.numValidChForTHPLATEID;
    algCreatePrm.numSecs2WaitB4Init		= pObj->createArgs.numSecs2WaitB4Init;
    algCreatePrm.chDefaultParams        = (THPLATEIDALG_chPrm *)&chDefaultParams[0];

    for(chId=0; chId < algCreatePrm.maxChannels; chId++)
    {
        THPLATEIDALG_chPrm			* chl	= &(algCreatePrm.chDefaultParams[chId]);
        AlgVehicleLink_ThPlateIdChParams	* chPrm = &(pObj->createArgs.chDefaultParams[chId]);

        // The remaining parameter values filled in here do not really matter as
        // they will be over-written by calls to THPLATEID_TI_setPrms. We'll fill in 
        // just a few
        chl->chId	= chPrm->chId;
        chl->width	= pObj->createArgs.maxWidth;
        chl->height     = pObj->createArgs.maxHeight;
        chl->stride     = pObj->createArgs.maxStride;
        chl->curFrame = NULL;
        chl->cImageFormat       = chPrm->cImageFormat;
        chl->bVertFlip              = chPrm->bVertFlip;
        chl->bDwordAligned      = chPrm->bDwordAligned;
        chl->bEnlarge               = chPrm->bEnlarge;
        chl->nPlateLocate_Th     = chPrm->nPlateLocate_Th;
        chl->nOCR_Th                = chPrm->nOCR_Th;
        chl->nContrast                  = chPrm->nContrast;
        chl->bLeanCorrection        = chPrm->bLeanCorrection;
        chl->bShadow                = chPrm->bShadow;   
        memcpy(&chl->szProvince[0], &chPrm->szProvince[0], 16);
        chl->dFormat = chPrm->dFormat;
        chl->pnMinFreeSRAM = chPrm->pnMinFreeSRAM;
        chl->pnMinFreeSDRAM = chPrm->pnMinFreeSDRAM; 
		chl->nMaxPlateWidth = chPrm->plateWidth.nMax;
		chl->nMinPlateWidth = chPrm->plateWidth.nMin;		
        //memcpy(&chl->algConfig, &chPrm->algConfig, sizeof(TH_PlateIDCfg));
        chl->rcDetect.top =         chPrm->rcDetect.top;
        chl->rcDetect.bottom =    chPrm->rcDetect.bottom;
        chl->rcDetect.left =         chPrm->rcDetect.left;
        chl->rcDetect.right =       chPrm->rcDetect.right;
    }
    
    /* Create algorithm instance and get algo handle  */
    pObj->algHndl = DSKT2_createAlg((Int)gThPlateIdScratchId,
            (IALG_Fxns *)algFxns, NULL,(IALG_Params *)&algCreatePrm);

    if(pObj->algHndl == NULL)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : DSKT2_createAlg ERROR !!!\n",
               Utils_getCurTimeInMsec());
#endif

        return FVID2_EFAIL;
    }

    /* Assign resources to the algorithm */
	IRES_Status iresStatus;
    iresStatus = RMAN_assignResources((IALG_Handle)pObj->algHndl, resFxns, scratchId);
    if (iresStatus != IRES_OK) {
        return FVID2_EFAIL;
    }

    for(ThPlateIdChId = 0;  ThPlateIdChId<pObj->createArgs.numValidChForTHPLATEID; ThPlateIdChId++)
    {
        pObj->chObj[ThPlateIdChId].frameSkipCtx.firstTime = TRUE;
        pObj->chObj[ThPlateIdChId].frameSkipCtx.inputFrameRate  = pObj->createArgs.inputFrameRate;
        pObj->chObj[ThPlateIdChId].frameSkipCtx.outputFrameRate = pObj->createArgs.outputFrameRate;

        pThPlateIdChPrm = &pObj->chParams[ThPlateIdChId];
        pChLinkPrm = &pObj->createArgs.chDefaultParams[ThPlateIdChId];
		pThPlateIdDynPrm = &pObj->thPlateIdDynParams[ThPlateIdChId];
        pThPlateIdChPrm->chId = pChLinkPrm->chId; 
		
        AlgVehicleLink_ThPlateIdalgSetChPrm(pThPlateIdChPrm, pChLinkPrm);
		AlgVehicleLink_ThPlateIdalgSetDynPrm(pThPlateIdDynPrm, pChLinkPrm);
		
        status = AlgVehicleLink_ThPlateIdInit(pObj);
        if(FVID2_SOK != status)
        {
            return status;
        } 
        
        AlgVehicleLink_ThPlateIdAlgSetImageFormat(pObj);
        AlgVehicleLink_ThPlateIdAlgSetEnbalePlateFormat(pObj);
        AlgVehicleLink_ThPlateIdAlgSetRecogThreshold(pObj);
        AlgVehicleLink_ThPlateIdAlgSetProvinceOrder(pObj);
        AlgVehicleLink_ThPlateIdAlgSetEnableLeanCorrection(pObj);
        AlgVehicleLink_ThPlateIdAlgSetEnableShadow(pObj);
        AlgVehicleLink_ThPlateIdAlgSetRecogArea(pObj);

        pThPlateIdChPrm->width  = pObj->inQueInfo->chInfo[pThPlateIdChPrm->chId].width +
            pObj->inQueInfo->chInfo[pThPlateIdChPrm->chId].startX;
        pThPlateIdChPrm->height = pObj->inQueInfo->chInfo[pThPlateIdChPrm->chId].height +
            pObj->inQueInfo->chInfo[pThPlateIdChPrm->chId].startY;
        pThPlateIdChPrm->stride = pObj->inQueInfo->chInfo[pThPlateIdChPrm->chId].pitch[0];

#ifdef SYSTEM_DEBUG_ALGVEHICLE
        Vps_printf(" %d: THPLATEIDLINK    : %d: %d x %d, In FPS = %d, Out FPS = %d!!!\n",
                   Utils_getCurTimeInMsec(),
                    pThPlateIdChPrm->chId,
                    pThPlateIdChPrm->width,
                    pThPlateIdChPrm->height,
                    pObj->chObj[ThPlateIdChId].frameSkipCtx.inputFrameRate,
                    pObj->chObj[ThPlateIdChId].frameSkipCtx.outputFrameRate
            );
#endif

    }

#ifdef SYSTEM_DEBUG_ALGVEHICLE
    Vps_printf(" %d: THPLATEIDLINK    : Create Done !!!\n",
               Utils_getCurTimeInMsec());
#endif

 return FVID2_SOK;
}

Int32 AlgVehicleLink_ThPlateIdalgDelete(AlgVehicleLink_ThPlateIdObj * pObj)
{
    Int32 scratchId = gThPlateIdScratchId;
    IRES_Status status;
    IRES_Fxns * resFxns = &THPLATEIDALG_TI_IRES;

#ifdef SYSTEM_DEBUG_ALGVEHICLE
    Vps_printf(" %d: THPLATEIDLINK    : Delete in progress !!!\n",
               Utils_getCurTimeInMsec());
#endif

    if(pObj->algHndl == NULL)
        return FVID2_EFAIL;

   /*
    * Deactivate All Resources
    */
    //RMAN_deactivateAllResources((IALG_Handle)pObj->algHndl, resFxns, scratchId);
   
    /* Deactivate algorithm */
    DSKT2_deactivateAlg(scratchId, (IALG_Handle)pObj->algHndl);

    /*
    * Free resources assigned to this algorihtm
    */
    status = RMAN_freeResources((IALG_Handle)pObj->algHndl, resFxns, scratchId);

    if (IRES_OK != status) {
        return FVID2_EFAIL;
    }  

    DSKT2_freeAlg(scratchId, (IALG_Handle)pObj->algHndl);

#ifdef SYSTEM_DEBUG_THPLATEID
    Vps_printf(" %d: THPLATEIDLINK    : Delete Done !!!\n",
               Utils_getCurTimeInMsec());
#endif

    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdalgProcessData(AlgVehicleLink_ThPlateIdObj * pObj, FVID2_Frame *pFrame, Utils_BitBufHndl *bufOutQue)
{
    Int32 chIdx = 0;
	THPLATEIDALG_Status 	 chanStatus; 
	THPLATEIDALG_Result 	 *pThPlateIdResult;
	UInt32			chanID;
	//AlgVehicleLink_ThPlateIdchPrm * chPrm;
	IRES_Fxns		*resFxns = &THPLATEIDALG_TI_IRES;
	pThPlateIdResult = &pObj->chObj[chIdx].thPlateIdResult;	

    //pObj->chParams[chIdx].curFrame = pFrame->addr[0][0];
    //pObj->chParams[chIdx].curFrameUV = pFrame->addr[0][1];

	
	/* Activate the Algorithm */
	DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pObj->algHndl);
	
	/*
	* Activate All Resources
	*/
	RMAN_activateAllResources((IALG_Handle)pObj->algHndl, resFxns, gThPlateIdScratchId);	

	chanID = pObj->chParams[chIdx].chId;
	//chPrm = &(pObj->chParams[chIdx]);

	pThPlateIdResult->nNumberOfVehicle= 0;
		
	chanStatus = THPLATEIDALG_TI_process(pObj->algHndl, chanID,
											(unsigned char*)pFrame->addr[0][0],
											(unsigned char*)pFrame->addr[0][1], pThPlateIdResult);

	/*
	* Deactivate All Resources
	*/
	RMAN_deactivateAllResources((IALG_Handle)pObj->algHndl, resFxns, gThPlateIdScratchId);

	/* Deactivate algorithm */
	DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pObj->algHndl);

	if(chanStatus != THPLATEID_NO_ERROR)
	{
#ifdef SYSTEM_DEBUG_THPLATEID
		Vps_printf(" %d: THPLATEIDLINK	  : ERROR: Alg Process (chanID = %d) !!!\n",
				  Utils_getCurTimeInMsec(), chanID );
#endif

		return FVID2_EFAIL;
	}

	return FVID2_SOK;

}

Int32 AlgVehicleLink_ThPlateIdalgSetConfig(AlgVehicleLink_ThPlateIdObj * algObj)
{
    Int32 status = FVID2_SOK;
    UInt32 bitMask;
    Bool setConfigFlag = FALSE;
    UInt key;

    key = Hwi_disable();
    bitMask = algObj->setConfigBitMask;

	Vps_printf("mask_b: %d \n", algObj->setConfigBitMask);

    /* Set the modified trigger info */
    if ((bitMask >> ALGVEHICLE_LINK_SETCONFIG_BITMASK_TRIGG_INFO) & 0x1)
    {
    	algObj->thPlateIdDynParams[0].trigArea.left = algObj->chParams[0].rcTrig.left;
    	algObj->thPlateIdDynParams[0].trigArea.right= algObj->chParams[0].rcTrig.right;
    	algObj->thPlateIdDynParams[0].trigArea.top= algObj->chParams[0].rcTrig.top;
    	algObj->thPlateIdDynParams[0].trigArea.bottom= algObj->chParams[0].rcTrig.bottom;
		algObj->thPlateIdDynParams[0].nTrigInterval = algObj->chParams[0].nTrigInterval;
		algObj->thPlateIdDynParams[0].nTrigMode = algObj->chParams[0].nTrigMode;
		algObj->thPlateIdDynParams[0].nVehicleDirection = algObj->chParams[0].nVehicleDirection;		
    	algObj->setConfigBitMask &= (ALGVEHICLE_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ALGVEHICLE_LINK_SETCONFIG_BITMASK_TRIGG_INFO));	
        setConfigFlag = TRUE;
		Vps_printf("mask_trig: %d,%d:%d:%d:%d \n", algObj->setConfigBitMask, algObj->chParams[0].rcTrig.left,
																algObj->chParams[0].rcTrig.right,
																algObj->chParams[0].rcTrig.top,
																algObj->chParams[0].rcTrig.bottom);
    }	

    /* Set the modified recog area */
    if ((bitMask >> ALGVEHICLE_LINK_SETCONFIG_BITMASK_RECOG_AREA) & 0x1)
    {
    	AlgVehicleLink_ThPlateIdAlgSetRecogArea(algObj);
        algObj->setConfigBitMask &= (ALGVEHICLE_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ALGVEHICLE_LINK_SETCONFIG_BITMASK_RECOG_AREA));	
        setConfigFlag = TRUE;
		Vps_printf("mask_recog: %d \n", algObj->setConfigBitMask);
    }	

    /* Set the modified default province */
    if ((bitMask >> ALGVEHICLE_LINK_SETCONFIG_BITMASK_DEFAULT_PROVINCE) & 0x1)
    {
    	AlgVehicleLink_ThPlateIdAlgSetProvinceOrder(algObj);
        algObj->setConfigBitMask &= (ALGVEHICLE_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ALGVEHICLE_LINK_SETCONFIG_BITMASK_DEFAULT_PROVINCE));	
        setConfigFlag = TRUE;
		Vps_printf("mask_prov: %d \n", algObj->setConfigBitMask);
    }

    /* Set the modified max min plate width */
    if ((bitMask >> ALGVEHICLE_LINK_SETCONFIG_BITMASK_PLATE_WIDTH) & 0x1)
    {
    	AlgVehicleLink_ThPlateIdAlgSetPlateWidth(algObj);
        algObj->setConfigBitMask &= (ALGVEHICLE_LINK_SETCONFIG_BITMASK_RESET_VALUE ^
                                     (1 << ALGVEHICLE_LINK_SETCONFIG_BITMASK_PLATE_WIDTH));	
        setConfigFlag = TRUE;
		Vps_printf("mask_plateWidth: %d \n", algObj->setConfigBitMask);
    }	

    Hwi_restore(key);

    if (setConfigFlag)
    {
    }

	return (status);
}

static Int32 AlgVehicleLink_ThPlateIdalgSetChPrm(AlgVehicleLink_ThPlateIdchPrm    *pThPlateIdChPrm,
                            AlgVehicleLink_ThPlateIdChParams * params)
{
    pThPlateIdChPrm->chId = params->chId;

    pThPlateIdChPrm->cImageFormat = params->cImageFormat;
    pThPlateIdChPrm->bVertFlip = params->bVertFlip;
    pThPlateIdChPrm->bDwordAligned = params->bDwordAligned;
    pThPlateIdChPrm->bEnlarge = params->bEnlarge;
    pThPlateIdChPrm->nPlateLocate_Th = params->nPlateLocate_Th;
    pThPlateIdChPrm->nOCR_Th = params->nOCR_Th;
    pThPlateIdChPrm->nContrast = params->nContrast;
    pThPlateIdChPrm->bLeanCorrection = params->bLeanCorrection;
    pThPlateIdChPrm->bShadow= params->bShadow;    
    
    memcpy(pThPlateIdChPrm->szProvince, params->szProvince, 16);

    pThPlateIdChPrm->dFormat = params->dFormat;
    pThPlateIdChPrm->pnMinFreeSRAM = params->pnMinFreeSRAM;
    pThPlateIdChPrm->pnMinFreeSDRAM = params->pnMinFreeSDRAM; 

	pThPlateIdChPrm->plateWidth.nMax = params->plateWidth.nMax;
	pThPlateIdChPrm->plateWidth.nMin = params->plateWidth.nMin;	
    
    //memcpy(&pThPlateIdChPrm->algConfig, &params->algConfig, sizeof(TH_PlateIDCfg));

    pThPlateIdChPrm->rcDetect.top =         params->rcDetect.top;
    pThPlateIdChPrm->rcDetect.bottom =    params->rcDetect.bottom;
    pThPlateIdChPrm->rcDetect.left =         params->rcDetect.left;
    pThPlateIdChPrm->rcDetect.right =       params->rcDetect.right;  

    pThPlateIdChPrm->rcTrig.top =         params->rcTrig.top;
    pThPlateIdChPrm->rcTrig.bottom =    params->rcTrig.bottom;
    pThPlateIdChPrm->rcTrig.left =         params->rcTrig.left;
    pThPlateIdChPrm->rcTrig.right =       params->rcTrig.right;	

	//pThPlateIdChPrm->nMaxPlateWidth = params->n;

    Vps_printf( "THPLATEIDLINK    :SetChPrm\n");
 
    return FVID2_SOK;
}

static Int32 AlgVehicleLink_ThPlateIdalgSetDynPrm(AlgVehicleLink_ThPlateIdDynPrm    *pThPlateIdDynPrm,
                            AlgVehicleLink_ThPlateIdChParams * params)
{
    pThPlateIdDynPrm->chId = params->chId;
    memcpy(pThPlateIdDynPrm->szProvince, params->szProvince, 16);
    pThPlateIdDynPrm->nPlateTypeSupt = 0;
    pThPlateIdDynPrm->nTrigMode = 0;
    pThPlateIdDynPrm->nTrigInterval = 0;
    pThPlateIdDynPrm->nVehicleDirection = 0;
    pThPlateIdDynPrm->plateWidth.nMax = params->plateWidth.nMax;
    pThPlateIdDynPrm->plateWidth.nMin = params->plateWidth.nMin;	
    pThPlateIdDynPrm->recogArea.top = params->rcDetect.top;
    pThPlateIdDynPrm->recogArea.bottom = params->rcDetect.bottom;
    pThPlateIdDynPrm->recogArea.left = params->rcDetect.left;
    pThPlateIdDynPrm->recogArea.right = params->rcDetect.right;  
    pThPlateIdDynPrm->trigArea.top = params->rcTrig.top;
    pThPlateIdDynPrm->trigArea.bottom = params->rcTrig.bottom;
    pThPlateIdDynPrm->trigArea.left = params->rcTrig.left;
    pThPlateIdDynPrm->trigArea.right = params->rcTrig.right;

    Vps_printf( "THPLATEIDLINK    :SetDynPrm\n");
 
    return FVID2_SOK;
}



Int32 AlgVehicleLink_ThPlateIdInit(AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;   
    THPLATEIDALG_Status      chanStatus;

    chanID = pThPlateIdAlgLinkObj->chParams[0].chId;

    //chanParam.setMode = THPLATEID_SET_INIT;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID,
											THPLATEID_SET_INIT, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: Init (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdDeInit (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;    
    THPLATEIDALG_Status      chanStatus;

    chanID = pThPlateIdAlgLinkObj->chParams[0].chId;

    //chanParam.setMode = THPLATEID_SET_DEINIT;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_DEINIT, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: DeInit (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");

    return FVID2_SOK;    
}


Int32 AlgVehicleLink_ThPlateIdAlgSetImageFormat (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_IMAGE_FORMAT;   
    chanParam.cImageFormat          = chPrm->cImageFormat;
    chanParam.bVertFlip                 = chPrm->bVertFlip;
    chanParam.bDwordAligned         = chPrm->bDwordAligned;    

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID,
											THPLATEID_SET_IMAGE_FORMAT, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetImageFormat (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetEnbalePlateFormat (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_ENABLED_PLATE_FORMAT;   
    chanParam.dFormat          = chPrm->dFormat;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID,
											THPLATEID_SET_ENABLED_PLATE_FORMAT, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetEnbalePlateFormat (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetRecogThreshold (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_RECOGTHRESHOLD;   
    chanParam.nPlateLocate_Th          = chPrm->nPlateLocate_Th;
    chanParam.nOCR_Th                   = chPrm->nOCR_Th;    

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID,
											THPLATEID_SET_RECOGTHRESHOLD, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SET_RECOGTHRESHOLD (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetProvinceOrder (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_PROVINCE_ORDER;   
    strcpy(&chanParam.szProvince[0], &chPrm->szProvince[0]);

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_PROVINCE_ORDER, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetProvinceOrder (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetEnlargeMode (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_ENLARGE_MODE;   
    chanParam.bEnlarge          = chPrm->bEnlarge;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_ENLARGE_MODE, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetEnlargeMode (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}

Int32 AlgVehicleLink_ThPlateIdAlgSetContrast (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_CONTRAST;   
    chanParam.nContrast          = chPrm->nContrast;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_CONTRAST, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetContrast (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetEnableLeanCorrection (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_ENABLE_LEAN_CORRECTION;   
    chanParam.bLeanCorrection          = chPrm->bLeanCorrection;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_ENABLE_LEAN_CORRECTION, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetEnableLeanCorrection (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetEnableShadow (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_ENABLE_SHADOW;   
    chanParam.bShadow          = chPrm->bShadow;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_ENABLE_SHADOW, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetEnableShadow (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetRecogArea (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;

    //chanParam.setMode = THPLATEID_SET_RECOGREGION;   
    chanParam.rcDetect.bottom = chPrm->rcDetect.bottom;
    chanParam.rcDetect.top      = chPrm->rcDetect.top;
    chanParam.rcDetect.left      = chPrm->rcDetect.left;
    chanParam.rcDetect.right    = chPrm->rcDetect.right;

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_RECOGREGION, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetRecogRegion (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


Int32 AlgVehicleLink_ThPlateIdAlgSetPlateWidth (AlgVehicleLink_ThPlateIdObj *pThPlateIdAlgLinkObj)
{
    UInt32 chanID;
    THPLATEIDALG_chPrm       chanParam;
    AlgVehicleLink_ThPlateIdchPrm * chPrm;    
    THPLATEIDALG_Status      chanStatus;

    chPrm = &(pThPlateIdAlgLinkObj->chParams[0]); 
    chanID = chPrm->chId;
	
    chanParam.nMaxPlateWidth = chPrm->plateWidth.nMax;
    chanParam.nMinPlateWidth = chPrm->plateWidth.nMin;	

    /* Activate the Algorithm */
    DSKT2_activateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);    
    
    chanStatus = THPLATEIDALG_TI_setPrms(pThPlateIdAlgLinkObj->algHndl, chanID, 
											THPLATEID_SET_PLATE_WIDTH, &chanParam); 

    /* Deactivate algorithm */
    DSKT2_deactivateAlg(gThPlateIdScratchId, (IALG_Handle)pThPlateIdAlgLinkObj->algHndl);

    if(chanStatus != THPLATEID_NO_ERROR)
    {
#ifdef SYSTEM_DEBUG_THPLATEID
        Vps_printf(" %d: THPLATEIDLINK    : ERROR: SetProvinceOrder (chanID = %d) !!!\n",
                  Utils_getCurTimeInMsec(), chanID );
#endif

        return FVID2_EFAIL;
    }    
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}



Int32 AlgVehicleLink_ThPlateIdprintStatistics (AlgVehicleLink_ThPlateIdObj *pObj, Bool resetAfterPrint)
{
    UInt32 chId;
    AlgVehicleLink_ThPlateIdChObj *pChObj;
    UInt32 elaspedTime;

    elaspedTime = Utils_getCurTimeInMsec() - pObj->chObj[0].statsStartTime; // in msecs
    elaspedTime /= 1000; // convert to secs

    Vps_printf( " \n"
            " *** THPLATEIDLINK    : Statistics *** \n"
            " \n"
            " Elasped Time           : %d secs\n"
            " Total Fields Processed : %d \n"
            " Total Fields FPS       : %d FPS\n"
            " \n"
            " \n"
            " CH  | In Recv In Process In Skip In Process Time \n"
            " Num | FPS     FPS        FPS     per frame (msec)\n"
            " -------------------------------------------------\n",
            elaspedTime,
                    pObj->chObj[0].totalFrameCount,
            pObj->chObj[0].totalFrameCount / (elaspedTime)
                    );

    for (chId = 0; chId < pObj->createArgs.numValidChForTHPLATEID; chId++)
    {
        pChObj = &pObj->chObj[chId];
   
        Vps_printf( "THPLATEIDLINK    : %3d | %7d %10d %7d %14d\n",
            chId,
            pChObj->inFrameRecvCount/elaspedTime,
            pChObj->inFrameProcessCount/elaspedTime,
            pChObj->inFrameUserSkipCount/elaspedTime,
            (pChObj->inFrameProcessTime)/(pChObj->inFrameProcessCount)
            );
    }
   
    Vps_printf( " \n");
   
    return FVID2_SOK;
}


