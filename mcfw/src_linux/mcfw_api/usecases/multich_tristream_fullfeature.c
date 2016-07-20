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
                            (1920x1080)      (720x480)
                            (420SP/422I)      (420SP)
                                |                |
                                |                |          (2)
   |--- MCTNF ON ------- DUP_VNF(VPSSM3)   DUP_SCLR(VPSSM3) ----- DISPLAY_SD(VPSSM3)
   |               |     (1)        |(0)    (0)|    |(1)
   |               | MCTNF OFF      |          |    |
   |               |                |(422I)    |    |
 IPC FRAMES  ISS_NF(VPSSME)   DSS_NF(VPSSM3)   |  SCLR(VPSSM3) --- VA ON --|
 OUT(VPSSM3)   |   |                |(420SP)   |    |(320x240)             |
   |           |   |                |          |    | VA OFF               |
   |           |  MUX_VNF(VPSSM3) --|          |    |                      |
 IPC FRAMES    |   |                           |  FD(VPSSM3)----|        DUP_VA
 IN(VIDEOM3)   |   |                           |                |       (0)| (1)
   |           |  DUP_MJPEG(VPSSM3)            |                |----------|---- VA ON VPSSM3 ---|
   |           |   |   |-----------------------|                 VA ON DSP |(1)                  |
 MCTNF(VIDEOM3)|   |   |                                                   |                     |
   |           |  MUX(VPSSM3)                                     IPC FRAMES OUT(VPSSM3)     VA(VPSSM3)
   |           |   |                                                       |
 IPC FRAMES    |   |                                                       |
 OUT(VIDEOM3)  |  SWOSD(VPSSM3)                                    IPC FRAMES IN(DSP)
   |           |   |                                                       |
   |           |   |                                                       |
 IPC FRAMES ---|  DUP_DIS(VPSSM3) ----- DISPLAY_HDMI(VPSSM3)             VA(DSP)
 IN(VPSSM3)        |(0)           (1)
                   |
             IPC OUT(VPSSM3)
                   |
                   |
             IPC IN(VIDEOM3)
                   |
                   |
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
#define NUM_DUP_LINK                    (5)
#define DIS_DUP_IDX                     (0)
#define MJPEG_DUP_LINK_IDX              (1)
#define SCALER_DUP_IDX                  (2)
#define VA_DUP_IDX                      (3)
#define VNF_DUP_IDX                     (4)

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
    IspLink_CreateParams  ispPrm;
    UInt32 glbceId;
    NsfLink_CreateParams nsfPrm;
    VnfLink_CreateParams vnfPrm;
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    MuxLink_CreateParams muxPrm;
#else
    MergeLink_CreateParams mergePrm[NUM_MERGE_LINK];
#endif
    IpcLink_CreateParams ipcOutVpssPrm;
    IpcLink_CreateParams ipcInVideoPrm;
    EncLink_CreateParams encPrm;
    DupLink_CreateParams dupPrm[NUM_DUP_LINK];
    SclrLink_CreateParams       sclrPrm;
    DisplayLink_CreateParams displayPrm;
    SwosdLink_CreateParams swosdPrm;
    IpcBitsOutLinkRTOS_CreateParams ipcBitsOutVideoPrm;
    IpcBitsInLinkHLOS_CreateParams ipcBitsInHostPrm0;
    VstabLink_CreateParams vstabPrm;
    GlbceSupportLink_CreateParams glbceSupportPrm;
    VaLink_CreateParams vaPrm;
	RvmLink_CreateParams rvmPrm;
    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutVpssPrm;
    IpcFramesInLinkRTOS_CreateParams ipcFramesInDspPrm;

	IpcFramesOutLinkRTOS_CreateParams ipcFramesOutDspPrm;
	IpcFramesOutLinkRTOS_CreateParams ipcFramesInVpssPrm;

    CameraLink_VipInstParams *pCameraInstPrm;
    CameraLink_OutParams *pCameraOutPrm;

