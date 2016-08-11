/*
 *  ======== thPlateIdAlg_TI_ialg.c ========
 *  thPlateId algrithm algorithm.
 *
 *  This file contains an implementation of the interface
 *  defined by XDM.
 */
#include <xdc/std.h>
#include <string.h>
#include <stdlib.h>

#include "thPlateIdAlg_TI_priv.h"
#include "thPlateIdAlg_ti_dmaOpt.h"
#include "../../../../../mcfw/src_bios6/utils/utils_common.h"
#include "../common/TH_PlateID.h"
#include "../common/TH_ErrorDef.h"

/*---------------------------------------------------------------------------*/
/* Set MTAB_NRECS to the number of Memtabs required for the algorithm.       */
/*---------------------------------------------------------------------------*/
#define MTAB_NRECS              6

//TH_PlateIDCfg c_Config;  
TH_PlateIDCfg *pAlgConfig; 
//TH_PlateIDResult result[6];
//const char *version = NULL;

//int g_initRes = -1;
//int seg_id;

/* TODO, need to autogenerate this */
#define VERSIONSTRING "1.00.00.00"

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    1
#define MINOUTBUFSIZE   1

#define FAST_MEMORY_SIZE                (0x4000)
#define NORMAL_MEMORY_SIZE           (0x03200000)//(0x2800000)

#define IALGFXNS  \
    &THPLATEIDALG_TI_IALG,/* module ID */                        \
    NULL,               /* activate */                          \
    THPLATEIDALG_TI_alloc,/* alloc */                            \
    NULL,               /* control (NULL => no control ops) */  \
    NULL,               /* deactivate */                        \
    THPLATEIDALG_TI_free, /* free */                             \
    THPLATEIDALG_TI_initObj, /* init */                          \
    NULL,               /* moved */                             \
    THPLATEIDALG_TI_numAlloc                /* numAlloc (NULL => IALG_MAXMEMRECS) */


IALG_Fxns THPLATEIDALG_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#if 0
THPLATEIDALG_createPrm THPLATEIDALG_TI_CREATE_PARAMS = {
    sizeof(THPLATEIDALG_createPrm),       /* size */
    1920,                    /* maxWidth */
    1080,                    /* maxHeight */
    1920,                    /* maxstride */
    1,                         /* maxChannels */
    0,                         /* numSecs2WaitB4Init */
    NULL,                   /* *chDefaultParams */
};
#endif

int THPLATEIDALG_TI_numAlloc(void)
{
    return (int)(MTAB_NRECS);
}

/*
 *  ======== THPLATEID_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int THPLATEIDALG_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[])
{
    //const THPLATEIDALG_Params *params = (THPLATEIDALG_Params *)algParams;
    
    /* Request memory for my object */
    memTab[0].size = sizeof(THPLATEIDALG_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;
	    
    memTab[1].size = FAST_MEMORY_SIZE;
    memTab[1].alignment = 0;
    memTab[1].space = IALG_DARAM0;
    memTab[1].attrs = IALG_PERSIST;
	
    memTab[2].size = NORMAL_MEMORY_SIZE;
    memTab[2].alignment = 0;
    memTab[2].space = IALG_EXTERNAL;
    memTab[2].attrs = IALG_PERSIST;

    memTab[3].size = sizeof(THPLATEIDALG_chPrm);
    memTab[3].alignment = 0;
    memTab[3].space = IALG_EXTERNAL;
    memTab[3].attrs = IALG_PERSIST;  

    memTab[4].size = 1920*1080*3/2;
    memTab[4].alignment = 0;
    memTab[4].space = IALG_EXTERNAL;
    memTab[4].attrs = IALG_PERSIST;         

    memTab[5].size = sizeof(TH_PlateIDCfg);
    memTab[5].alignment = 0;
    memTab[5].space = IALG_EXTERNAL;
    memTab[5].attrs = IALG_PERSIST;

    return (MTAB_NRECS);
}


/*
 *  ======== THPLATEID_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int THPLATEIDALG_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    // move to control function	
    //TH_UniitPlateIDSDK(&c_Config);

    /*    
     * Because our internal object size doesn't depend on create
     *
     *
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */
    return(THPLATEIDALG_TI_alloc(NULL, NULL, memTab));
}


