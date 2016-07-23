/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *
 */

/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef _THPLATEIDALG_TI_PRIV_
#define _THPLATEIDALG_TI_PRIV_

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/sdo/fc/dskt2/dskt2.h>
#include <mcfw/interfaces/link_api/systemLink_common.h>

#include <ti/xdais/ialg.h>
#include <ti/xdais/ires.h>
#include <ti/sdo/fc/ires/edma3chan/ires_edma3Chan.h>
#include <ti/sdo/fc/ecpy/ecpy.h>
//#include <ti/sdo/fc/dskt2/dskt2.h>
#include <ti/sdo/fc/rman/rman.h>

#include "thPlateIdAlg.h"

// define's
#define THPLATEIDALG_DMA_HNDL_LINKED_DMA   0
#define THPLATEIDALG_DMA_HNDL_MEMCPY       1
#define THPLATEIDALG_DMA_HNDL_MAX          4//(THPLATEIDALG_DMA_HNDL_MEMCPY+1)

#define THPLATEIDALG_DMA_CH_IN_A  0x0
#define THPLATEIDALG_DMA_CH_IN_B  0x1
#define THPLATEIDALG_DMA_CH_ALP   0x2
#define THPLATEIDALG_DMA_CH_OUT  0x3
#define THPLATEIDALG_DMA_CH_AUX  0x4

#define THPLATEIDALG_DMA_CH_MAX   (THPLATEIDALG_DMA_CH_AUX+1)

// data structure's

typedef struct THPLATEIDALG_DMAObj {
	IRES_EDMA3CHAN2_Handle edma3ResourceHandles[THPLATEIDALG_DMA_CH_MAX];
	UInt32 *edma_params_array[THPLATEIDALG_DMA_CH_MAX];
	UInt32 edma_tcc_array[THPLATEIDALG_DMA_CH_MAX];
	UInt32 edma_phy_to_lgl_map[THPLATEIDALG_DMA_CH_MAX];
	UInt32 num_edma_channels;
	UInt32 channel_mask_low;
	UInt32 channel_mask_high;
} THPLATEIDALG_DMAObj;

typedef struct THPLATEIDALG_TI_Obj {
    IALG_Obj    alg;            /* MUST be first field of all XDAS algs */
    void            			*thPlateIdAlgStaticCfg; /* Pointer to the private static structure of thPlateId alg */
    IRES_EDMA3CHAN2_Handle      edmaHandle[THPLATEIDALG_DMA_CH_MAX];
    ECPY_Handle                 ecpyHandle[THPLATEIDALG_DMA_CH_MAX];    
    IRES_ProtocolRevision       edmaRev;  //{2,0,0} Version of resource;
    IRES_EDMA3CHAN_ProtocolArgs edma3ProtocolArgs[THPLATEIDALG_DMA_CH_MAX];
    THPLATEIDALG_DMAObj dmaHandle;
} THPLATEIDALG_TI_Obj;


int THPLATEIDALG_TI_numAlloc(void);

int THPLATEIDALG_TI_alloc(const IALG_Params *algParams, IALG_Fxns **pf,
    IALG_MemRec memTab[]);

int THPLATEIDALG_TI_free(IALG_Handle handle, IALG_MemRec memTab[]);

int THPLATEIDALG_TI_initObj(IALG_Handle handle,
    const IALG_MemRec memTab[], IALG_Handle parent,
    const IALG_Params *algParams);

#endif

