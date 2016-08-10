/** ==================================================================
 *  @file   multich_tristream_fullFeature.c
 *
 *  @path    ipnc_mcfw/mcfw/src_linux/mcfw_api/usecases/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/*------------------------------ TRIPLE OUT <1080p + D1 + 1080p> ---------------------------

                                      SENSOR
                                ________||_______
            +---SALDRE/WDR ON---|                |
            |                   |   CAMERA LINK  |
            |                   |____(VPSS M3)___|
   GLBCE_LINK/WDR(VPSSM3)          |           |
            |                      |           |
            |   ____________      SALDRE/WDR OFF
            |   |          |       |           |
            +---| ISP LINK |       |           |
                |_(VPSSM3)_|       |           |
                  |      |      |--|           |--|
                  |      |      |                 |
                  |      |------|---------------| |
                  |             |               | |
                  |----------- RSZA             RSZB
                            (1920x1080)      (1920x1080)
                            (420SP/422I)      (420SP)
                                |                |
                                |                |         
                   ------DUP_VNF(VPSSM3)---      ---------------------------
                   |                      |                                |  
                   |                      |                                |
                   |                      |                                |
             ISS_NF(VPSSME)               |----SCLR(VPSSM3)                |
                   |                             | (720*480)               |  
                   |                             | (420SP)                 | 
                   |                             |                         |
                   |                             |                         |
                   |   |--------------------------                         |
                   |   |                                                   |
                   |   |  |-----------------------                         |
                   |   |  |                      |                         |
                  MUX(VPSSM3)                    |                IPC FRAMES OUT(VPSSM3)
                   |                             |                         |
                   |                             |                         |
                  SWOSD(VPSSM3)                  |                 IPC FRAMES IN(DSP)
                   |                             |                         |
                   |                             |                         |
                  DUP_DIS(VPSSM3)                |                 VehicleAlgritm(DSP)
                   |                             |                         |
				   |                             |                         |
                   |                             |                IPC FRAMES OUT(DSP)   
             IPC OUT(VPSSM3)                     |                         |
                   |                             |                         |
                   |                             |                 IPC FRAMES IN(VPSS)
             IPC IN(VIDEOM3)                     |                         |
                   |                             |                         | 
                   |                             --------------------------|
              ENC(VIDEOM3)
                   |
                   |
           IPC BITS OUT(VIDEOM3)
                   |
                   |
             IPC BITS IN(A8)
                   |
                   |
             *************
             * STREAMING *
             *************
------------------------------------------------------------------------------------------*/

#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"

#ifdef IPNC_DSP_ON
    #define VA_ON_DSP
#endif

#define VS_W    1600
#define VS_H    900

/*in YUV sensor case, Enabling GLBCE means enabling Memory 2 Memory  */
#ifdef IMGS_OMNIVISION_OV10630
    #define YUV_M2M_DRV_TEST
#endif

UInt32 gNoiseFilterMode;

/* =============================================================================
 * Externs
 * ============================================================================= */
extern UInt32 gIsGlbceInitDone;
extern GlbcePreset gGLBCE_AlgoPreset;

/* =============================================================================
 * Use case code
 * ============================================================================= */
static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl = {
    .isPopulated = 1,
    .ivaMap[0] = {
                  .EncNumCh = 2,
                  .EncChList = {0, 1},
                  .DecNumCh = 2,
                  .DecChList = {0, 1},
                  },

};
#ifdef ENABLE_TILER
    #define TILER_ENABLE    TRUE
#else
    #define TILER_ENABLE    FALSE
#endif

#define INCLUDE_MUX_LINK_INSTEAD_OF_MERGE

/* Merge Link for camera streams */
#define NUM_MERGE_LINK                  (1)
#define CAM_STREAM_MERGE_IDX            (0)

/* Dup Links */
#define NUM_DUP_LINK                    (2)
#define VNF_DUP_IDX                     (0)//4
#define DIS_DUP_IDX						(1)