/*
 *  ======== THPLATEID_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int THPLATEIDALG_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *algParams)
{  
    const THPLATEIDALG_createPrm       *params	= (THPLATEIDALG_createPrm *)algParams;
    THPLATEIDALG_TI_Obj			*obj	= (THPLATEIDALG_TI_Obj *)handle;
    THPLATEIDALG_chPrm			*thPlateIdAlgChParams	= NULL;

    //THPLATEIDALG_Status status;  


    if (params == NULL) {
        Vps_printf("\n> THPLATEIDALG: thPlateId alg params do not specified!!!!\n");
        return (THPLATEID_WARNING_PARAMETER_UNDERSPECIFIED);        
    }       

    /*
     * In lieu of XDM defined default params, use our codec-specific ones.
     * Note that these default values _should_ be documented in your algorithm
     * documentation so users know what to expect.
     */

    if(memTab[3].base != NULL)
    {
        obj->thPlateIdAlgStaticCfg = memTab[3].base;
        thPlateIdAlgChParams = (THPLATEIDALG_chPrm *)obj->thPlateIdAlgStaticCfg;
    }
    else
    {
        Vps_printf("\n> THPLATEIDALG: Application memory allocation ==> FAILED FOR memTab 3!!!!\n");
        return (THPLATEID_ERR_MEMORY_INSUFFICIENT);
    }  

    thPlateIdAlgChParams->width = params->chDefaultParams->width;
    thPlateIdAlgChParams->height = params->chDefaultParams->height;
    thPlateIdAlgChParams->rcDetect.bottom = params->chDefaultParams[0].rcDetect.bottom;
    thPlateIdAlgChParams->rcDetect.top = params->chDefaultParams[0].rcDetect.top;
    thPlateIdAlgChParams->rcDetect.left = params->chDefaultParams[0].rcDetect.left;
    thPlateIdAlgChParams->rcDetect.right = params->chDefaultParams[0].rcDetect.right;  
    thPlateIdAlgChParams->curFrame= (unsigned char *)memTab[4].base;
	thPlateIdAlgChParams->nMaxPlateWidth = params->chDefaultParams[0].nMaxPlateWidth;
	thPlateIdAlgChParams->nMinPlateWidth = params->chDefaultParams[0].nMinPlateWidth;	

    pAlgConfig = (TH_PlateIDCfg *)memTab[5].base;

    if(NULL != pAlgConfig)
    {
        pAlgConfig->nMinPlateWidth = thPlateIdAlgChParams->nMinPlateWidth;
        pAlgConfig->nMaxPlateWidth = thPlateIdAlgChParams->nMaxPlateWidth;
        pAlgConfig->nMaxImageWidth = thPlateIdAlgChParams->width;
        pAlgConfig->nMaxImageHeight = thPlateIdAlgChParams->height;
        pAlgConfig->bVertCompress = 0;
        pAlgConfig->bIsFieldImage = 0;		
        pAlgConfig->bOutputSingleFrame = 1;
        pAlgConfig->bMovingImage = 1;	
        pAlgConfig->bIsNight = 0;	
        pAlgConfig->nImageFormat = ImageFormatYUV420COMPASS;
        pAlgConfig->pFastMemory = NULL;	
        pAlgConfig->nFastMemorySize = FAST_MEMORY_SIZE; 
        pAlgConfig->pMemory = NULL;	
        pAlgConfig->nMemorySize = NORMAL_MEMORY_SIZE;
        pAlgConfig->DMA_DataCopy = NULL;	   
        pAlgConfig->Check_DMA_Finished = NULL;
        pAlgConfig->nLastError = 0; 
        pAlgConfig->nErrorModelSN = 0; 
        pAlgConfig->nOrderOpt = 0;
        pAlgConfig->bLeanCorrection = 1;
        pAlgConfig->bMovingOutputOpt = 0;
        pAlgConfig->nImproveSpeed = 0;
        pAlgConfig->bCarLogo = 0;
        pAlgConfig->bLotDetect = 0; 
        pAlgConfig->bShadow = 1;
        pAlgConfig->bUTF8 = 0;
        pAlgConfig->bShieldRailing = 0;
        pAlgConfig->bCarModel = 0;    

        if( (NULL != memTab[1].base) && (NULL != memTab[2].base))   
        {
            pAlgConfig->pFastMemory = (unsigned char *)memTab[1].base;
            pAlgConfig->nFastMemorySize = memTab[1].size;
            pAlgConfig->pMemory = (unsigned char *)memTab[2].base;
            pAlgConfig->nMemorySize = memTab[2].size;
        }
        else
        {
            Vps_printf("\n> THPLATEIDALG: Application memory allocation ==> FAILED FOR memTab 1 or 2 !!!!\n");
        }        
    }
    else
    {
        Vps_printf("\n> THPLATEIDALG: Application memory allocation ==> FAILED FOR memTab 5 !!!!\n");
    }
    
    Vps_printf("\n THPLATEIDALG---niqi test----------.\n");

    return (THPLATEID_NO_ERROR);
}