#ifndef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    UInt32 mergeId[NUM_MERGE_LINK];
#endif

    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 vstabId;
    UInt32 glbceSupportId;
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;

	UInt32 ipcFramesOutDspId;
	UInt32 ipcFramesInVpssId;

    /* IPC struct init */
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams, ipcInVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkRTOS_CreateParams, ipcBitsOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkHLOS_CreateParams, ipcBitsInHostPrm0);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesOutVpssPrm);
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams, ipcFramesInDspPrm);

	MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesOutDspPrm);
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, ipcFramesInVpssPrm);

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    MULTICH_INIT_STRUCT(MuxLink_CreateParams, muxPrm);
#endif


    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        OSA_printf("\n********** SMART ANALYTICS USECASE ********\n");
        OSA_printf
            ("********* Entered Smart Analytics usecase - H264 1080p @60fps + H264 D1 @30fps + MJPEG 1080p @5fps ********\n\n");
    }
    else
    {
        OSA_printf("\n********** FULL FEATURE USECASE ********\n");
        OSA_printf
            ("********* Entered Tri Streaming usecase - H264 1080p @60fps + H264 D1 @30fps + MJPEG 1080p @5fps ********\n\n");
    }

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
#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    gVsysModuleContext.muxId = SYSTEM_VPSS_LINK_ID_MUX_0;
#else
    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