/* ===================================================================
 *  @func     MultiCh_createTriStreamFullFtr
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
Void MultiCh_createTriStreamFullFtr()
{
    UInt32 i;
    CameraLink_CreateParams cameraPrm;
    NsfLink_CreateParams nsfPrm;
    VnfLink_CreateParams vnfPrm;
    MuxLink_CreateParams muxPrm;
    IpcLink_CreateParams ipcOutVpssPrm;
    IpcLink_CreateParams ipcInVideoPrm;
    EncLink_CreateParams encPrm;
    DupLink_CreateParams dupPrm[NUM_DUP_LINK];
    SclrLink_CreateParams       sclrPrm;
    AlgVehicleLink_CreateParams vehiclePrm;
    
    SwosdLink_CreateParams swosdPrm;
    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;
    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;
    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutVpssPrm;
    IpcFramesInLinkRTOS_CreateParams ipcFramesInDspPrm;
	IpcFramesOutLinkRTOS_CreateParams ipcFramesOutDspPrm;
	IpcFramesOutLinkRTOS_CreateParams ipcFramesInVpssPrm;	

    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams *pCameraOutPrm;

    CommunicationLink_CreateParams ipcCommunicationPrm;
    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;
	UInt32 ipcFramesOutDspId;
	UInt32 ipcFramesInVpssId;

    UInt32 vehicleId;

    /* IPC struct init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams, ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams, ipcBitsInHostPrm0);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams, ipcFramesInDspPrm);
	MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesOutDspPrm);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesInVpssPrm);	

    MULTICH_INIT_STRUCT(MuxLink_CreateParams, muxPrm);

    MultiCh_detectBoard();

    System_linkControl(SYSTEM_LINK_ID_M3VPSS,
                       SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    System_linkControl(SYSTEM_LINK_ID_M3VIDEO,
                       SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL,
                       &systemVid_encDecIvaChMapTbl,
                       sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);

    /* Link IDs */
    gVcamModuleContext.cameraId = SYSTEM_LINK_ID_CAMERA;
    gVencModuleContext.encId = SYSTEM_LINK_ID_VENC_0;
    gMjpegModuleContext.mjpepEncId = SYSTEM_LINK_ID_MJPEG;
    gVsysModuleContext.muxId = SYSTEM_VPSS_LINK_ID_MUX_0;

    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
	dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;

    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
	ipcFramesOutDspId = SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0;
	ipcFramesInVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;

    vehicleId = SYSTEM_LINK_ID_ALGVEHICLE_0;
	gVsysModuleContext.vehicleLprLinkId = SYSTEM_LINK_ID_ALGVEHICLE_0;

#ifdef ENABLE_TILER_CAMERA
    /*
     * HDVPSS NSF doesn't support TILED input memory.
     * So if Camera TILER is enabled only ISS VNF is enabled.
     */
    gUI_mcfw_config.noisefilterMode = ISS_VNF_ON;
#endif

    /* Camera Link params */
    CameraLink_CreateParams_Init(&cameraPrm);

    gVcamModuleContext.nsfId = SYSTEM_LINK_ID_NSF_0;
    gVcamModuleContext.vnfId = SYSTEM_LINK_ID_VNF;
    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
    {
        cameraPrm.vnfLinkId = SYSTEM_LINK_ID_VNF;
    }

    cameraPrm.captureMode = CAMERA_LINK_CAPMODE_ISIF;

	if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON){
		cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.nsfId;
	}
	else{
		cameraPrm.outQueParams[0].nextLink = gVcamModuleContext.vnfId;
	}
    //cameraPrm.outQueParams[0].nextLink = dupId[VNF_DUP_IDX];
    cameraPrm.outQueParams[1].nextLink = ipcFramesOutVpssId;//dupId[SCALER_DUP_IDX];

    /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
     * audio channels connected in cascade */
    cameraPrm.numAudioChannels = 1;
    cameraPrm.numVipInst = 1;

#ifdef ENABLE_TILER_CAMERA
    cameraPrm.tilerEnable = TRUE;
#else
    cameraPrm.tilerEnable = FALSE;
