/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup COMMUNICATION_LINK_API ALG Link API

    @{
*/

/**
    \file communicationLink.h
    \brief communication Link API
*/

#ifndef _COMMUNICATION_IPC_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _COMMUNICATION_IPC_LINK_H_


#include <mcfw/interfaces/link_api/system.h>

typedef struct
{
	Int32 reserve;
} CommunicationLink_CreateParams;


Int32 IpcCommunicationLink_init();
Int32 IpcCommunicationLink_deInit();


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/

