// ======================================================================== 
//																			
//  AUTHORS         					  	            
//    Darnell J. Moore	(DJM)			       		            
//									    
//  CONTACT                						    
//	  djmoore@ti.com, +1 214 480 7422
//    DSP Solutions R&D Center, Dallas, TX
//    Video & Image Processing Laboratory
//    Embedded Vision Branch
//									    
//  FILE NAME								    
//	  scd.h							    
//									    
//  REVISION HISTORY                                                        
//    Oct 14, 2011 * DJM created
//    Dec  5, 2011 * DJM added support for motion detection
//									    
//  DESCRIPTION                                                             
//	  Public header file for Scene Change Detection algorithm, which 
//    enables the following:
//    a) detection of change/motion in individual image tiles 
//    b) detection of global changes to scene consistent with basic camera
//       tampering events, e.g. fully blocking lens, blinding with flashlight,
//       etc.
//
// ------------------------------------------------------------------------ 
//         Copyright (c) 2011 Texas Instruments, Incorporated.           
//                           All Rights Reserved.                           
// ======================================================================== 
#ifndef _THPLATEIDALG_H_
#define _THPLATEIDALG_H_

#include <xdc/std.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/ires.h>
#include "common/TH_ErrorDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THPLATEIDALG_SOK    0  ///< Status : OK
#define THPLATEIDALG_EFAIL  -1 ///< Status : Generic error


typedef enum
{
	THPLATEID_NO_ERROR = 0,
	THPLATEID_ERR_INSTANCE_CREATE_FAILED = 1,
	THPLATEID_ERR_INPUT_INVALID = 2,
	THPLATEID_ERR_INPUT_INVALID_FRAME = 4,
	THPLATEID_ERR_INPUT_NEGATIVE = 8,
	THPLATEID_ERR_INPUT_EXCEEDED_RANGE = 16,
	THPLATEID_ERR_MEMORY_EXCEEDED_BOUNDARY = 32,
	THPLATEID_ERR_MEMORY_INSUFFICIENT = 64,
	THPLATEID_ERR_MEMORY_POINTER_NULL = 128,
	THPLATEID_ERR_INTERNAL_FAILURE = 256,
	THPLATEID_WARNING_LOW_MEMORY = 512,
	THPLATEID_WARNING_INITIALIZING = 1024,
	THPLATEID_WARNING_PARAMETER_UNDERSPECIFIED = 2048,
	THPLATEID_WARNING_DISABLED = 4096
} THPLATEIDALG_Status;

extern IALG_Fxns	THPLATEIDALG_TI_IALG;
extern IRES_Fxns	THPLATEIDALG_TI_IRES;

/////////////////////////////////////////////////////////////////////////////
//
// PUBLIC THPLATEID DEFINED TYPES
//
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    THPLATEID_SET_INIT = 0,
    THPLATEID_SET_DEINIT = 1,    
    THPLATEID_SET_IMAGE_FORMAT  = 2,
    THPLATEID_SET_ENLARGE_MODE  = 3,
    THPLATEID_SET_ENABLED_PLATE_FORMAT  = 4,
    THPLATEID_SET_PROVINCE_ORDER  = 5,
    THPLATEID_SET_RECOGTHRESHOLD  =6,
    THPLATEID_SET_CONTRAST  = 7,
    THPLATEID_SET_ENABLE_LEAN_CORRECTION = 8,
    THPLATEID_SET_ENABLE_SHADOW = 9,
    THPLATEID_SET_RECOGREGION = 10,
    THPLATEID_SET_AUTOSLOPERECTIFY_MODE = 11,
    THPLATEID_CHECK_MIN_FREE_MEMORY = 12,
    THPLATEID_TH_GET_VERSION = 13
} ThPlateIdSetParamsMode;


typedef struct
{
    int                           errorRet;
    /**< error code */
    
    int	       nNumberOfVehicle;    
    /**< THPLATEID number of recognition results from entire frame */

    TH_PlateIDResult       thPlateIdResultAll[MAX_RECOGNITION_VEHICLE_NUMBER];
     /**< THPLATEID all of  recognition results from entire frame */

} THPLATEIDALG_Result;

/**
    \brief THPLATEID Channel parameters used to SET BEFORE CALLING PROCESS
*/
typedef struct
{
    unsigned int chId;
    /**< Unique video channel identifier, e.g. channel no. */

    ThPlateIdSetParamsMode setMode;

    unsigned int width;
    /**< Set the width (in pixels) of video frame that THPLATEIDALG will process */

    unsigned int height;
    /**< Set the height (in pixels) of video frame that THPLATEIDALG will process */

    unsigned int stride;
    /**< Set the video frame pitch/stride of the images in the video buffer*/

    void *curFrame;
    /**< Luma pointer to current frame */ 

    unsigned char cImageFormat;
    unsigned char bVertFlip;
    unsigned char bDwordAligned;
    unsigned char bEnlarge;
    unsigned char nPlateLocate_Th;
    unsigned char nOCR_Th;
    unsigned char nContrast;
    unsigned char bLeanCorrection;
    unsigned char bShadow;    
    //unsigned char bIsAutoSlope;
    //unsigned char nSlopeDetectRange;
    char szProvince[16];
    unsigned int dFormat;
    int pnMinFreeSRAM;
    int pnMinFreeSDRAM;         

    TH_RECT rcDetect;

} THPLATEIDALG_chPrm;

/**
    \brief THPLATEID Algorithm parameters (CREATE)
*/
typedef struct
{
    unsigned int  maxWidth;
    /**< Set the maximum width (in pixels) of video frame that THPLATEID will process */
    
    unsigned int  maxHeight;
    /**< Set the maximum height (in pixels) of video frame that THPLATEID will process */
	
	unsigned int  maxStride;
    /**< Set the maximum video frame pitch/stride of the images in the video buffer*/
    
    unsigned int  maxChannels;
	/**< Set the maximum number of video channels that THPLATEID will monitor (Max is 16) */

    unsigned int numSecs2WaitB4Init;
    /**< Set the number of seconds to wait before initializing THPLATEID monitoring. 
    	 This wait time occurs when the algorithm instance first starts and 
    	 allows video input to stabilize.*/

    THPLATEIDALG_chPrm	*chDefaultParams;
    /**< Pointer to array of channel params used to configure THPLATEID Algorithm. */
} THPLATEIDALG_createPrm;

/////////////////////////////////////////////////////////////////////////////
//
// PUBLIC THPLATEID APPLICATION PROGRAMMING INTERFACE (API)
//
/////////////////////////////////////////////////////////////////////////////

THPLATEIDALG_Status 
THPLATEIDALG_TI_setPrms(void         *handle,
                        unsigned int        chanID,
			   THPLATEIDALG_chPrm *pThPlateIdChPrm);

THPLATEIDALG_Status
THPLATEIDALG_TI_process(void		   *handle, 
			   unsigned int		   chanID,
			   unsigned char        *pFrameBufY,
			   unsigned char        *pFrameBufUV,
			   THPLATEIDALG_Result  *pThPlateIdResult);

#ifdef __cplusplus
}
#endif
#endif /* _THPLATEIDALG_H_ */