#endif

    pCameraInstPrm = &cameraPrm.vipInst[0];
    pCameraInstPrm->vipInstId = SYSTEM_CAMERA_INST_VP_CSI2;
    pCameraInstPrm->videoDecoderId = MultiCh_getSensorId(gUI_mcfw_config.sensorId);
    pCameraInstPrm->inDataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraInstPrm->sensorOutWidth  = 1920;
    pCameraInstPrm->sensorOutHeight = 1080;

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        pCameraInstPrm->standard = SYSTEM_STD_1080P_60;
    }
    else
    {
        pCameraInstPrm->standard = SYSTEM_STD_1080P_30;
        cameraPrm.issVnfEnable = 1;
    }

    pCameraInstPrm->numOutput = 2;

    /* First stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[0];

    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV422I_UYVY;
    }
    else
    {
        pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
    }

    pCameraOutPrm->scEnable = FALSE;

    /* When VS is enabled then scOutWidth/scOutHeight cannot be equal to sensorOutWidth/sensorOutHeight */
    pCameraOutPrm->scOutWidth = 1920;
    pCameraOutPrm->scOutHeight = 1080;

    pCameraOutPrm->outQueId = 0;

    /* Second stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[1];
    pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable = FALSE;
    pCameraOutPrm->scOutWidth = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    pCameraOutPrm->standard = SYSTEM_STD_NTSC;//SYSTEM_STD_1080P_30;
    pCameraOutPrm->outQueId = 1;

    /* 2A config */
    cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;

    cameraPrm.vnfFullResolution = FALSE;

    /* NSF link params */
    NsfLink_CreateParams_Init(&nsfPrm);
    if(gUI_mcfw_config.snfEnable)
    {
        if (gUI_mcfw_config.tnfEnable)
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_NONE;
        else
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_TNF;
    }
    else
    {
        if (gUI_mcfw_config.tnfEnable)
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_SNF;
        else
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_SNF_TNF;
    }
#if (TILER_ENABLE == TRUE)
    nsfPrm.tilerEnable = TRUE;
#else
    nsfPrm.tilerEnable = FALSE;
#endif

    nsfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;//dupId[VNF_DUP_IDX];
    nsfPrm.inQueParams.prevLinkQueId = 0;
    nsfPrm.numOutQue = 1;
    nsfPrm.inputFrameRate = 60;
    nsfPrm.outputFrameRate = 60;

    /*
     *  More than 6 output buffers are neede to realize 60 fps at encoder input
     */
    if(pCameraInstPrm->standard == SYSTEM_STD_1080P_60)
        nsfPrm.numBufsPerCh = 8;
    else
        nsfPrm.numBufsPerCh = 6;
    nsfPrm.outQueParams[0].nextLink = dupId[VNF_DUP_IDX];//gVsysModuleContext.muxIdVnf;

    nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;

    /* Channel enable/disable */
    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
    {
        nsfPrm.enable[0] = 0;
    }

    /* VNF link params */
    VnfLink_ChCreateParams *vnfParams;
    VnfLink_CreateParams_Init(&vnfPrm);
    vnfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;//;dupId[VNF_DUP_IDX];
    vnfPrm.inQueParams.prevLinkQueId = 0;
    vnfPrm.outQueParams.nextLink = dupId[VNF_DUP_IDX];//gVsysModuleContext.muxIdVnf;
    vnfParams = &vnfPrm.chCreateParams[0];
    vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
    if (gUI_mcfw_config.ldcEnable)
    {
        vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
    }
    else
    {
#ifdef USE_TNF2_FILTER
        vnfParams->sParams.eOperateMode =  VNF_LINK_3DNF;
#elif defined (USE_TNF3_FILTER)
        vnfParams->sParams.eOperateMode =  VNF_LINK_TNF3;
#else
        vnfParams->sParams.eOperateMode = VNF_LINK_AFFINE_NSF_TNF;
#endif
        vnfParams->sParams.eNsfSet = VNF_LINK_NSF_LUMA_CHROMA;
    }

    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON)
    {
        if (((vnfParams->sParams.eOperateMode == VNF_LINK_TNF3) && (vnfParams->sParams.eNsfSet == VNF_LINK_NSF_LUMA_CHROMA)) ||
            (vnfParams->sParams.eOperateMode == VNF_LINK_NSF2) || (vnfParams->sParams.eOperateMode == VNF_LINK_LDC_AFFINE_NSF_TNF))
        {
            cameraPrm.vnfFullResolution = TRUE;
        }
    }

    vnfParams->dParams.ldcParams.eInterpolationLuma = VNF_LINK_YINTERPOLATION_BILINEAR;
    vnfParams->dParams.ldcParams.unPixelPad = 4;

    vnfParams->dParams.nsfParams.bLumaEn = TRUE;
    vnfParams->dParams.nsfParams.bChromaEn = TRUE;
    vnfParams->dParams.nsfParams.eSmoothVal = VNF_LINK_SMOOTH_2;
    vnfParams->dParams.nsfParams.bSmoothLumaEn = TRUE;
    vnfParams->dParams.nsfParams.bSmoothChromaEn = FALSE;

    vnfParams->dParams.tnfParams.useDefault = TRUE;

    vnfParams->dParams.nsfFilter.eFilterParam = VNF_LINK_PARAM_DEFAULT;
    vnfParams->dParams.nsfEdge.eEdgeParam = VNF_LINK_PARAM_DISABLE;
    vnfParams->dParams.nsfShading.eShdParam = VNF_LINK_PARAM_DISABLE;
    vnfParams->dParams.nsfDesaturation.eDesatParam = VNF_LINK_PARAM_DISABLE;

    vnfPrm.chCreateParams[0].bSnfEnable = gUI_mcfw_config.snfEnable;
    vnfPrm.chCreateParams[0].bTnfEnable = gUI_mcfw_config.tnfEnable;
    vnfPrm.chCreateParams[0].strength   = gUI_mcfw_config.vnfStrength;

    /* Channel enable/disable */
    if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
    {
        vnfPrm.enable[0] = 0;
    }

