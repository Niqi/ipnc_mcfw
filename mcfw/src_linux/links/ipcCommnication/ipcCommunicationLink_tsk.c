/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
#include <stddef.h>
#include <time.h>

#include "ipcCommunicationLink_priv.h"
#include <ti/syslink/utils/IHeap.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>

#include <mcfw/interfaces/common_def/ti_vdis_common_def.h>

#if 0
typedef struct TagCommunicationLprResualt{
    UInt32 payload;
	Int32 frameAddr;
	DSP_LPR_RESULT lprResult;
} TCommunicationLprResualt;
#endif

//extern Int32 DramPlrInfoToFrame(Int32 frameAddr,Int8 *resString);
//extern Int32 HalCodecSetOsdDisplayLprInfo(UInt32 timeStamp, DSP_LPR_RESULT *result);
IpcCommunicationLink_Obj gIpcCommunicationLink_obj;

static Int32 IpcCommunicationLink_create(IpcCommunicationLink_Obj * pObj,CommunicationLink_CreateParams * pPrm)
{
    OSA_printf(" %d: IPC_COMMUNICATION   : Create in progress !!!\n",OSA_getCurTimeInMsec());
    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));
    OSA_printf(" %d: IPC_COMMUNICATION   : Create Done !!!\n", OSA_getCurTimeInMsec());
    return OSA_SOK;
}

static Int32 IpcCommunicationLink_stop(IpcCommunicationLink_Obj * pObj)
{
    return OSA_SOK;
}

static Int32 IpcCommunicationLink_delete(IpcCommunicationLink_Obj * pObj)
{
    return OSA_SOK;
}

static Int32 IpcCommunicationParse(IpcCommunicationLink_Obj * pObj,Ptr pPram)
{
	TDSPLprResualt *ptsOsdPayload = NULL;
	ptsOsdPayload = (TDSPLprResualt *)pPram;
	if(ptsOsdPayload == NULL){
		return OSA_EFAIL;
	}
	switch(ptsOsdPayload->payload){
		case 0x00000001: /* from dsp */
		{
			//OSA_printf("TCommunicationLprResualt.payload = 0x%08x\n",ptsOsdPayload->payload);
			//OSA_printf("TCommunicationLprResualt.frameAddr = 0x%08x\n",ptsOsdPayload->frameAddr);
			//OSA_printf("TCommunicationLprResualt.resualtStr = %s\n",ptsOsdPayload->lprResult.license);
			//DramPlrInfoToFrame(lprRet->frameAddr,lprRet->resualtStr);
			//HalCodecSetOsdDisplayLprInfo(ptsOsdPayload->timeStamp, &ptsOsdPayload->lprResult);
			if(pObj->callbackFunc != NULL){
				pObj->callbackFunc(ptsOsdPayload->timeStamp, &ptsOsdPayload->lprResult);
			}
			break;
		}
		default:
			break;
	}

	return OSA_SOK;
}

static Int IpcCommunicationLink_tskMain(struct OSA_TskHndl * pTsk, OSA_MsgHndl * pMsg,
                            Uint32 curState)
{
    UInt32 cmd = OSA_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int status = OSA_SOK;
    IpcCommunicationLink_Obj *pObj = (IpcCommunicationLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        OSA_tskAckOrFreeMsg(pMsg, OSA_EFAIL);
        return status;
    }
    status = IpcCommunicationLink_create(pObj, OSA_msgGetPrm(pMsg));
    OSA_tskAckOrFreeMsg(pMsg, status);
    if (status != OSA_SOK)
        return status;

    done = FALSE;
    ackMsg = FALSE;
    while (!done)
    {
        status = OSA_tskWaitMsg(pTsk, &pMsg);
        if (status != OSA_SOK)
            break;

        cmd = OSA_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            case SYSTEM_CMD_NEW_DATA:
				IpcCommunicationParse(pObj,pMsg->pPrm);
				OSA_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_STOP:
                IpcCommunicationLink_stop(pObj);
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;

			case IPCCOMMUNICATION_CMD_CALLBACK:
                pObj->callbackFunc = *(HOST_CB_FUNC *)pMsg->pPrm;
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                OSA_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    IpcCommunicationLink_delete(pObj);

    OSA_printf(" %d: IPC_COMMUNICATION   : Delete Done !!!\n", OSA_getCurTimeInMsec());

    if (ackMsg && pMsg != NULL)
        OSA_tskAckOrFreeMsg(pMsg, status);

    return OSA_SOK;
}

Int32 IpcCommunicationLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    IpcCommunicationLink_Obj *pObj;
    char tskName[32];
    UInt32 procId = System_getSelfProcId();

	pObj = &gIpcCommunicationLink_obj;
	memset(pObj, 0, sizeof(*pObj));

	pObj->tskId = SYSTEM_MAKE_LINK_ID(procId, SYSTEM_HOST_LINK_ID_IPC_COMMUNICATION);

	linkObj.pTsk = &pObj->tsk;

	System_registerLink(pObj->tskId, &linkObj);

	OSA_SNPRINTF(tskName, "IPC_COMMUNICATION%d", 0);

	status = OSA_tskCreate(&pObj->tsk,
	                       IpcCommunicationLink_tskMain,
	                       OSA_THR_PRI_MAX,
	                       IPC_LINK_TSK_STACK_SIZE, 0, pObj);

    OSA_assert(status == OSA_SOK);

    return status;
}

Int32 IpcCommunicationLink_deInit()
{
    OSA_tskDelete(&gIpcCommunicationLink_obj.tsk);
    return OSA_SOK;
}