/* ===================================================================
*  @func     THPLATEIDALG_TI_process
*
*  @desc     THPLATEID algorithm Process call to process upon the 
*            buffers received at DSP side
*
*  @modif    This function modifies the following structures
*
*  @inputs   This function takes the following inputs
*            <IALG_Handle>
*            Handle to the algoirthm 
*            <U32 chanID>
*            Input Channel id that algorithm need to process
*            <THPLATEIDALG_Result *pHelloWorldResult>
*            Pointer to output buffer that processed algorithm 
*            can write the results
*
*  @outputs  <argument name>
*            Description of usage
*
*  @return   Status of process call
*  ==================================================================
*/
THPLATEIDALG_Status
THPLATEIDALG_TI_process(void        *handle, 
						 unsigned int        chanID,
						 unsigned char      *pFrameBufY,
						 unsigned char      *pFrameBufUV,
                         THPLATEIDALG_Result *pThPlateIdResult)
{
    THPLATEIDALG_Status	status = THPLATEID_NO_ERROR;
    THPLATEIDALG_TI_Obj	*obj	= (THPLATEIDALG_TI_Obj *)handle;
    THPLATEIDALG_chPrm	*thPlateIdAlgChParams	= NULL;
    TH_RECT rcDetect;
    int recRet= 65535; 
    int nNumberOfVehicle; 

    if( (NULL == handle) || (NULL == obj->thPlateIdAlgStaticCfg))
    {
        return THPLATEID_WARNING_PARAMETER_UNDERSPECIFIED;
    }
 
    thPlateIdAlgChParams = (THPLATEIDALG_chPrm *)obj->thPlateIdAlgStaticCfg;

    rcDetect.bottom = thPlateIdAlgChParams->rcDetect.bottom;
    rcDetect.top = thPlateIdAlgChParams->rcDetect.top;
    rcDetect.left = thPlateIdAlgChParams->rcDetect.left;
    rcDetect.right = thPlateIdAlgChParams->rcDetect.right; 
	pAlgConfig->nMaxPlateWidth = thPlateIdAlgChParams->nMaxPlateWidth;
	pAlgConfig->nMinPlateWidth = thPlateIdAlgChParams->nMinPlateWidth;
    nNumberOfVehicle = 1;

	//reserved for down sampling resizer
    if(0)
    {
        ECPY_activate(obj->ecpyHandle[THPLATEIDALG_DMA_CH_ALP]);
        THPLATEIDALG_TI_DMA_Fast2D2D(
            &obj->dmaHandle,
            THPLATEIDALG_DMA_CH_ALP,
            (void *)pFrameBufY,
            (void *)thPlateIdAlgChParams->curFrame,
            thPlateIdAlgChParams->width,
            thPlateIdAlgChParams->height*3/2,
            thPlateIdAlgChParams->width,
            thPlateIdAlgChParams->width
        );    
        THPLATEIDALG_TI_DMA_FastWait(&obj->dmaHandle);
        ECPY_deactivate(obj->ecpyHandle[THPLATEIDALG_DMA_CH_ALP]);    
    }

    Cache_inv(pFrameBufY,
                    (thPlateIdAlgChParams->width * thPlateIdAlgChParams->height * 3 /2),
                    Cache_Type_ALL, TRUE);    

    if(1)
    {
    	
        recRet = TH_RecogImage(pFrameBufY,//(unsigned char *)thPlateIdAlgChParams->curFrame , 
                                                thPlateIdAlgChParams->width, thPlateIdAlgChParams->height,
                                                &pThPlateIdResult->thPlateIdResultAll[0], &nNumberOfVehicle, 
                                                &rcDetect, pAlgConfig); 
    }

    pThPlateIdResult->errorRet = recRet;
    pThPlateIdResult->nNumberOfVehicle= nNumberOfVehicle;

	if(TH_ERR_NONE != recRet)
	{
		status = THPLATEID_ERR_INTERNAL_FAILURE;
	}

    return status;
}