#if (TILER_ENABLE == TRUE)
    vnfPrm.tilerEnable = TRUE;
#else
    vnfPrm.tilerEnable = FALSE;
#endif

	/* DUP link params */
	dupPrm[VNF_DUP_IDX].numOutQue = 2;
	if (gUI_mcfw_config.noisefilterMode == DSS_VNF_ON){
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkId    = gVcamModuleContext.nsfId;
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkQueId = 0;
    }
	else{
		dupPrm[VNF_DUP_IDX].inQueParams.prevLinkId    = gVcamModuleContext.vnfId;
        dupPrm[VNF_DUP_IDX].inQueParams.prevLinkQueId = 0;
	}
    dupPrm[VNF_DUP_IDX].numOutQue                 = 2;
    dupPrm[VNF_DUP_IDX].outQueParams[0].nextLink  = gVsysModuleContext.muxId;
    dupPrm[VNF_DUP_IDX].outQueParams[1].nextLink  = gVcamModuleContext.sclrId[0];
    dupPrm[VNF_DUP_IDX].notifyNextLink 			  = TRUE;	

    /* Scaler Link Params */
    SclrLink_CreateParams_Init(&sclrPrm);
    sclrPrm.inQueParams.prevLinkId             = dupId[VNF_DUP_IDX];
    sclrPrm.inQueParams.prevLinkQueId          = 1;
    sclrPrm.outQueParams.nextLink          	   = gVsysModuleContext.muxId;
    sclrPrm.tilerEnable                        = FALSE;
    sclrPrm.enableLineSkipSc                   = FALSE;
    sclrPrm.inputFrameRate                     = 30;
    sclrPrm.outputFrameRate                    = 30;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.scaleMode                          = DEI_SCALE_MODE_ABSOLUTE;
    sclrPrm.outScaleFactor.absoluteResolution.outWidth   = 720;
    sclrPrm.outScaleFactor.absoluteResolution.outHeight  = 480;
    sclrPrm.outDataFormat = VF_YUV420SP_UV;
    sclrPrm.pathId = SCLR_LINK_SEC0_SC3;  

	/**********************************************************************************************/
    /* IPC Frames Out VPSS for Vehicle link params */
    ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    ipcFramesOutVpssPrm.baseCreateParams.inQueParams.prevLinkQueId = 1;
    ipcFramesOutVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesOutVpssPrm.baseCreateParams.inputFrameRate = 30;
    ipcFramesOutVpssPrm.baseCreateParams.outputFrameRate = 30;
    ipcFramesOutVpssPrm.baseCreateParams.numOutQue = 1;
    ipcFramesOutVpssPrm.baseCreateParams.outQueParams[0].nextLink = ipcFramesInDspId;
    ipcFramesOutVpssPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesOutVpssPrm.baseCreateParams.processLink = SYSTEM_LINK_ID_INVALID;
    ipcFramesOutVpssPrm.baseCreateParams.notifyProcessLink = FALSE;
    ipcFramesOutVpssPrm.baseCreateParams.noNotifyMode = FALSE;

    /* IPC Frames in DSP for Vehicle link params */
    ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkId = ipcFramesOutVpssId;
    ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInDspPrm.baseCreateParams.numOutQue   = 1;
    ipcFramesInDspPrm.baseCreateParams.outQueParams[0].nextLink = vehicleId;
    ipcFramesInDspPrm.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesInDspPrm.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesInDspPrm.baseCreateParams.noNotifyMode   = FALSE;

    /* vehicle Link Params niqi_debug */
    AlgVehicleLink_CreateParams_Init(&vehiclePrm);
    vehiclePrm.inQueParams.prevLinkId = ipcFramesInDspId;
    vehiclePrm.inQueParams.prevLinkQueId = 0; 
	vehiclePrm.outQueParams.nextLink = ipcFramesOutDspId;
    vehiclePrm.thPlateIdCreateParams.inputFrameRate = 30;
    vehiclePrm.thPlateIdCreateParams.outputFrameRate = 30;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].cImageFormat = ImageFormatYUV420COMPASS;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].bVertFlip = 0;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].bDwordAligned = 0;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].dFormat = 2;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].nPlateLocate_Th = 5;
    vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].nOCR_Th = 2;
    strcpy(&vehiclePrm.thPlateIdCreateParams.chDefaultParams[0].szProvince[0], "‘¡∏”œÊ");    

	/* IPC Frames out DSP to vpss m3 link params */
	ipcFramesOutDspPrm.baseCreateParams.inQueParams.prevLinkId = vehicleId;
	ipcFramesOutDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
	ipcFramesOutDspPrm.baseCreateParams.notifyPrevLink = TRUE;
	ipcFramesOutDspPrm.baseCreateParams.inputFrameRate = 30;
	ipcFramesOutDspPrm.baseCreateParams.outputFrameRate = 30;
	ipcFramesOutDspPrm.baseCreateParams.numOutQue = 1;
	ipcFramesOutDspPrm.baseCreateParams.outQueParams[0].nextLink = ipcFramesInVpssId;
	ipcFramesOutDspPrm.baseCreateParams.notifyNextLink = TRUE;
	ipcFramesOutDspPrm.baseCreateParams.processLink = SYSTEM_LINK_ID_INVALID;
	ipcFramesOutDspPrm.baseCreateParams.notifyProcessLink = FALSE;
	ipcFramesOutDspPrm.baseCreateParams.noNotifyMode = FALSE;
	
	ipcFramesInVpssPrm.baseCreateParams.inQueParams.prevLinkId = ipcFramesOutDspId;
	ipcFramesInVpssPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
	ipcFramesInVpssPrm.baseCreateParams.numOutQue	= 1;
	ipcFramesInVpssPrm.baseCreateParams.outQueParams[0].nextLink = gVsysModuleContext.muxId;
	ipcFramesInVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
	ipcFramesInVpssPrm.baseCreateParams.notifyNextLink = TRUE;
	ipcFramesInVpssPrm.baseCreateParams.noNotifyMode   = FALSE;	
	/*********************************************/
	/***********************************************************************************************/

    /* MUX link params */
    muxPrm.numInQue = 3;
    muxPrm.inQueParams[0].prevLinkId    = dupId[VNF_DUP_IDX];
    muxPrm.inQueParams[0].prevLinkQueId = 0;

    muxPrm.inQueParams[1].prevLinkId    = gVcamModuleContext.sclrId[0];
    muxPrm.inQueParams[1].prevLinkQueId = 0;

	muxPrm.inQueParams[2].prevLinkId = ipcFramesInVpssId;
	muxPrm.inQueParams[2].prevLinkQueId = 0;	

    muxPrm.outQueParams.nextLink = gVsysModuleContext.swOsdId;

    muxPrm.muxNumOutChan = 3;
	muxPrm.outChMap[0].inQueId = 0;
	muxPrm.outChMap[0].inChNum = 0;

	muxPrm.outChMap[1].inQueId = 1;
	muxPrm.outChMap[1].inChNum = 0;

	muxPrm.outChMap[2].inQueId = 2;
	muxPrm.outChMap[2].inChNum = 0;

	/* SWOSD Link Params */
	swosdPrm.inQueParams.prevLinkId = gVsysModuleContext.muxId;
	swosdPrm.inQueParams.prevLinkQueId = 0;
	swosdPrm.outQueParams.nextLink = dupId[DIS_DUP_IDX];

	/* Display dup link params */
	dupPrm[DIS_DUP_IDX].inQueParams.prevLinkId = gVsysModuleContext.swOsdId;
	dupPrm[DIS_DUP_IDX].inQueParams.prevLinkQueId = 0;
	dupPrm[DIS_DUP_IDX].numOutQue = 1;
	dupPrm[DIS_DUP_IDX].outQueParams[0].nextLink = ipcOutVpssId;
	dupPrm[DIS_DUP_IDX].notifyNextLink = TRUE;	
  
    /* IPC Out VPSS link params */
    ipcOutVpssPrm.inQueParams.prevLinkId = dupId[DIS_DUP_IDX];
    ipcOutVpssPrm.inQueParams.prevLinkQueId = 0;
    ipcOutVpssPrm.numOutQue = 1;
    ipcOutVpssPrm.outQueParams[0].nextLink = ipcInVideoId;
    ipcOutVpssPrm.notifyNextLink = TRUE;
    ipcOutVpssPrm.notifyPrevLink = TRUE;
    ipcOutVpssPrm.noNotifyMode = FALSE;

    /* IPC In VIDEO params */
    ipcInVideoPrm.inQueParams.prevLinkId = ipcOutVpssId;
    ipcInVideoPrm.inQueParams.prevLinkQueId = 0;
    ipcInVideoPrm.numOutQue = 1;
    ipcInVideoPrm.outQueParams[0].nextLink = gVencModuleContext.encId;
    ipcInVideoPrm.notifyNextLink = TRUE;
    ipcInVideoPrm.notifyPrevLink = TRUE;
    ipcInVideoPrm.noNotifyMode = FALSE;

    /* Video Encoder Link params */
    MULTICH_INIT_STRUCT(EncLink_CreateParams, encPrm);
    {
        EncLink_ChCreateParams *pLinkChPrm;
        EncLink_ChDynamicParams *pLinkDynPrm;
        VENC_CHN_DYNAMIC_PARAM_S *pDynPrm;
        VENC_CHN_PARAMS_S *pChPrm;

        for (i = 0; i < VENC_PRIMARY_CHANNELS; i++)
        {
            pLinkChPrm  = &encPrm.chCreateParams[i];
            pLinkDynPrm = &pLinkChPrm->defaultDynamicParams;

            pChPrm      = &gVencModuleContext.vencConfig.encChannelParams[i];
            pDynPrm     = &pChPrm->dynamicParam;

            switch(gUI_mcfw_config.demoCfg.codec_combo) {
                case 0: pLinkChPrm->format = IVIDEO_H264HP; break; //"SINGLE_H264"
                case 1: pLinkChPrm->format = IVIDEO_MPEG4SP; break; //"SINGLE_MPEG4"
                case 2: pLinkChPrm->format = IVIDEO_MJPEG; break; //"SINGLE_JPEG"
                case 3: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_MJPEG; break; //"H264_JPEG"
                case 4: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MJPEG; break; //"MPEG4_JPEG"
                case 5: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_H264HP; break; //"DUAL_H264"
                case 6: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MPEG4SP; break; //"DUAL_MPEG4"
                case 7: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_MPEG4SP; break;  //"H264_MPEG4"
                case 8: pLinkChPrm->format = (i==0)? IVIDEO_H264HP:IVIDEO_H264HP; break;  //"TRIPLE_H264"
                case 9: pLinkChPrm->format = (i==0)? IVIDEO_MPEG4SP:IVIDEO_MPEG4SP; break; //"TRIPLE_MPEG4"
                default: pLinkChPrm->format = IVIDEO_H264HP;
            }

            pLinkChPrm->profile                 = gVencModuleContext.vencConfig.h264Profile[i];
            pLinkChPrm->dataLayout              = IVIDEO_PROGRESSIVE;
            pLinkChPrm->fieldMergeEncodeEnable  = FALSE;
            pLinkChPrm->enableAnalyticinfo      = pChPrm->enableAnalyticinfo;
            pLinkChPrm->maxBitRate              = pChPrm->maxBitRate;
            pLinkChPrm->encodingPreset          = pChPrm->encodingPreset;
            pLinkChPrm->rateControlPreset       = IVIDEO_USER_DEFINED; //pChPrm->rcType;
            pLinkChPrm->enableHighSpeed         = FALSE;
            pLinkChPrm->enableWaterMarking      = pChPrm->enableWaterMarking;
            pLinkChPrm->StreamPreset            = gUI_mcfw_config.StreamPreset[i];

            pLinkDynPrm->intraFrameInterval     = pDynPrm->intraFrameInterval;
            pLinkDynPrm->targetBitRate          = pDynPrm->targetBitRate;
            pLinkDynPrm->interFrameInterval     = 1;
            pLinkDynPrm->mvAccuracy             = IVIDENC2_MOTIONVECTOR_QUARTERPEL;
            pLinkDynPrm->inputFrameRate         = pDynPrm->inputFrameRate;
            pLinkDynPrm->rcAlg                  = pDynPrm->rcAlg;
            pLinkDynPrm->qpMin                  = pDynPrm->qpMin;
            pLinkDynPrm->qpMax                  = pDynPrm->qpMax;
            pLinkDynPrm->qpInit                 = pDynPrm->qpInit;
            pLinkDynPrm->vbrDuration            = pDynPrm->vbrDuration;
            pLinkDynPrm->vbrSensitivity         = pDynPrm->vbrSensitivity;

            encPrm.numBufPerCh[i] = 4;
            gVencModuleContext.encFormat[i] = pLinkChPrm->format;
        }
    }

    /* Video Encoder Framerate */