#endif
    dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[MJPEG_DUP_LINK_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[SCALER_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    dupId[VA_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_3;
    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_4;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    gVcamModuleContext.ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;

    gVdisModuleContext.displayId[VDIS_DEV_HDMI] = SYSTEM_LINK_ID_DISPLAY_0;
    gVdisModuleContext.displayId[VDIS_DEV_SD] = SYSTEM_LINK_ID_DISPLAY_2;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    gVencModuleContext.ipcBitsOutRTOSId = SYSTEM_VIDEO_LINK_ID_IPC_BITS_OUT_0;
    gVencModuleContext.ipcBitsInHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_IN_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;

	ipcFramesOutDspId = SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0;
	ipcFramesInVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;

#ifdef VA_ON_DSP
    gVsysModuleContext.vaId = SYSTEM_DSP_LINK_ID_VA;
#else
    gVsysModuleContext.vaId = SYSTEM_VPSS_LINK_ID_VA;
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
#ifndef USE_MCTNF
        /* When VA is running on VPSS M3 then if we run ISS NF which runs
           in nseries with VA will bring down system fps */
        gUI_mcfw_config.noisefilterMode = DSS_VNF_ON;
#endif
    }
#endif

#ifdef ENABLE_TILER_CAMERA
    /*
     * HDVPSS NSF doesn't support TILED input memory.
     * So if Camera TILER is enabled only ISS VNF is enabled.
     */
    gUI_mcfw_config.noisefilterMode = ISS_VNF_ON;
#endif

	/**********************************************************************************************/
	/***1*** Camera Link params */
    {
    gVcamModuleContext.glbceId = SYSTEM_LINK_ID_INVALID;

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
    cameraPrm.outQueParams[1].nextLink = ipcFramesOutVpssId;

    /* This is for TVP5158 Audio Channels - Change it to 16 if there are 16
     * audio channels connected in cascade */
    cameraPrm.numAudioChannels = 1;
    cameraPrm.numVipInst = 1;

#ifdef ENABLE_TILER_CAMERA
    cameraPrm.tilerEnable = TRUE;
#else
    cameraPrm.tilerEnable = FALSE;
#endif

    cameraPrm.vsEnable = gUI_mcfw_config.vsEnable;
    cameraPrm.vstabLinkId = vstabId;

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
    if(cameraPrm.vsEnable == 0)
    {
        pCameraOutPrm->scOutWidth = 1920;
        pCameraOutPrm->scOutHeight = 1080;
    }
    else
    {
        pCameraOutPrm->scOutWidth = VS_W;
        pCameraOutPrm->scOutHeight = VS_H;
    }

    pCameraOutPrm->outQueId = 0;

    /* Second stream */
    pCameraOutPrm = &pCameraInstPrm->outParams[1];
    pCameraOutPrm->dataFormat = SYSTEM_DF_YUV420SP_UV;
    pCameraOutPrm->scEnable = FALSE;
    pCameraOutPrm->scOutWidth = 1920;
    pCameraOutPrm->scOutHeight = 1080;
    pCameraOutPrm->standard = SYSTEM_STD_NTSC;
    pCameraOutPrm->outQueId = 1;

    /* 2A config */
    cameraPrm.t2aConfig.n2A_vendor = gUI_mcfw_config.n2A_vendor;
    cameraPrm.t2aConfig.n2A_mode = gUI_mcfw_config.n2A_mode;
    }

    /* vstab Link params */
    vstabPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
    vstabPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;

    glbceSupportPrm.glbceLinkID = glbceId;
    glbceSupportPrm.totalFrameWidth  = pCameraInstPrm->sensorOutWidth;
    glbceSupportPrm.totalFrameHeight = pCameraInstPrm->sensorOutHeight;

    ispPrm.vnfFullResolution = FALSE;
    cameraPrm.vnfFullResolution = FALSE;


	/**********************************************************************************************/
	/***2*** NSF link params */
    NsfLink_CreateParams_Init(&nsfPrm);
    if(gUI_mcfw_config.snfEnable){
        if (gUI_mcfw_config.tnfEnable)
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_NONE;
        else
            nsfPrm.bypassNsf = NSF_LINK_BYPASS_TNF;
    }
    else{
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

    nsfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
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
    nsfPrm.outQueParams[0].nextLink = dupId[VNF_DUP_IDX];

    nsfPrm.nsfStrength = gUI_mcfw_config.vnfStrength;
    /* Channel enable/disable */
    if (gUI_mcfw_config.noisefilterMode != DSS_VNF_ON){
        nsfPrm.enable[0] = 0;
    }

    /**********************************************************************************************/
	/***3*** VNF link params */
    VnfLink_ChCreateParams *vnfParams;
    VnfLink_CreateParams_Init(&vnfPrm);
    vnfPrm.inQueParams.prevLinkId = gVcamModuleContext.cameraId;
    vnfPrm.inQueParams.prevLinkQueId = 0;
    vnfPrm.outQueParams.nextLink = dupId[VNF_DUP_IDX];
    vnfParams = &vnfPrm.chCreateParams[0];
    vnfParams->sParams.eOutputFormat = SYSTEM_DF_YUV420SP_UV;
    if (gUI_mcfw_config.ldcEnable){
        vnfParams->sParams.eOperateMode = VNF_LINK_LDC_AFFINE_NSF_TNF;
    }
    else{
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
            {
                ispPrm.vnfFullResolution = FALSE;
                cameraPrm.vnfFullResolution = TRUE;
            }
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

	/**********************************************************************************************/
	/***4*** DUP link params */
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


	/**********************************************************************************************/
	/***5*** Scaler Link Params */
	SclrLink_CreateParams_Init(&sclrPrm);
	sclrPrm.inQueParams.prevLinkId			   = dupId[VNF_DUP_IDX];
	sclrPrm.inQueParams.prevLinkQueId		   = 1;
	sclrPrm.outQueParams.nextLink		   	   = gVsysModuleContext.muxId;
	sclrPrm.tilerEnable 					   = FALSE;
	sclrPrm.enableLineSkipSc				   = FALSE;
	sclrPrm.inputFrameRate					   = 30;
	sclrPrm.outputFrameRate 				   = 30;
	sclrPrm.scaleMode						   = DEI_SCALE_MODE_ABSOLUTE;
	sclrPrm.scaleMode						   = DEI_SCALE_MODE_ABSOLUTE;
	sclrPrm.outScaleFactor.absoluteResolution.outWidth	 = 720;
	sclrPrm.outScaleFactor.absoluteResolution.outHeight  = 480;
	sclrPrm.outDataFormat = VF_YUV420SP_UV;
	sclrPrm.pathId = SCLR_LINK_SEC0_SC3;


	/**********************************************************************************************/
	/***6*** Mux Link Params */
	muxPrm.numInQue = 3;
	muxPrm.inQueParams[0].prevLinkId = dupId[VNF_DUP_IDX];
	muxPrm.inQueParams[0].prevLinkQueId = 0;

	muxPrm.inQueParams[1].prevLinkId = gVcamModuleContext.sclrId[0];
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

	/**********************************************************************************************/
	/***7*** SWOSD Link Params */
    swosdPrm.inQueParams.prevLinkId 	= gVsysModuleContext.muxId;
    swosdPrm.inQueParams.prevLinkQueId 	= 0;
    swosdPrm.outQueParams.nextLink 		= dupId[DIS_DUP_IDX];

	/* Display dup link params */
	dupPrm[DIS_DUP_IDX].inQueParams.prevLinkId = gVsysModuleContext.swOsdId;
	dupPrm[DIS_DUP_IDX].inQueParams.prevLinkQueId = 0;
	dupPrm[DIS_DUP_IDX].numOutQue = 1;
	dupPrm[DIS_DUP_IDX].outQueParams[0].nextLink = ipcOutVpssId;
	//dupPrm[DIS_DUP_IDX].outQueParams[1].nextLink = gVdisModuleContext.displayId[VDIS_DEV_HDMI];
	dupPrm[DIS_DUP_IDX].notifyNextLink = TRUE;

	/* display link params */
	MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm);
	displayPrm.inQueParams[0].prevLinkId = dupId[DIS_DUP_IDX];
	displayPrm.inQueParams[0].prevLinkQueId = 1;
	displayPrm.displayRes = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
	displayPrm.displayId  = DISPLAY_LINK_DISPLAY_SC2;

	/**********************************************************************************************/
	/***8*** SWOSD Link Params */
    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        /* IPC Frames Out VPSS for VA link params */
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

        /* IPC Frames in DSP for VA link params */
        ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkId = ipcFramesOutVpssId;
        ipcFramesInDspPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
        ipcFramesInDspPrm.baseCreateParams.numOutQue   = 1;
        ipcFramesInDspPrm.baseCreateParams.outQueParams[0].nextLink = gVsysModuleContext.vaId;
        ipcFramesInDspPrm.baseCreateParams.notifyPrevLink = TRUE;
        ipcFramesInDspPrm.baseCreateParams.notifyNextLink = TRUE;
        ipcFramesInDspPrm.baseCreateParams.noNotifyMode   = FALSE;

        /* VA link params */
        VaLink_CreateParams_Init(&vaPrm);
        vaPrm.inQueParams.prevLinkId    = ipcFramesInDspId;
        vaPrm.inQueParams.prevLinkQueId = 0;
		vaPrm.outQueParams.nextLink     = ipcFramesOutDspId;
        vaPrm.encLinkId                 = gVencModuleContext.encId;
        vaPrm.swosdLinkId               = gVsysModuleContext.swOsdId;
        vaPrm.cameraLinkId              = gVcamModuleContext.cameraId;
        vaPrm.vaFrameRate               = 25;

		rvmPrm.inQueParams.prevLinkId = ipcFramesInDspId;
	    rvmPrm.inQueParams.prevLinkQueId = 0;
	    rvmPrm.outQueParams.nextLink = ipcFramesOutDspId;

		ipcFramesOutDspPrm.baseCreateParams.inQueParams.prevLinkId = gVsysModuleContext.vaId;
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
        ipcFramesInVpssPrm.baseCreateParams.numOutQue   = 1;
        ipcFramesInVpssPrm.baseCreateParams.outQueParams[0].nextLink = gVsysModuleContext.muxId;
        ipcFramesInVpssPrm.baseCreateParams.notifyPrevLink = TRUE;
        ipcFramesInVpssPrm.baseCreateParams.notifyNextLink = TRUE;
        ipcFramesInVpssPrm.baseCreateParams.noNotifyMode   = FALSE;
    }


    /* IPC Out VPSS link params */
    ipcOutVpssPrm.inQueParams.prevLinkId = dupId[DIS_DUP_IDX];//gVsysModuleContext.swOsdId;
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
    encPrm.chCreateParams[1].defaultDynamicParams.inputFrameRate = 30;  // ENC_LINK_DEFAULT_ALGPARAMS_INPUTFRAMERATE;

    encPrm.vsEnable = gUI_mcfw_config.vsEnable;

    if(gUI_mcfw_config.vaUseCase == TRUE)
    {
        encPrm.isVaUseCase = 1;
    }
    else
    {
        encPrm.isVaUseCase = 0;
    }

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
    /* Camera Link */
    System_linkCreate(gVcamModuleContext.cameraId, &cameraPrm,sizeof(cameraPrm));
    System_linkControl(gVcamModuleContext.cameraId, CAMERA_LINK_CMD_DETECT_VIDEO, NULL, 0, TRUE);
    {
        gIsGlbceInitDone = 0;
    }
	/* VNF NSF Link*/
    if(gUI_mcfw_config.noisefilterMode == DSS_VNF_ON){
        System_linkCreate(gVcamModuleContext.vnfId,&nsfPrm,sizeof(nsfPrm));
    }
	else{
    	System_linkCreate(gVcamModuleContext.vnfId,&vnfPrm,sizeof(vnfPrm));
	}
	/* Dup to 2 channels */
    System_linkCreate(dupId[VNF_DUP_IDX],&dupPrm[VNF_DUP_IDX],sizeof(dupPrm[VNF_DUP_IDX]));
	/* Scaler Link */
	System_linkCreate(gVcamModuleContext.sclrId[0], &sclrPrm, sizeof(sclrPrm));

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
#ifdef VA_ON_DSP
        /* IPC Frames links */
        System_linkCreate(ipcFramesOutVpssId, &ipcFramesOutVpssPrm, sizeof(ipcFramesOutVpssPrm));
        System_linkCreate(ipcFramesInDspId, &ipcFramesInDspPrm, sizeof(ipcFramesInDspPrm));
#endif
        /* VA link */
        System_linkCreate(gVsysModuleContext.vaId, &vaPrm, sizeof(vaPrm));
		System_linkCreate(ipcFramesOutDspId, &ipcFramesOutDspPrm, sizeof(ipcFramesOutDspPrm));
		System_linkCreate(ipcFramesInVpssId, &ipcFramesInVpssPrm, sizeof(ipcFramesInVpssPrm));
    }

	/* Mux 2 channel */
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

    /* VSTAB link */
    System_linkCreate(vstabId, &vstabPrm, sizeof(vstabPrm));

    /* IPC Bits Links */
    System_linkCreate(gVencModuleContext.ipcBitsOutRTOSId, &ipcBitsOutVideoPrm,
                      sizeof(ipcBitsOutVideoPrm));
    System_linkCreate(gVencModuleContext.ipcBitsInHLOSId, &ipcBitsInHostPrm0,
                      sizeof(ipcBitsInHostPrm0));


    /*
     *  Scaler Dup should formward the frame on outQue 0 (outQueId)
     *  only when outQue 1 (syncOutQueId) has returned the frame back.
     */
    Vsys_setDupDelayPrm(dupId[SCALER_DUP_IDX],
                        0,                      // outQueId - should be a valid out queue Id.
                        1);                     // syncOutQueId - can be valid queue Id or
                                                //              - an invlaid queueId(DUP_LINK_DELAYED_OUTPUT_QUE_INVALID_ID)
                                                //              - In the latter case it waits on all the other output queues.

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
    //UInt32 ispId;
    UInt32 glbceId;
    UInt32 mergeId[NUM_MERGE_LINK];
    UInt32 ipcOutVpssId;
    UInt32 ipcInVideoId;
    UInt32 ipcFramesOutVpssId;
    UInt32 ipcFramesInDspId;
    UInt32 dupId[NUM_DUP_LINK];
    UInt32 vstabId;
    UInt32 glbceSupportId;
#ifdef WDR_ON
    UInt32 wdrId;
#endif

#ifdef USE_MCTNF
    UInt32 ipcOutVpssIdForMCTNF;
    UInt32 ipcInVideoIdForMCTNF;
    UInt32 ipcOutVideoIdForMCTNF;
    UInt32 ipcInVpssIdForMCTNF;
#endif

    mergeId[CAM_STREAM_MERGE_IDX] = SYSTEM_VPSS_LINK_ID_MERGE_0;
    dupId[DIS_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_0;
    dupId[MJPEG_DUP_LINK_IDX] = SYSTEM_VPSS_LINK_ID_DUP_1;
    dupId[SCALER_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_2;
    dupId[VA_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_3;
    gVcamModuleContext.sclrId[0] = SYSTEM_LINK_ID_SCLR_INST_0;
    gVsysModuleContext.fdId = SYSTEM_LINK_ID_FD;
    gVsysModuleContext.swOsdId = SYSTEM_LINK_ID_SWOSD_0;
    ipcOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_0;
    ipcInVideoId = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_0;
    ipcFramesOutVpssId = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_OUT_0;
    ipcFramesInDspId   = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
    gVcamModuleContext.ispId = SYSTEM_LINK_ID_ISP_0;
    glbceId = SYSTEM_LINK_ID_GLBCE;
    vstabId = SYSTEM_LINK_ID_VSTAB_0;
    glbceSupportId = SYSTEM_LINK_ID_GLBCE_SUPPORT_0;

#ifdef WDR_ON
    wdrId = SYSTEM_LINK_ID_WDR;
#endif
#ifdef USE_MCTNF
    ipcOutVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_OUT_M3_1;
    ipcInVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_IN_M3_1;
    ipcOutVideoIdForMCTNF = SYSTEM_VIDEO_LINK_ID_IPC_OUT_M3_0;
    ipcInVpssIdForMCTNF = SYSTEM_VPSS_LINK_ID_IPC_IN_M3_0;
#endif

    dupId[VNF_DUP_IDX] = SYSTEM_VPSS_LINK_ID_DUP_4;
    gVsysModuleContext.muxIdVnf = SYSTEM_VPSS_LINK_ID_MUX_1;

    /* Links Deletion in reverse order from sink to source */

    /* display link */
    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_HDMI]);

    System_linkDelete(gVdisModuleContext.displayId[VDIS_DEV_SD]);

    /* IPC Bits Links */
    System_linkDelete(gVencModuleContext.ipcBitsOutRTOSId);
    System_linkDelete(gVencModuleContext.ipcBitsInHLOSId);

    /* VSTAB Link */
    System_linkDelete(vstabId);

    /* Video Encoder Link */
    System_linkDelete(gVencModuleContext.encId);

    /* IPC Links */
    System_linkDelete(ipcOutVpssId);
    System_linkDelete(ipcInVideoId);

    /* FD Link */
    System_linkDelete(gVsysModuleContext.fdId);

    if(Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
        /* VA link */
        System_linkDelete(gVsysModuleContext.vaId);

#ifdef VA_ON_DSP
        /* IPC Frames Links */
        System_linkDelete(ipcFramesOutVpssId);
        System_linkDelete(ipcFramesInDspId);
#endif

        /* Dup for VA link */
        System_linkDelete(dupId[VA_DUP_IDX]);
    }

    /* Scaler Link */
    System_linkDelete(gVcamModuleContext.sclrId[0]);

    /* dup link for display */
    System_linkDelete(dupId[DIS_DUP_IDX]);

    /* SWOSD Link */
    System_linkDelete(gVsysModuleContext.swOsdId);

#ifdef INCLUDE_MUX_LINK_INSTEAD_OF_MERGE
    System_linkDelete(gVsysModuleContext.muxId);
#else
    /* Merge Link */
    System_linkDelete(mergeId[CAM_STREAM_MERGE_IDX]);
#endif

    /* Dup link for Scaler */
    System_linkDelete(dupId[SCALER_DUP_IDX]);

    /* Dup Link */
    System_linkDelete(dupId[MJPEG_DUP_LINK_IDX]);

#ifdef MEMORY_256MB
    if(gNoiseFilterMode == DSS_VNF_ON)
        System_linkDelete(gVcamModuleContext.nsfId);
#else
    System_linkDelete(gVsysModuleContext.muxIdVnf);
    System_linkDelete(gVcamModuleContext.nsfId);
#endif

#ifdef USE_MCTNF
    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifndef VNF_BEFORE_MCTNF
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
    }

    System_linkDelete(ipcInVpssIdForMCTNF);
    System_linkDelete(ipcOutVideoIdForMCTNF);
    System_linkDelete(gVcamModuleContext.mctnfId);
    System_linkDelete(ipcInVideoIdForMCTNF);
    System_linkDelete(ipcOutVpssIdForMCTNF);

    if(Vsys_getSystemUseCase() != VSYS_USECASE_TRISTREAM_SMARTANALYTICS)
    {
    #ifdef VNF_BEFORE_MCTNF
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
    }
#else
    #ifdef MEMORY_256MB
        if(gNoiseFilterMode == ISS_VNF_ON)
            System_linkDelete(gVcamModuleContext.vnfId);
    #else
        System_linkDelete(gVcamModuleContext.vnfId);
    #endif
#endif
    System_linkDelete(dupId[VNF_DUP_IDX]);

    if(gIsGlbceInitDone)
    {
#ifdef YUV_M2M_DRV_TEST
#else
#ifndef WDR_ON
        System_linkDelete(glbceId);
        System_linkDelete(glbceSupportId);
#else
        System_linkDelete(wdrId);
#endif
#endif
        System_linkDelete(gVcamModuleContext.ispId);
        gIsGlbceInitDone = 0;
    }

    /* Camera Link */
    System_linkDelete(gVcamModuleContext.cameraId);

    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    OSA_printf("USECASE TEARDOWN DONE\n");
}

