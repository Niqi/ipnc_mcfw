/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup LINK_API
    \defgroup ALGVEHICLE_LINK_API ALG Link API

    ALG Link can be used to apply some algorithms <THPLATEID, ,..> on channel data

    @{
*/

/**
    \file ALGLink.h
    \brief ALG Link API
*/

#ifndef _ALGVEHICLE_LINK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALGVEHICLE_LINK_H_

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/src_bios6/alg/thPlateId/common/TH_ErrorDef.h>

#define ALGVEHICLE_LINK_THPLATEID_MAX_CH  (1)

/** 
  \brief Max ALG outputs/output queues 

  Currently ALG link has one output Link for THPLATEID output. 
*/
#define ALGVEHICLE_LINK_MAX_OUT_QUE  (1)

#define ALGVEHICLE_BUFFER_ALIGNMENT (32)

/**
    \brief Link CMD: Configure THPLATEID params
*/
#define ALGVEHICLE_LINK_THPLATEID_CMD_INIT_PLATEIDSDK							(0x8001)
#define ALGVEHICLE_LINK_THPLATEID_CMD_DEINIT_PLATEIDSDK							(0x8002)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_IMAGE_FORMAT							(0x8003)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_PLATE_FORMAT					(0x8004)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOGTHRESHOLD						(0x8005)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_PROVINCE_ORDER						(0x8006)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENLARGE_MODE							(0x8007)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_CONTRAST								(0x8008)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLE_LEAN_CORRECTION				(0x8009)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_ENABLED_SHADOW						(0x800a)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_RECOG_AREA							(0x800b)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_TRIGG_INFO							(0x800c)
#define ALGVEHICLE_LINK_THPLATEID_CMD_SET_PLATE_WIDTH							(0x800d)
#define ALGVEHICLE_LINK_THPLATEID_CMD_PRINT_STATISTICS							(0x800e)


/**
    \brief ThPlateId result parameters for application interface
*/
typedef struct {
	char	license[16];	// 车牌字符串
	char	color[8];		// 车牌颜色
	int 	nColor;			// 车牌颜色
	int		nType;			// 车牌类型
	int		nRectLeftX;		// 车牌坐标左上角X
	int		nRectLeftY;		// 车牌坐标左上角Y
	int		nRectWidth;		// 车牌宽度
	int		nRectHeight;	// 车牌高度
} DSP_LPR_RESULT;

typedef struct TagDSPLprResualt{
    UInt32 payload;
    UInt32 timeStamp;
    DSP_LPR_RESULT lprResult;
} TDSPLprResualt;


/**
    \brief ThPlateId Dynamic parameters for application interface 
*/
#define ALGVEHICLE_LINK_MAX_POLYGON_POINT_NUMBER 4

typedef struct
{
	int x;
	int y;
} AlgLprPoint;

typedef struct 
{
	AlgLprPoint arr[4];//0,1,2,3 左上角为0，依次按顺时针排序4个点组成的四边形
} AlgLprPolygonArea;

typedef struct 
{
	int nMax;			// 检测的最大车牌宽度，以像素为单位//
	int nMin;			// 检测的最小车牌宽度，以像素为单位//
} AlgLprPlateWidth;

typedef struct 
{
	int nTrigMode;			// 识别结果输出触发模式
	int nTrigInterval;		// 相同车牌触发时间间隔	
	int nVehicleDirection;		// 车辆通过方向
	AlgLprPolygonArea trigArea;	// 触发区域(虚拟线圈)
} AlgLprTriggerInfo;

typedef struct
{
	//unsigned int chId;			// 通道ID
	char szProvince[16];			// 默认省份	
	int nPlateTypeSupt;				// 支持的车牌类型
	AlgLprPlateWidth plateWidth;	// 最大最小车牌宽度	
	AlgLprPolygonArea recogArea;	// 识别区域//
	AlgLprTriggerInfo trigInfo;		// 算法输出触发条件

} AlgVehicleLink_ThPlateIdDynParams;


/**
    \brief ThPlateId channel control

    Defines variables to consume ThPlateId-generated metadata to 
	determine if tamper condition exist as well as if change
	occurred within block    
*/
typedef struct
{
    UInt32 size;
    /**< Size of this struct */

    UInt32 chId;
    /**< ThPlateId channel number */

} AlgVehicleLink_ThPlateIdChStatus;


/**
    \brief ThPlateId link algorithm output 

    Defines variable that ThPlateId algorithm will update.
    Generated for every frame.
*/
typedef struct 
{
    //UInt32                      chId;
    /**< ThPlateId channel number */
    
    int                           errorRet;
    /**< error code */
    
    unsigned int	       nNumberOfVehicle;    
    /**< THPLATEID number of recognition results from entire frame */

    TH_PlateIDResult       thPlateIdResultAll[MAX_RECOGNITION_VEHICLE_NUMBER];
     /**< THPLATEID all of  recognition results from entire frame */

} AlgVehicleLink_ThPlateIdResult;

/**
    \brief THPLATEID link config parameters
*/
typedef struct
{
    unsigned int chId;
    /**< Unique video channel identifier, e.g. channel no. */

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
    unsigned int dFormat;
    int pnMinFreeSRAM;
    int pnMinFreeSDRAM;

    char szProvince[16];
    AlgLprPlateWidth plateWidth;

    TH_RECT rcDetect;
    TH_RECT rcTrig;
    
} AlgVehicleLink_ThPlateIdChParams;


/**
    \brief AlgVehicleLink create parameters
*/
typedef struct
{
    UInt32 maxWidth;
    /**< Set the maximum width (in pixels) of video frame that ThPlateId will process */

    UInt32 maxHeight;
    /**< Set the maximum height (in pixels) of video frame that ThPlateId will process */

    UInt32 maxStride;
    /**< Set the maximum video frame pitch/stride of the images in the video buffer*/

    UInt32 numCh;
    /**< Set the maximum number of video channels that ThPlateId will monitor (Max is 1) */

    UInt32 numSecs2WaitB4Init;
    /**< Set the number of seconds to wait before initializing ThPlateId monitoring. 
         This wait time occurs when the algorithm instance first starts and 
         allows video input to stabilize.*/

    UInt32 inputFrameRate;
    /**< Frames per second fed to the ThPlateId link*/

    UInt32 outputFrameRate;
    /**< Frames per second alogorithm is operated at*/

    UInt32 ThPlateIdChEnableFlag[ALGVEHICLE_LINK_THPLATEID_MAX_CH]; 
    /**< Set flag to install ThPlateId on selective channels <CIF resolution> */

    AlgVehicleLink_ThPlateIdChParams chDefaultParams[ALGVEHICLE_LINK_THPLATEID_MAX_CH];  
    /**< Pointer to array of channel params used to configure ThPlateIdAlgLink. */

    UInt32 numBufPerCh;
    /**< Number of buffer to allocate per channel*/

    UInt32 numValidChForTHPLATEID;
    /**< Set the maximum channels on which ThPlateId will run. 
          startChNoForTHPLATEID + numValidChForTHPLATEID should be less than MAX Input channels 
          Still all parameter settings for THPLATEID will be for Ch Num 0 ~ numValidChForTHPLATEID.
          startChNoForTHPLATEID, numValidChForTHPLATEID are used only to separate valid channels from an input queue having
          other channels which dont require THPLATEID 
         */

} AlgVehicleLink_ThPlateIdCreateParams;


/**
    \brief ALG Vehicle link create parameters
*/
typedef struct
{

    Int32   enableThPlateIdAlg;
    /**< Flag to enable ThPlateId Algorithm */

    System_LinkInQueParams  inQueParams;     
    /**< Input queue information. */

    System_LinkOutQueParams outQueParams;    
    /**< Output queue information. */

    AlgVehicleLink_ThPlateIdCreateParams    thPlateIdCreateParams;
    /**< create time THPLATEID params for all channels. */   
    
} AlgVehicleLink_CreateParams;


/**
    \brief AlgVehicle link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 AlgVehicleLink_init();

/**
    \brief AlgVehicle link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 AlgVehicleLink_deInit();


static inline void AlgVehicleLink_CreateParams_Init(AlgVehicleLink_CreateParams *pPrm)
{
    memset(pPrm, 0, sizeof(*pPrm));

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->outQueParams.nextLink  = SYSTEM_LINK_ID_INVALID;

    pPrm->enableThPlateIdAlg = TRUE;

    pPrm->thPlateIdCreateParams.maxWidth    = 1920;
    pPrm->thPlateIdCreateParams.maxHeight   = 1080;
    pPrm->thPlateIdCreateParams.maxStride	    = 1920;  
    pPrm->thPlateIdCreateParams.numCh           = 1;    
    pPrm->thPlateIdCreateParams.numSecs2WaitB4Init	= 3;
    pPrm->thPlateIdCreateParams.inputFrameRate        = 30;
    pPrm->thPlateIdCreateParams.outputFrameRate       = 5;  
    
    pPrm->thPlateIdCreateParams.ThPlateIdChEnableFlag[0] = 1;

    pPrm->thPlateIdCreateParams.chDefaultParams[0].chId = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].cImageFormat = ImageFormatYUV420COMPASS;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].bVertFlip = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].bDwordAligned = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].bEnlarge = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].dFormat = 2;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].nPlateLocate_Th = 5;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].nOCR_Th = 2;
    strcpy(pPrm->thPlateIdCreateParams.chDefaultParams[0].szProvince, "粤湘赣");
    pPrm->thPlateIdCreateParams.chDefaultParams[0].nContrast = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].bLeanCorrection = 1;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].bShadow = 1;    
    pPrm->thPlateIdCreateParams.chDefaultParams[0].pnMinFreeSDRAM = 0;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].pnMinFreeSRAM = 0;

	pPrm->thPlateIdCreateParams.chDefaultParams[0].plateWidth.nMax = 500;
	pPrm->thPlateIdCreateParams.chDefaultParams[0].plateWidth.nMin = 80;
   
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcDetect.top = 200;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcDetect.bottom = 1080 - 200;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcDetect.left = 300;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcDetect.right = 1920 - 300;

    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcTrig.top = 401;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcTrig.bottom = 1080 - 401;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcTrig.left = 201;
    pPrm->thPlateIdCreateParams.chDefaultParams[0].rcTrig.right = 1920 - 201;

    pPrm->thPlateIdCreateParams.numBufPerCh = 1 ;
    pPrm->thPlateIdCreateParams.numValidChForTHPLATEID = 1;
    
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/*@}*/