#ifdef IMGS_OMNIVISION_OV10630
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#elif defined IMGS_SONY_IMX104
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#elif defined IMGS_MICRON_MT9M034
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 30;  //
#else
    encPrm.chCreateParams[0].defaultDynamicParams.inputFrameRate = 60;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;
#endif
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;  //30// ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;

    for (i = VENC_PRIMARY_CHANNELS; i < (VENC_CHN_MAX - 1); i++)
    {
        encPrm.chCreateParams[i].format = IVIDEO_MJPEG;
        encPrm.chCreateParams[i].profile = 0;
        encPrm.chCreateParams[i].dataLayout = IVIDEO_PROGRESSIVE;
        encPrm.chCreateParams[i].fieldMergeEncodeEnable = FALSE;
        encPrm.chCreateParams[i].defaultDynamicParams.intraFrameInterval = 0;
        encPrm.chCreateParams[i].encodingPreset = 0;
        encPrm.chCreateParams[i].enableAnalyticinfo = 0;
        encPrm.chCreateParams[i].enableWaterMarking = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.inputFrameRate = 60;
        encPrm.chCreateParams[i].rateControlPreset = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.targetBitRate =
            100 * 1000;
        encPrm.chCreateParams[i].defaultDynamicParams.interFrameInterval = 0;
        encPrm.chCreateParams[i].defaultDynamicParams.mvAccuracy = 0;

        gVencModuleContext.encFormat[i] = encPrm.chCreateParams[i].format;
    }
    encPrm.inQueParams.prevLinkId = ipcInVideoId;
    encPrm.inQueParams.prevLinkQueId = 0;
    encPrm.outQueParams.nextLink = gVencModuleContext.ipcBitsOutRTOSId;

    /* IPC Bits Out VIDEO Link params */
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkId =
        gVencModuleContext.encId;
    ipcBitsOutVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsOutVideoPrm.baseCreateParams.numOutQue = 1;
    ipcBitsOutVideoPrm.baseCreateParams.outQueParams[0].nextLink =
        gVencModuleContext.ipcBitsInHLOSId;
    MultiCh_ipcBitsInitCreateParams_BitsOutRTOS(&ipcBitsOutVideoPrm, TRUE);

    /* IPC Bits In HOST Link params */
    ipcBitsInHostPrm0.baseCreateParams.inQueParams.prevLinkId =
        gVencModuleContext.ipcBitsOutRTOSId;
    ipcBitsInHostPrm0.baseCreateParams.inQueParams.prevLinkQueId = 0;
    MultiCh_ipcBitsInitCreateParams_BitsInHLOS(&ipcBitsInHostPrm0);

    /* Links Creation */

    /* communication Link */
    System_linkCreate(SYSTEM_HOST_LINK_ID_IPC_COMMUNICATION, &ipcCommunicationPrm, sizeof(ipcCommunicationPrm));

    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm, sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId, CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);

    gIsGlbceInitDone = 0;

	/* VNF NSF Link*/
    if(gUI_mcfw_config.noisefilterMode == DSS_VNF_ON){
        System_linkCreate(gVcamModuleContext.vnfId,&nsfPrm,sizeof(nsfPrm));
    }
	else{
    	System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
	}	

    /* Dup Link to 2 channels */
    System_linkCreate(dupId[VNF_DUP_IDX],&dupPrm[VNF_DUP_IDX],sizeof(dupPrm[VNF_DUP_IDX]));

    /* Scaler Link */
    System_linkCreate(gVcamModuleContext.sclrId[0], &sclrPrm, sizeof(sclrPrm));

    /* IPC Frames VPSS->DSP links */
    System_linkCreate(ipcFramesOutVpssId, &ipcFramesOutVpssPrm, sizeof(ipcFramesOutVpssPrm));
    System_linkCreate(ipcFramesInDspId, &ipcFramesInDspPrm, sizeof(ipcFramesInDspPrm));  

    /* vehicle Link*/
    System_linkCreate(vehicleId, &vehiclePrm, sizeof(vehiclePrm)); 

	/* IPC Frames DSP->VPSS links */
	System_linkCreate(ipcFramesOutDspId, &ipcFramesOutDspPrm, sizeof(ipcFramesOutDspPrm));
	System_linkCreate(ipcFramesInVpssId, &ipcFramesInVpssPrm, sizeof(ipcFramesInVpssPrm));	

    /* Mux 3 channel */
    System_linkCreate(gVsysModuleContext.muxId, &muxPrm, sizeof(muxPrm));

    /* SWOSD Link */
    System_linkCreate(gVsysModuleContext.swOsdId, &swosdPrm, sizeof(swosdPrm));
	
	/* Dup link */
	System_linkCreate(dupId[DIS_DUP_IDX], &dupPrm[DIS_DUP_IDX], sizeof(dupPrm[DIS_DUP_IDX]));

    /* IPC Links */
    System_linkCreate(ipcOutVpssId, &ipcOutVpssPrm, sizeof(ipcOutVpssPrm));
    System_linkCreate(ipcInVideoId, &ipcInVideoPrm, sizeof(ipcInVideoPrm));

    /* Video Encoder Link */
    System_linkCreate(gVencModuleContext.encId, &encPrm, sizeof(encPrm));

    /* IPC Bits Links */
    System_linkCreate(gVencModuleContext.ipcBitsOutRTOSId, &ipcBitsOutVideoPrm,
                      sizeof(ipcBitsOutVideoPrm));
    System_linkCreate(gVencModuleContext.ipcBitsInHLOSId, &ipcBitsInHostPrm0,
                      sizeof(ipcBitsInHostPrm0));

    gNoiseFilterMode = gUI_mcfw_config.noisefilterMode;

    OSA_printf("USECASE SETUP DONE\n");
}

