/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup NSF_LINK_API Noise Filter (NSF / NF) Link API

    @{
*/

/**
    \file nsfLink.h
    \brief Noise Filter (NSF / NF) Link API
*/

#ifndef _NSF_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _NSF_LINK_H_


#include <mcfw/interfaces/link_api/system.h>

#define NSF_LINK_MAX_OUT_QUE 						(2)

/* Maximum NF channels per output queue */
#define NSF_LINK_MAX_CH_PER_QUE             		(4)

/** \NSF command to set Frame Rate */
#define NSF_LINK_CMD_SET_FRAME_RATE          		(0x8001)

#define NSF_LINK_CMD_PRINT_STATISTICS        		(0x8002)

#define NSF_LINK_CMD_PRINT_BUFFER_STATISTICS        (0x8003)

#define NSF_LINK_CMD_SET_LEVEL	 					(0x8004)

#define NSF_LINK_CMD_ENABLE_SNF	 					(0x8005)

#define NSF_LINK_CMD_ENABLE_TNF	 					(0x8006)

#define NSF_LINK_CMD_NSF_STRENGTH	 				(0x8007)

#define NSF_LINK_CMD_ENABLE_CHANNEL					(0x8008)

#define NSF_LINK_CMD_DISABLE_CHANNEL				(0x8009)

/** \brief Indicates number of output buffers to be set to default
 *         value by the nsf link
 */
#define NSF_LINK_NUM_BUFS_PER_CH_DEFAULT 			(0)

typedef enum
{
    NSF_LINK_BYPASS_NONE = 0,	/**< Full NF, Spatial NF ON + Temporal NF ON */
    NSF_LINK_BYPASS_SNF,   		/**< Only Temporal NF ON, Spatial NF bypassed */
    NSF_LINK_BYPASS_TNF,   		/**< Only Spatial NF ON, Temporal NF bypassed */
    NSF_LINK_BYPASS_SNF_TNF, 	/**< No NF, Only YUV422 to YUV420 downsample ON */
    NSF_LINK_BYPASS_MODE_MAX    /**< Max bypass mode value */
} NSF_LINK_BYPASS_MODE;

/**
 * @enum NSF_LINK_NF_STRENGTH
 * NSF strength
*/
typedef enum {
	NSF_LINK_NF_STRENGTH_AUTO = 0,
	NSF_LINK_NF_STRENGTH_LOW,
	NSF_LINK_NF_STRENGTH_MIDDLE,
	NSF_LINK_NF_STRENGTH_HIGH,
} NSF_LINK_NF_STRENGTH;

typedef struct
{
    System_LinkInQueParams      inQueParams;

    /* Since NSF link can have 2 output queues, incoming channels will
     * be splitted in half automatically if user enables both the queues.
     * Channels {0 to (incomingChannels/2 - 1)} will goto output queue 0 and
     * channels {incomingChannels/2 to incomingChannels} will goto output queue 1.
     * If only 1 output queue is enabled, incoming channels will not be
     * splitted and sent only to output queue 0.
     * For e.g.:
     * Incoming channels = 16, numOutQue = 1 -> outQueue0 = 16, outQueue1 = 0
     * Incoming channels = 16, numOutQue = 2 -> outQueue0 = 8, outQueue1 = 8
     * Incoming channels = 8, numOutQue = 1 -> outQueue0 = 8, outQueue1 = 0
     * Incoming channels = 8, numOutQue = 2 -> outQueue0 = 4, outQueue1 = 4
     */
    UInt32                      numOutQue;
    System_LinkOutQueParams     outQueParams[NSF_LINK_MAX_OUT_QUE];

    UInt32                      bypassNsf; /* applied for all Chs, do only Chroma DS */
    UInt32                      tilerEnable;

    UInt32                      numBufsPerCh;
    /**< Number of output buffers per channel in capture */
    
    UInt32                     inputFrameRate;
    UInt32                     outputFrameRate;

    NSF_LINK_NF_STRENGTH 		nsfStrength;
	
	UInt32 enable[NSF_LINK_MAX_CH_PER_QUE];

} NsfLink_CreateParams;

/**
    \brief NSF link channel dynamic set config params

    Defines Dei FPS parameters that can be changed dynamically
    on a per channel basis
*/
typedef struct NsfLink_ChFpsParams
{
    UInt32 chId;
    /**< Dei channel number */
    UInt32 inputFrameRate;
    /**< input frame rate - 60 or 50 fps if interlaced */
    UInt32 outputFrameRate;
    /**< Expected output frame rate */
} NsfLink_ChFpsParams;


Int32 NsfLink_init();
Int32 NsfLink_deInit();

static inline void NsfLink_CreateParams_Init(NsfLink_CreateParams *pPrm)
{
	UInt32 i;

    memset(pPrm, 0, sizeof(*pPrm));
    
    pPrm->bypassNsf       = TRUE;
    pPrm->tilerEnable     = FALSE;
    pPrm->numBufsPerCh    = NSF_LINK_NUM_BUFS_PER_CH_DEFAULT;
    pPrm->inputFrameRate  = 30;
    pPrm->outputFrameRate = 30;
	
	for(i = 0;i < NSF_LINK_MAX_CH_PER_QUE;i ++)
	{
		pPrm->enable[i] = 1;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

