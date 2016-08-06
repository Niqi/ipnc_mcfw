/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
    \addtogroup HLOS_Links          HLOS links

    @{
    \defgroup   IpcFramesInLink_HLOS  Video Frame receive link

    The IpcFramesInLink_HLOS is used to receive video frames defined
    by VIDFrame_Buf from a remote core's IpcFramesOutLink.
    On receiving the video frame  an application defined callback
    funciton is invoked. @sa IpcFramesInCbFcn
    @{
*/

/**
    \file  mcfw/src_linux/links/ipcFramesIn/ipcFramesInLink_priv.h
    \brief Ipc Frames Import Link private data structures definition
*/

#ifndef _IPC_COMMUNICATION_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IPC_COMMUNICATION_LINK_PRIV_H_

#include <stdio.h>
#include <osa.h>
#include <osa_que.h>
#include <osa_tsk.h>
#include <osa_debug.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/List.h>
#include <mcfw/interfaces/link_api/ipcLink.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_debug.h>
#include <mcfw/src_linux/links/system/system_priv_ipc.h>
#include <mcfw/interfaces/link_api/vidframe.h>
#include <mcfw/interfaces/link_api/algVehicleLink.h>
#include <mcfw/interfaces/link_api/communicationIpcLink.h>


typedef struct IpcComM3Link_Obj {
    UInt32 tskId;
    OSA_TskHndl tsk;
    CommunicationLink_CreateParams createArgs;
} IpcCommunicationLink_Obj;

//Int32 IpcCommunicationLink_init();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _IPC_COMMUNICATION_LINK_PRIV_H_
                                                            */