/* ===================================================================
 *  @func     MultiCh_deleteTriStreamFullFtr
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
Void MultiCh_deleteTriStreamFullFtr()
{
    UInt32 mergeId[NUM_MERGE_LINK];
    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;
	UInt32 ipcFramesOutDspId;
	UInt32 ipcFramesInVpssId;	
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 vehicleId;

    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
	ipcFramesOutDspId = SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0;
	ipcFramesInVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;	

    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
	dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;

    vehicleId = SYSTEM_LINK_ID_ALGVEHICLE_0;

    /* Links Deletion in reverse order from sink to source */

    /* IPC Bits Links */
    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);

    /* Video Encoder Link */
    System_linkDelete(gVencModuleContext.encId);

    /* IPC Links */
    System_linkDelete(ipcOutVpssId);
    System_linkDelete(ipcInVideoId);

	/* DUP Links */
    System_linkDelete(dupId[DIS_DUP_IDX]);	

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);	

	/* mux Link */
    System_linkDelete(gVsysModuleContext.muxId);	

    /* IPC Frames VPSS<->DSP Links */
    System_linkDelete(ipcFramesOutDspId);
    System_linkDelete(ipcFramesInVpssId);  
    System_linkDelete(vehicleId);
    System_linkDelete(ipcFramesOutVpssId);
    System_linkDelete(ipcFramesInDspId);	

    /* Scaler Link */
    System_linkDelete(gVcamModuleContext.sclrId[0]);	

	/* DUP Links */
    System_linkDelete(dupId[VNF_DUP_IDX]);

	/* nsf vnf Links */
    if(gUI_mcfw_config.noisefilterMode == DSS_VNF_ON)
	{
    	System_linkDelete(gVcamModuleContext.nsfId);
    }
	else
	{
    	System_linkDelete(gVcamModuleContext.vnfId);
	}

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

	/* communication Link */
	System_linkDelete(SYSTEM_HOST_LINK_ID_IPC_COMMUNICATION);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_printf("USECASE TEARDOWN DONE\n");
}