/* ===================================================================
*  @func     THPLATEID_TI_setPrms
*
*  @desc     Once DSKT2 memory manager has allocated memory, it makes 
*       
     this function call. Here algorithm get pointers to those 
*       
     memory and initializes it's state buffer.
*
*  @modif    This function modifies the following structures
*
*  @inputs   This function takes the following inputs
*       
     <IALG_Handle>
*       
     Handle to the algoirthm 
*       
    <IALG_MemRec>
*       
     Memory table records with allocated buffer pointers
*       
     <IALG_Handle>
*       
     Not used by algorithm. This is to maintain IALG interface
*       
     protocol. 
*       
     <IALG_Params>
*       
     Algorithm create time parameters
*
*  @outputs  <argument name>
*       
     Description of usage
*
*  @return   Status of process call
*  =====
=============================================================
*/
THPLATEIDALG_Status 
THPLATEIDALG_TI_setPrms(void         	*handle,        
       unsigned int        chanID, ThPlateIdSetParamsMode setMode,
       THPLATEIDALG_chPrm   *pThPlateIdChPrm)
{
    THPLATEIDALG_Status	status = THPLATEID_NO_ERROR;
    THPLATEIDALG_TI_Obj	*obj	= (THPLATEIDALG_TI_Obj *)handle;
    THPLATEIDALG_chPrm	*thPlateIdAlgChParams	= NULL;
    Int thPlateIdRetVal = 0;
    const char *version = NULL;

    if((NULL == handle) || (NULL == obj->thPlateIdAlgStaticCfg))
    {      
        Vps_printf("THPLATEIDALG:setPrms handle null!! \n");
        return THPLATEID_WARNING_PARAMETER_UNDERSPECIFIED;
    }
  
    //thPlateIdAlgChParams = (THPLATEIDALG_chPrm *)obj->thPlateIdAlgStaticCfg;
    /* All run time dynamic params settings here */

    switch(setMode)
    { 
        case THPLATEID_SET_INIT:    
        thPlateIdRetVal = TH_InitPlateIDSDK(pAlgConfig);
        Vps_printf("THPLATEIDALG:SET_INIT :%s \n", TH_GetVersion());   
        break;
 
        case THPLATEID_SET_DEINIT:          
            thPlateIdRetVal = TH_UninitPlateIDSDK(pAlgConfig);        
            Vps_printf("THPLATEIDALG:SET_DEINIT \n");
            break;
     
        case THPLATEID_SET_IMAGE_FORMAT:    
                
            thPlateIdRetVal = TH_SetImageFormat(pThPlateIdChPrm->cImageFormat,       
                                                                        pThPlateIdChPrm->bVertFlip,        
                                                                        pThPlateIdChPrm->bDwordAligned,        
                                                                        pAlgConfig);              
            Vps_printf("THPLATEIDALG:SET_IMAGE_FORMAT,%d,%d,%d \n", pThPlateIdChPrm->cImageFormat,       
                                                                                                            pThPlateIdChPrm->bVertFlip,       
                                                                                                            pThPlateIdChPrm->bDwordAligned);                  
            break;
                
        case THPLATEID_SET_ENLARGE_MODE: 
            Vps_printf("THPLATEIDALG:SET_ENLARGE_MODE \n");              
            break;

        case THPLATEID_SET_ENABLED_PLATE_FORMAT:
            thPlateIdRetVal = TH_SetEnabledPlateFormat(pThPlateIdChPrm->dFormat, pAlgConfig);
            //thPlateIdRetVal = TH_SetEnabledPlateFormat(PARAM_ARMPOLICE_ON, pAlgConfig);
            //thPlateIdRetVal = TH_SetEnabledPlateFormat(PARAM_TWOROWARMY_ON, pAlgConfig);
            //thPlateIdRetVal = TH_SetEnabledPlateFormat(PARAM_ARMPOLICE2_ON, pAlgConfig);
												
            Vps_printf("THPLATEIDALG:SET_ENABLED_PLATE_FORMAT,%d \n", pThPlateIdChPrm->dFormat);
            break;

        case THPLATEID_SET_RECOGTHRESHOLD:
            thPlateIdRetVal = TH_SetRecogThreshold(pThPlateIdChPrm->nPlateLocate_Th,        
                                                                            pThPlateIdChPrm->nOCR_Th,        
                                                                            pAlgConfig);  
            Vps_printf("THPLATEIDALG:SET_RECOGTHRESHOLD,%d, %d \n",pThPlateIdChPrm->nPlateLocate_Th,       
                                                                                                            pThPlateIdChPrm->nOCR_Th);
            break;

        case THPLATEID_SET_PROVINCE_ORDER:
            thPlateIdRetVal = TH_SetProvinceOrder(&pThPlateIdChPrm->szProvince[0],        
                                                                        pAlgConfig);   
            Vps_printf("THPLATEIDALG:SET_PROVINCE_ORDER,%s,%d \n", pThPlateIdChPrm->szProvince, thPlateIdRetVal);  
            break;            

        case THPLATEID_SET_CONTRAST:
            thPlateIdRetVal = TH_SetContrast(pThPlateIdChPrm->nContrast, pAlgConfig); 
            Vps_printf("THPLATEIDALG:SET_CONTRAST,%d \n", pThPlateIdChPrm->nContrast); 
            break;

        case THPLATEID_SET_ENABLE_LEAN_CORRECTION:
            thPlateIdRetVal = TH_SetEnableLeanCorrection(pThPlateIdChPrm->bLeanCorrection, pAlgConfig); 
            Vps_printf("THPLATEIDALG:SET_LEAN_CORRECTION,%d \n", pThPlateIdChPrm->bLeanCorrection);            
            break;

        case THPLATEID_SET_ENABLE_SHADOW:
            thPlateIdRetVal = TH_SetEnableShadow(pThPlateIdChPrm->bShadow, pAlgConfig); 
            Vps_printf("THPLATEIDALG:SET_SHADOW,%d \n", pThPlateIdChPrm->bShadow);            
            break;  

        case THPLATEID_SET_RECOGREGION:
            thPlateIdAlgChParams->rcDetect.bottom = pThPlateIdChPrm->rcDetect.bottom;
            thPlateIdAlgChParams->rcDetect.top = pThPlateIdChPrm->rcDetect.top;
            thPlateIdAlgChParams->rcDetect.left= pThPlateIdChPrm->rcDetect.left;
            thPlateIdAlgChParams->rcDetect.right= pThPlateIdChPrm->rcDetect.right;
            Vps_printf("THPLATEIDALG:RECOGREGION,%d:%d:%d:%d \n", pThPlateIdChPrm->rcDetect.left,
															pThPlateIdChPrm->rcDetect.right,
															pThPlateIdChPrm->rcDetect.top,
															pThPlateIdChPrm->rcDetect.bottom);            
            break;  

		case THPLATEID_SET_PLATE_WIDTH:
			thPlateIdAlgChParams->nMaxPlateWidth = pThPlateIdChPrm->nMaxPlateWidth;
			thPlateIdAlgChParams->nMinPlateWidth = pThPlateIdChPrm->nMinPlateWidth;		
			Vps_printf("THPLATEIDALG:PLATE_WIDTH,%d,%d \n", pThPlateIdChPrm->nMaxPlateWidth,
															pThPlateIdChPrm->nMinPlateWidth);
			break;
        
        case THPLATEID_SET_AUTOSLOPERECTIFY_MODE:
            Vps_printf("THPLATEIDALG:SET_AUTOSLOPERECTIFY_MODE \n");
            break;

        case THPLATEID_CHECK_MIN_FREE_MEMORY:
            Vps_printf("THPLATEIDALG:CHECK_MIN_FREE_MEMORY \n");
            break;

        case THPLATEID_TH_GET_VERSION:
            version = TH_GetVersion();
            Vps_printf("THPLATEIDALG:TH_GET_VERSION: %s \n", version);
            break;

        default:
            break;
    } 
   
    if(TH_ERR_NONE != thPlateIdRetVal)
    {      
        Vps_printf("THPLATEIDALG:SET_PARAMS: %d\n", thPlateIdRetVal);            
        status = THPLATEID_ERR_INTERNAL_FAILURE;
    }

    return status;
}

