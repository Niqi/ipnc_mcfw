/*
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

#include <thPlateIdAlg_TI_priv.h>

int THPLATEIDALG_TI_algMemcpy2D(THPLATEIDALG_TI_Obj *thPlateIdAlgObj, UInt8 *dest, UInt8 *src, UInt16 width, UInt16 height, UInt16 lineOffsetSrc, UInt16 lineOffsetDest)
{
  ECPY_Params params;

  if(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY]==NULL)
    return THPLATEIDALG_EFAIL;

  ECPY_activate(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY]);

  /* If src or dst is internal memory, offset the address by 0x30000000 */
  /* as all internal memory via EDMA is accessed using L3 address space */
  if(((UInt32)src >= 0x10800000u) && ((UInt32)src < 0x10840000u))
  {
	  src += 0x30000000;
  }
  if(((UInt32)dest >= 0x10800000u) && ((UInt32)dest < 0x10840000u))
  {
	  dest += 0x30000000;
  }

  params.transferType = ECPY_2D2D;
  params.srcAddr = src;
  params.dstAddr = dest;
  params.elementSize = width;
  params.numElements = height;
  params.numFrames = 1;
  params.srcElementIndex = lineOffsetSrc;
  params.dstElementIndex = lineOffsetDest;
  params.srcFrameIndex = 0;
  params.dstFrameIndex = 0;

  ECPY_directConfigure(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY], &params, 1);

  /* submit as no linked transfers */
  ECPY_directSetFinal(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY], 1);

  ECPY_directStartEdma(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY]);
  ECPY_directWait(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY]);

  ECPY_deactivate(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_MEMCPY]);

  return THPLATEIDALG_SOK;
}

void THPLATEIDALG_TI_dmaActivate(THPLATEIDALG_TI_Obj *thPlateIdAlgObj)
{
  ECPY_activate(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA]);
}

void THPLATEIDALG_TI_dmaDeactivate(THPLATEIDALG_TI_Obj *thPlateIdAlgObj)
{
  ECPY_deactivate(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA]);
}


void THPLATEIDALG_TI_dmaStart(THPLATEIDALG_TI_Obj *thPlateIdAlgObj,
                       UInt8 *pInA, UInt8 *pInB,
                       UInt8 *pInAlp, UInt8 *pOut,
                       UInt64 *pLineBufA, UInt64 *pLineBufB,
                       Int32 *pLineBufAlp, UInt64 *pLineBufOut,
					   UInt16 width
                      )
{
	ECPY_Params params;

	params.transferType = ECPY_1D1D;
	params.srcAddr = (Ptr)pInA;
	params.dstAddr = (Ptr)((Uns)pLineBufA + 0x30000000);
	params.numElements = 1;
	params.numFrames = 1;
	params.srcElementIndex = 0;
	params.dstElementIndex = 0;
	params.srcFrameIndex = 0;
	params.dstFrameIndex = 0;
    params.elementSize = width;

    ECPY_directConfigure(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], &params, THPLATEIDALG_DMA_CH_IN_A + 1);


	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pLineBufB + 0x30000000, THPLATEIDALG_DMA_CH_IN_B + 1);
	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pInB, THPLATEIDALG_DMA_CH_IN_B + 1);

	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pLineBufAlp + 0x30000000, THPLATEIDALG_DMA_CH_ALP + 1);
	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pInAlp, THPLATEIDALG_DMA_CH_ALP + 1);

	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_DSTADDR, (Uns)pOut, THPLATEIDALG_DMA_CH_OUT + 1);
	ECPY_directConfigure32(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], ECPY_PARAMFIELD_SRCADDR, (Uns)pLineBufOut + 0x30000000, THPLATEIDALG_DMA_CH_OUT + 1);

      /* submit as a four linked 1D transfers */
	ECPY_directSetFinal(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], THPLATEIDALG_DMA_CH_MAX);

	ECPY_directStartEdma(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA]);
}

void THPLATEIDALG_TI_dmaWait(THPLATEIDALG_TI_Obj *thPlateIdAlgObj)
{
  ECPY_directWait(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA]);
}

void THPLATEIDALG_TI_dmaSetup(THPLATEIDALG_TI_Obj *thPlateIdAlgObj, UInt16 width, Bool dmaAlpha)
{
  ECPY_Params params;
  int i;

  if(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA]==NULL)
    return;

  params.transferType = ECPY_1D1D;
  params.srcAddr = NULL;
  params.dstAddr = NULL;
  params.numElements = 1;
  params.numFrames = 1;
  params.srcElementIndex = 0;
  params.dstElementIndex = 0;
  params.srcFrameIndex = 0;
  params.dstFrameIndex = 0;

  ECPY_setEarlyCompletionMode(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], 0);

  for(i=0; i<THPLATEIDALG_DMA_CH_MAX; i++) {

    if(i==THPLATEIDALG_DMA_CH_ALP) {
      // alpha
      if(dmaAlpha)
        params.elementSize = width;
      else
        params.elementSize = 0;
    } else {
      params.elementSize = width;
    }

    ECPY_directConfigure(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], &params, i+1);
  }

      /* submit as a four linked 1D transfers */
    ECPY_directSetFinal(thPlateIdAlgObj->ecpyHandle[THPLATEIDALG_DMA_HNDL_LINKED_DMA], THPLATEIDALG_DMA_CH_MAX);
}
