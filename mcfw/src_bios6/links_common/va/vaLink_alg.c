/** ==================================================================
 *  @file   vaLink_alg.c
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
#include <mcfw/interfaces/link_api/encLink.h>
#include <mcfw/interfaces/link_api/swosdLink.h>
#include <mcfw/interfaces/link_api/cameraLink.h>


/* ===================================================================
 *  @func     VaLink_algInitParams
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
Int32 VaLink_algInitParams(VaLink_Obj *pObj)
{
	return FVID2_SOK;
}

/* VA ALG create */

/* ===================================================================
 *  @func     VaLink_algCreate
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
Int32 VaLink_algCreate(VaLink_Obj *pObj)
{
	System_getEdmaCh(&pObj->algEdmaHndl);
	System_getEdmaCh(&pObj->copyEdmaHndl);
    return FVID2_SOK;
}
/* Va link copy frames */

/* ===================================================================
 *  @func     VaLink_algCopyFrames
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
static Int32 Valink_Statistics(void)
{
	static UInt32 cCount = 0;
	static UInt32 tStart = 0;
	cCount++;
	if(cCount == 1){
		tStart = Utils_getCurTimeInMsec();
	}
	if(cCount == 100){
		UInt32 lostTime = Utils_getCurTimeInMsec() - tStart;
		Vps_rprintf("Frame rates: %d,lostTime: %d\n",100*1000/lostTime,lostTime);
		cCount = 0;
	}
	return FVID2_SOK;
}


/* Va link process frames */

/* ===================================================================
 *  @func     VaLink_algProcessFrames
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
Int32 VaLink_algProcessFrames(VaLink_Obj *pObj)
{
    Int32 status = FVID2_SOK;
	Int32 procesFrames = 0;
	FVID2_Frame *pEmptyFrame,*pFullFrame;

	while(TRUE){
		Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
		status = Utils_bufGetFullFrame(&pObj->processFrameQue, &pFullFrame, BIOS_NO_WAIT);
		if ((status != FVID2_SOK) || (pFullFrame == NULL)){
			break;
		}
		Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
		//Task_sleep(40);

		/* Put the full buffer into full queue */
        status = Utils_bufPutFullFrame(&pObj->outFrameQue, pEmptyFrame);
		UTILS_assert(status == FVID2_SOK);
		procesFrames++;
		Valink_Statistics();
	}
	if(procesFrames){
	    /* send SYSTEM_CMD_NEW_DATA to next link */
		Vps_rprintf("%s %d\n",__FUNCTION__,__LINE__);
	    System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,SYSTEM_CMD_NEW_DATA);
	}

	return FVID2_SOK;
}

/* Delete the driver instance */

/* ===================================================================
 *  @func     VaLink_algDelete
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
Int32 VaLink_algDelete(VaLink_Obj *pObj)
{
    return FVID2_SOK;
}
