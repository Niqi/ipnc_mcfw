/** ==================================================================
 *  @file   swosdLink_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/swosd/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "swosdLink_priv.h"
#include "osd.h"
#pragma DATA_ALIGN(gSwosdLink_tskStack, 32)
#pragma DATA_SECTION(gSwosdLink_tskStack, ".bss:taskStackSection")
UInt8 gSwosdLink_tskStack[SWOSD_LINK_OBJ_MAX][SWOSD_LINK_TSK_STACK_SIZE];

SwosdLink_Obj gSwosdLink_obj[SWOSD_LINK_OBJ_MAX];

extern Int32 isTiled[];
extern Int32 lineOffsets[];

int isIMXbasedSWOSDtoBeCalled = 0;
int isTransparencyEnabled[3] = { 0, 0, 0};
extern Semaphore_Handle cpisSync;
extern int isCPIS_GLBCE_deleted;


TOsd_Global_Info g_osd_total_info;

void SwosdLink_EmptyGlobalInfo(void)
{
	g_osd_total_info.osdType = OSD_INVAL_TYPE;
	g_osd_total_info.recvFinishFlag = 0;
	g_osd_total_info.bmpTimeInfo = NULL;
	g_osd_total_info.auxBmpTimeInfo = NULL;
	g_osd_total_info.thirdChannelBmpInfo = NULL;
	g_osd_total_info.osdInfo = NULL;
	g_osd_total_info.auxOsdInfo = NULL;

	g_osd_total_info.timeInfo[0] = 0;
	g_osd_total_info.timeInfo[1] = 0;
	g_osd_total_info.timeInfo[2] = 0;
	g_osd_total_info.timeInfo[3] = 0;
	g_osd_total_info.timeInfo[4] = 0;
	g_osd_total_info.timeInfo[5] = 0;
	g_osd_total_info.timeInfo[6] = 0;
}

void SwosdLink_FreeGlobalInfo(void)
{
	if (NULL != g_osd_total_info.thirdChannelBmpInfo)
	{
		free(g_osd_total_info.thirdChannelBmpInfo);
	}
	if (NULL != g_osd_total_info.auxBmpTimeInfo)
	{
		free(g_osd_total_info.auxBmpTimeInfo);
	}

	if (NULL != g_osd_total_info.bmpTimeInfo)
	{
		free(g_osd_total_info.bmpTimeInfo);
	}

	if (NULL != g_osd_total_info.osdInfo)
	{
		free(g_osd_total_info.osdInfo);
	}

	if (NULL != g_osd_total_info.auxOsdInfo)
	{
		free(g_osd_total_info.auxOsdInfo);
	}
}



void SwosdLink_EmptyOSDStruct(TSWOSD_Char *osd_info)
{
	osd_info->rows		  = 0;
	osd_info->width		  = 0;
	osd_info->pitch		  = 0;
	osd_info->pixel_mode  = 0;
	osd_info->bitmap_left = 0;
	osd_info->bitmap_top  = 0;
	osd_info->advance_x	  = 0;
	osd_info->advance_y	  = 0;
	osd_info->text_len	  = 0;
	osd_info->len		  = 0;
	osd_info->textPosX	  = 0;
	osd_info->textPosY	  = 0;
	osd_info->flag		  = 0;
	osd_info->ackNum 	  = 0;
	osd_info->bmp_num	  = 0;
	osd_info->fontsize	  = 0;

	memset(osd_info->buffer, 0, 45 * 45);
}
int SwosdLink_GetRecvFinishFlag(void)
{
    return g_osd_total_info.recvFinishFlag;
}

void SwosdLink_SetRecvFinishFlag(int flag)
{
    g_osd_total_info.recvFinishFlag = flag;
}

TSWOSD_Char *SwosdLink_GetBmpTimeInfo(void)
{
    return g_osd_total_info.bmpTimeInfo;
}
void SwosdLink_SetBmpTimeInfo(TSWOSD_Char * bmpTimeInfo)
{
    g_osd_total_info.bmpTimeInfo = bmpTimeInfo;
}
TSWOSD_Char *SwosdLink_GetAUXBmpTimeInfo(void)
{
    return g_osd_total_info.auxBmpTimeInfo;
}
void SwosdLink_SetAUXBmpTimeInfo(TSWOSD_Char * auxBmpTimeInfo)
{
    g_osd_total_info.auxBmpTimeInfo = auxBmpTimeInfo;
}
TSWOSD_Char *SwosdLink_GetThirdChannelBmpInfo(void)
{
    return g_osd_total_info.thirdChannelBmpInfo;
}
void SwosdLink_SetThirdChannelBmpInfo(TSWOSD_Char * thridChannelBmpInfo)
{
    g_osd_total_info.thirdChannelBmpInfo = thridChannelBmpInfo;
}

TSWOSD_Char *SwosdLink_GetOsdInfo(void)
{
    return g_osd_total_info.osdInfo;
}

void SwosdLink_SetOsdInfo(TSWOSD_Char * osdInfo)
{
    g_osd_total_info.osdInfo = osdInfo;
}

TSWOSD_Char *SwosdLink_GetAuxOsdInfo(void)
{
    return g_osd_total_info.auxOsdInfo;
}

void SwosdLink_SetAuxOsdInfo(TSWOSD_Char * auxOsdInfo)
{
	g_osd_total_info.auxOsdInfo = auxOsdInfo;
}

unsigned int *SwosdLink_GetTimeInfo(int *num)
{
	*num = 7;
    return &(g_osd_total_info.timeInfo[0]);
}

void SwosdLink_SetTimeInfoByCmd(TSWOSD_Char *save_time_info, UInt32 cmd)
{
	switch (cmd)
	{
		case SWOSDLINK_SETBMP_DATETIME:
			SwosdLink_SetBmpTimeInfo(save_time_info);
			break;
		case SWOSDLINK_SETBMP_AUX_DATETIME:
			SwosdLink_SetAUXBmpTimeInfo(save_time_info);
			break;
		case SWOSDLINK_SETBMP_LPRINFO:
			SwosdLink_SetThirdChannelBmpInfo(save_time_info);
			break;
		default:
			break;
	}
}

TSWOSD_Char *SwosdLink_GetTimeInfoByCmd(UInt32 cmd)
{
	TSWOSD_Char *save_time_info = NULL;

	switch (cmd)
	{
		case SWOSDLINK_SETBMP_DATETIME:
			save_time_info = SwosdLink_GetBmpTimeInfo();
			break;
		case SWOSDLINK_SETBMP_AUX_DATETIME:
			save_time_info = SwosdLink_GetAUXBmpTimeInfo();
			break;
		case SWOSDLINK_SETBMP_LPRINFO:
			save_time_info = SwosdLink_GetThirdChannelBmpInfo();
			break;
		default:
			Vps_printf("SwosdLink_SaveTimeInfo cmd type error!! cmd: %d\n", cmd);
			return NULL;
	}

	return save_time_info;
}

void SwosdLink_CopyOsdStruct(TSWOSD_Char *dst_Osd, TSWOSD_Char *src_Osd)
{
	int aux_bufsiz = 0;

	dst_Osd->rows		 = src_Osd->rows;
	dst_Osd->width		 = src_Osd->width;
	dst_Osd->pitch		 = src_Osd->pitch;
	dst_Osd->pixel_mode  = src_Osd->pixel_mode;
	dst_Osd->bitmap_left = src_Osd->bitmap_left;
	dst_Osd->bitmap_top  = src_Osd->bitmap_top;
	dst_Osd->advance_x	 = src_Osd->advance_x;
	dst_Osd->advance_y	 = src_Osd->advance_y;
	dst_Osd->text_len	 = src_Osd->text_len;
	dst_Osd->len		 = src_Osd->len;
	dst_Osd->bmp_num	 = src_Osd->bmp_num;
	dst_Osd->flag		 = src_Osd->flag;
	dst_Osd->ackNum 	 = src_Osd->ackNum;
	dst_Osd->textPosX	 = src_Osd->textPosX;
	dst_Osd->textPosY 	 = src_Osd->textPosY;
	dst_Osd->fontsize	 = src_Osd->fontsize;

	aux_bufsiz = src_Osd->rows * src_Osd->pitch;
	memcpy(dst_Osd->buffer, src_Osd->buffer, aux_bufsiz);
}

void SwosdLink_CopyOsdInfoMem(TSWOSD_Char *dst_Osd, TSWOSD_Char *src_Osd, int size)
{
	int loop = 0;
	TSWOSD_Char *dst_ptr = (TSWOSD_Char *)dst_Osd;
	TSWOSD_Char *src_ptr = (TSWOSD_Char *)src_Osd;

	while (loop < size)
	{
		SwosdLink_CopyOsdStruct(dst_ptr, src_ptr);

		dst_ptr++;
		src_ptr++;
		loop++;
	}
}

Int32 SwosdLink_SaveTimeInfo(Utils_MsgHndl *pMsg, UInt32 cmd)
{
	TSWOSD_Char *msg_time_info = NULL;
	TSWOSD_Char *save_time_info = NULL;
	UInt32 osdBmpNum = 0;
	msg_time_info = (TSWOSD_Char *)Utils_msgGetPrm(pMsg);
	if(NULL == msg_time_info)
	{
		Vps_printf("SwosdLink_SaveTimeInfo ERROR: msg_time_info is NULL\n");
		return FVID2_EALLOC;
	}

	osdBmpNum = msg_time_info->len;
	save_time_info = SwosdLink_GetTimeInfoByCmd(cmd);
	if ((NULL != save_time_info) && (osdBmpNum != save_time_info->len))
	{
		free(save_time_info);
		SwosdLink_SetTimeInfoByCmd(NULL, cmd);

		save_time_info = (TSWOSD_Char *)calloc(osdBmpNum, sizeof(TSWOSD_Char));
	}
	else if (NULL == save_time_info)
	{
		save_time_info = (TSWOSD_Char *)calloc(osdBmpNum, sizeof(TSWOSD_Char));
	}
	else
	{
		SwosdLink_EmptyOSDStruct(save_time_info);
	}

	if (NULL == save_time_info)
	{
		Vps_printf("SwosdLink_SaveTimeInfo ERROR: calloc save_time_info fail!\n");
		return FVID2_EFAIL;
	}
	SwosdLink_SetTimeInfoByCmd(save_time_info, cmd);

	/* 一次性接收BMP并保存到内存中 */
	SwosdLink_CopyOsdInfoMem(save_time_info, msg_time_info, osdBmpNum);
	return FVID2_SOK;
}

void SwosdLink_auxSetOsdTime(FVID2_Frame *pFrame,int Width, int Height)
{
	TSWOSD_Char *displayInfo = NULL;
	int num = 0;
	if (0 == pFrame->channelNum){
	    displayInfo = SwosdLink_GetBmpTimeInfo();
	}
	else if (1 == pFrame->channelNum){
	    displayInfo = SwosdLink_GetAUXBmpTimeInfo();
	}
	else if (2 == pFrame->channelNum){
		displayInfo = SwosdLink_GetThirdChannelBmpInfo();
	}
	if ((NULL != displayInfo) && (1 == displayInfo->flag))
	{
		if(2 != pFrame->channelNum){
			if (22 > displayInfo->len){
				Vps_printf("SwosdLink_auxSetOsdTime: ERROR!! Time info length error, len: %d.!!!\n", displayInfo->len);
				return;
			}
			if (0 == pFrame->channelNum){
				displayInfo->textPosX = 0;
				displayInfo->textPosY = 0;
				Width = 1952;
				OSD_time_show(pFrame->channelNum, SwosdLink_GetTimeInfo(&num), displayInfo, displayInfo->textPosX,
					displayInfo->textPosY, (unsigned char *)pFrame->addr[0][0],Width,Height,displayInfo->fontsize);
			}
			else if (1 == pFrame->channelNum){
				displayInfo->textPosX = 0;
				displayInfo->textPosY = 0;
				Width = 736;
				OSD_time_show(pFrame->channelNum, SwosdLink_GetTimeInfo(&num), displayInfo, displayInfo->textPosX,
					displayInfo->textPosY, (unsigned char *)pFrame->addr[0][0],Width,Height,displayInfo->fontsize);
			}
		}
		else {
			displayInfo->textPosX = 0;
			displayInfo->textPosY = 0;
			Width = 1920;
			OSD_textShow(displayInfo->textPosX,displayInfo->textPosY, displayInfo,
				(unsigned char *)pFrame->addr[0][0],Width,Height);
		}
	}
}


Int32 Swosd_drvPrintRtStatus(SwosdLink_Obj * pObj)
{

    Vps_rprintf(" %d: SWOSD:  Latency (Min / Max) = ( %d / %d ) !!! \r\n",
         Utils_getCurTimeInMsec(),
        pObj->minLatency,
        pObj->maxLatency
        );

    /* reset max time */

    pObj->maxLatency = 0;
    pObj->minLatency = 0xFF;

    return 0;
}

/* Swosd link create */

/* ===================================================================
 *  @func     SwosdLink_create
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
Int32 SwosdLink_create(SwosdLink_Obj * pObj, SwosdLink_CreateParams * pPrm)
{
    Int32 status;

    UInt32 strId,j,swOsdEnable;

    UInt32 chId;

    SWOSD_config_t swOsdPrm;

    System_LinkChInfo *pChInfo;

    System_LinkQueInfo *pOutQueInfo;

    System_LinkChInfo *pInChInfo, *pOutChInfo;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get the link info from prev link */
    status = System_linkGetInfo(pPrm->inQueParams.prevLinkId, &pObj->inTskInfo);

    UTILS_assert(status == FVID2_SOK);
    UTILS_assert(pPrm->inQueParams.prevLinkQueId < pObj->inTskInfo.numQue);

    /* copy the previous link info */
    memcpy(&pObj->inQueInfo,
           &pObj->inTskInfo.queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Create the output buffers */
    status = Utils_bufCreate(&pObj->outFrameQue,           // pHndl
                             FALSE,                        // blockOnGet
                             FALSE);                       // blockOnPut

    UTILS_assert(status == FVID2_SOK);

    isIMXbasedSWOSDtoBeCalled = 0;
    /* SWOSD alg create */
    memset(&swOsdPrm, 0x00, sizeof(SWOSD_config_t));

    /* NOTE:Assuming numCh is same as no of streams */
    swOsdPrm.numCaptureStream = pObj->inQueInfo.numCh;
    swOsdPrm.autoFocusVal = 0;

    if ((pObj->inQueInfo.chInfo[0].dataFormat == FVID2_DF_YUV420SP_VU) ||
        (pObj->inQueInfo.chInfo[0].dataFormat == FVID2_DF_YUV420SP_UV))
    {
        swOsdPrm.captureYuvFormat = SWOSD_FORMATYUV420p;
    }
    else
    {
        swOsdPrm.captureYuvFormat = SWOSD_FORMATYUV422i;
    }

    for (strId = 0; strId < swOsdPrm.numCaptureStream; strId++)
    {
        pChInfo = &pObj->inQueInfo.chInfo[strId];

        swOsdEnable = 1;

        /*
           SWOSD should be applied on the no of streams generated and not on the
           no of stream encoded
        */
        for(j = 0;j < strId;j ++)
        {
		    if((swOsdPrm.swosdConfig[j].cropWidth == pChInfo->width) &&
		       (swOsdPrm.swosdConfig[j].cropHeight == pChInfo->height))
		    {
			    swOsdEnable = 0;
			}
		}

        swOsdPrm.swosdConfig[strId].swosdEnable = swOsdEnable;
        swOsdPrm.swosdConfig[strId].osd_count = 0;
        swOsdPrm.swosdConfig[strId].capLineOffsetY = pChInfo->pitch[0];
		swOsdPrm.swosdConfig[strId].capLineOffsetUV = pChInfo->pitch[1];
        swOsdPrm.swosdConfig[strId].capWidth = pChInfo->width;
        swOsdPrm.swosdConfig[strId].capHeight = pChInfo->height;
        swOsdPrm.swosdConfig[strId].cropWidth = pChInfo->width;
        swOsdPrm.swosdConfig[strId].cropHeight = pChInfo->height;
        swOsdPrm.swosdConfig[strId].codecType = ALG_VID_CODEC_NONE;
        swOsdPrm.swosdConfig[strId].rateControlType =
            ALG_VID_ENC_RATE_CONTROL_VBR;
        swOsdPrm.swosdConfig[strId].frameRateBase = 0;
        swOsdPrm.swosdConfig[strId].codecBitrate = 0;
        swOsdPrm.swosdConfig[strId].swosdType = SWOSD_BASIC;
        swOsdPrm.swosdConfig[strId].swosdTextPos = SWOSD_FMT_TOP_RIGHT;
        swOsdPrm.swosdConfig[strId].swosdLogoPos = SWOSD_FMT_TOP_LEFT;
        swOsdPrm.swosdConfig[strId].swosdDatePos = SWOSD_FMT_BOTTOM_LEFT;
        swOsdPrm.swosdConfig[strId].swosdTimePos = SWOSD_FMT_BOTTOM_RIGHT;
        swOsdPrm.swosdConfig[strId].swosdTimeFormat = SWOSD_FMT_24HR;
        swOsdPrm.swosdConfig[strId].swosdDateFormat = SWOSD_FMT_DMY;
        swOsdPrm.swosdConfig[strId].swosdDateEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTransparencyEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTimeEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdLogoEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdTextEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdEvalEnable = OSD_NO;
        swOsdPrm.swosdConfig[strId].swosdLogoStringUpdate = OSD_YES;
        swOsdPrm.swosdConfig[strId].swosdDateTimeUpdate = OSD_YES;

		isTiled[strId] = FALSE;
		
		if(pChInfo->memType == SYSTEM_MT_TILEDMEM)
            isTiled[strId] = TRUE;
			
        swOsdPrm.swosdConfig[strId].memType = isTiled[strId];
		lineOffsets[strId] = pChInfo->pitch[0];
	    UTILS_assert(SWOSD_imxCreate(strId, isTiled[strId]/*isTiled*/,lineOffsets[strId]/*Video Buffer Pitch*/) == 0); 
    }

    status = DM81XX_SWOSD_create(&swOsdPrm);
    UTILS_assert(status == SWOSD_SOK);

    /* Populate the link's info structure */
    pObj->info.numQue = 1;

    pOutQueInfo = &pObj->info.queInfo[0];
    pObj->info.queInfo[0].numCh = pObj->inQueInfo.numCh;

    for (chId = 0; chId < pObj->info.queInfo[0].numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];
        pOutChInfo = &pOutQueInfo->chInfo[chId];

        memcpy(pOutChInfo, pInChInfo, sizeof(System_LinkChInfo));
    }
	
    pObj->minLatency = 0xFF;
    pObj->maxLatency = 0;

	/* 初始化全局变量 */
	SwosdLink_EmptyGlobalInfo();

#ifdef SYSTEM_DEBUG_SWOSD
    Vps_printf(" %d: SWOSD   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* Swosd process frames */

/* ===================================================================
 *  @func     SwosdLink_processFrames
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
Int32 SwosdLink_processFrames(SwosdLink_Obj * pObj)
{
    Int32 status = FVID2_SOK;

    UInt32 frameId ,latency;

	Int32 Width = 0,Height = 0,Pitch0 =0,Pitch1 = 0;

    FVID2_FrameList frameList;

    FVID2_Frame *pFullFrame;

    System_LinkInQueParams *pInQueParams;

    System_LinkQueInfo *pInQueInfo;

    System_LinkChInfo *pChInfo;

	System_FrameInfo *pInFrameInfo;

    Bool rtParamUpdatePerFrame = FALSE;
	
	UInt32 histData;
	
	Ptr fullFrameAddrY,fullFrameAddrUV;
	
	Ptr swosdApplyAddrY,swosdApplyAddrUV;

    pInQueParams = &pObj->createArgs.inQueParams;
    pInQueInfo = &pObj->inTskInfo.queInfo[pInQueParams->prevLinkQueId];

    System_getLinksFullFrames(pInQueParams->prevLinkId,
                              pInQueParams->prevLinkQueId, &frameList);

    if (frameList.numFrames)
    {
        /* Apply SWOSD on the full frames */
        for (frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFullFrame = frameList.frames[frameId];

            pChInfo 	 = &pInQueInfo->chInfo[pFullFrame->channelNum];
			pInFrameInfo = (System_FrameInfo *) pFullFrame->appData;

			if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == TRUE))
            {				
				DM81XX_SWOSD_getResolution(pFullFrame->channelNum,&Pitch0,&Pitch1,&Width,&Height);
				
				if (pInFrameInfo->rtChInfo.height != Height)
                {
                    Height = pInFrameInfo->rtChInfo.height;
					rtParamUpdatePerFrame = TRUE;					
                }

                if (pInFrameInfo->rtChInfo.width != Width)
                {
                    Width = pInFrameInfo->rtChInfo.width;
					rtParamUpdatePerFrame = TRUE;
                }

                if (pInFrameInfo->rtChInfo.pitch[0] != Pitch0)
                {
                    Pitch0 = pInFrameInfo->rtChInfo.pitch[0];
					rtParamUpdatePerFrame = TRUE;
                }

                if (pInFrameInfo->rtChInfo.pitch[1] != Pitch1)
                {
                    Pitch1 = pInFrameInfo->rtChInfo.pitch[1];
					rtParamUpdatePerFrame = TRUE;
                }
				
				if(rtParamUpdatePerFrame == TRUE)
				{
					DM81XX_SWOSD_Reset(pFullFrame->channelNum,Pitch0,Pitch1,Width,Height);
					SWOSD_imxUpdateLineOffset(pFullFrame->channelNum, Pitch0);
				}	
            }
			if ((pInFrameInfo != NULL) && (pInFrameInfo->rtChInfoUpdate == FALSE))
            {				
				Height = pChInfo->height;
				Width  = pChInfo->width;
				Pitch0 = pChInfo->pitch[0];
				Pitch1 = pChInfo->pitch[1];
			}
			
			swosdApplyAddrY  = fullFrameAddrY  = (Ptr)((UInt32)pFullFrame->addr[0][0] + 	
													   (pChInfo->startX + (pChInfo->startY * pChInfo->pitch[0])));
			swosdApplyAddrUV = fullFrameAddrUV = (Ptr)((UInt32)pFullFrame->addr[0][1] + 
			                                           (pChInfo->startX + (pChInfo->startY/2 * pChInfo->pitch[1])));
						
			if(pChInfo->memType == SYSTEM_MT_TILEDMEM)
			{
				fullFrameAddrY  = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)fullFrameAddrY);
				fullFrameAddrUV = (Ptr)Utils_tilerAddr2CpuAddr((UInt32)fullFrameAddrUV);
				
				if(DM81XX_SWOSD_isTransparencyEnable(pFullFrame->channelNum) == 0)
				{
					/*
					 * If transperency is OFF then it is going to be EDMA cpy based SWOSD apply.
					 * For EDMA we need to supply Tiler CPU address.
                     */					 
					swosdApplyAddrY  = fullFrameAddrY;
					swosdApplyAddrUV = fullFrameAddrUV;
				}
			}
			
            /* Draw the faces for FD */
            status = DM812X_FD_Draw(pFullFrame->channelNum,
                                    fullFrameAddrY, 	
                                    fullFrameAddrUV,	
                                    Width,
                                    Height,
                                    pChInfo->dataFormat,
                                    Pitch0, pChInfo->codingformat);

            UTILS_assert(status == FVID2_SOK);

			DM81XX_SWOSD_apply(pFullFrame->channelNum,     	// stream Id
							   (Int32)swosdApplyAddrY,	  	// Y plane address
							   (Int32)swosdApplyAddrUV); 	// UV plane address
			
			/* Draw the histogram */						
			if(pFullFrame->channelNum == 0)
			{
                CameraLink_getHistData(&histData);
							
				DM81XX_SWOSD_drawHistogram(pFullFrame->channelNum,     			// stream Id
										   (Int32) fullFrameAddrY,	  			// Y plane
										   (Int32) fullFrameAddrUV, 			// UV plane
										   (Int32*) histData);
            }

			/* show current time */
			SwosdLink_auxSetOsdTime(pFullFrame,Width, Height);

			latency = Utils_getCurTimeInMsec() - pFullFrame->timeStamp;
			if(latency>pObj->maxLatency)
				pObj->maxLatency = latency;
			if(latency<pObj->minLatency)
				pObj->minLatency = latency;
				
            /* Put the full buffer into full queue */
            status = Utils_bufPutFullFrame(&pObj->outFrameQue, pFullFrame);

            UTILS_assert(status == FVID2_SOK);
        }

        /* send SYSTEM_CMD_NEW_DATA to next link */
        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink,
                           SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

/* Swosd link drv delete */

/* ===================================================================
 *  @func     SwosdLink_delete
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
Int32 SwosdLink_delete(SwosdLink_Obj * pObj)
{
    Int32 status;	
	Int32 i = 0;


    /* Delete the buffers */
    status = Utils_bufDelete(&pObj->outFrameQue);

    UTILS_assert(status == FVID2_SOK);

    /* SW OSD Alg Delete */
    DM81XX_SWOSD_delete();
	
	for(i = 0; i < pObj->inQueInfo.numCh; i++)
	  SWOSD_imxDelete(i);

#ifdef SYSTEM_DEBUG_SWOSD
    Vps_printf(" %d: SWOSD   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}
void synchronizeTheCPIS(SwosdLink_GuiParams *pSwOsdGuiPrm, UInt32 streamId)
{
	isTransparencyEnabled[streamId] = pSwOsdGuiPrm->transparencyEnable;				
    Semaphore_pend(cpisSync, BIOS_WAIT_FOREVER ); 
	isIMXbasedSWOSDtoBeCalled = isTransparencyEnabled[0] || isTransparencyEnabled[1] || isTransparencyEnabled[2];  
    Semaphore_post(cpisSync); 
	
	if(isIMXbasedSWOSDtoBeCalled)
	{
	    do
		{
		}while(!isCPIS_GLBCE_deleted);
	}
	
	return;

}
 
/* Swosd link task main function */

/* ===================================================================
 *  @func     SwosdLink_tskMain
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
Void SwosdLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    UInt32 cmd = Utils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    SwosdLink_GuiParams *pSwOsdGuiPrm;

    SwosdLink_dateTime *pDateTimePrm;
	
	SwosdLink_vaStrPrm *pVaStrPrm;

    UInt32 streamId;

    UInt32 dateTimePrm[6];

    UInt32 flushCmds[2];

    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* create the swosd driver */
    status = SwosdLink_create(pObj, Utils_msgGetPrm(pMsg));

    Utils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
				SwosdLink_FreeGlobalInfo();
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                flushCmds[0] = SYSTEM_CMD_NEW_DATA;
                Utils_tskFlushMsg(pTsk, flushCmds, 1);

                SwosdLink_processFrames(pObj);
                break;

            case SWOSDLINK_CMD_GUIPRM:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                synchronizeTheCPIS(pSwOsdGuiPrm, streamId);
               

                DM81XX_SWOSD_setDateEnable(streamId, pSwOsdGuiPrm->dateEnable);
                DM81XX_SWOSD_setTransparencyEnable(streamId, pSwOsdGuiPrm->transparencyEnable);
                DM81XX_SWOSD_setTimeEnable(streamId, pSwOsdGuiPrm->timeEnable);
                DM81XX_SWOSD_setLogoEnable(streamId, pSwOsdGuiPrm->logoEnable);
                DM81XX_SWOSD_setLogoPosn(streamId, pSwOsdGuiPrm->logoPos);
                DM81XX_SWOSD_setTextEnable(streamId, pSwOsdGuiPrm->textEnable);
                DM81XX_SWOSD_setTextPosn(streamId, pSwOsdGuiPrm->textPos);
                DM81XX_SWOSD_setDtailedInfo(streamId,
                                            pSwOsdGuiPrm->detailedInfo);
                DM81XX_SWOSD_setUserString(streamId,
                                           (char *) pSwOsdGuiPrm->usrString);
                DM81XX_SWOSD_setCodecType(streamId, pSwOsdGuiPrm->encFormat);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_BR:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setCodecBitRate(streamId, pSwOsdGuiPrm->bitRate);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_RC:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setRateControl(streamId,
                                            pSwOsdGuiPrm->rateControl);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_FR:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                DM81XX_SWOSD_setCodecFrameRate(streamId,
                                               pSwOsdGuiPrm->frameRate);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_DATETIMEPOSN:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
                streamId = pSwOsdGuiPrm->streamId;

                dateTimePrm[0] = pSwOsdGuiPrm->dateFormat;
                dateTimePrm[1] = pSwOsdGuiPrm->datePos;
                dateTimePrm[2] = pSwOsdGuiPrm->timeFormat;
                dateTimePrm[3] = pSwOsdGuiPrm->timePos;

                DM81XX_SWOSD_setDateTimePosn(streamId, (Int32 *) dateTimePrm);

                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SWOSDLINK_CMD_DATETIME:
                pDateTimePrm = (SwosdLink_dateTime *) Utils_msgGetPrm(pMsg);
				g_osd_total_info.timeInfo[0] = pDateTimePrm->year;
				g_osd_total_info.timeInfo[1] = pDateTimePrm->month;
				g_osd_total_info.timeInfo[2] = pDateTimePrm->day;
				g_osd_total_info.timeInfo[3] = pDateTimePrm->hour;
				g_osd_total_info.timeInfo[4] = pDateTimePrm->min;
				g_osd_total_info.timeInfo[5] = pDateTimePrm->sec;
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

			case SWOSDLINK_CMD_VASTRING:
				pVaStrPrm = (SwosdLink_vaStrPrm *) Utils_msgGetPrm(pMsg);	
				
				for (streamId = 0; streamId <  pObj->inQueInfo.numCh; streamId++)
				{
				    DM81XX_SWOSD_setVaString(streamId,(char*)pVaStrPrm->vaString,pVaStrPrm->disFlag);
				}
				
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
				
			case SWOSDLINK_CMD_HISTENABLE:
                pSwOsdGuiPrm = (SwosdLink_GuiParams *) Utils_msgGetPrm(pMsg);
				
				for (streamId = 0; streamId <  pObj->inQueInfo.numCh; streamId++)
				{
					DM81XX_SWOSD_setHistEnable(streamId,pSwOsdGuiPrm->histEnable);
				}
				
				Utils_tskAckOrFreeMsg(pMsg, status);
                break;
			case SWOSDLINK_SETBMP_DATETIME:
				SwosdLink_SaveTimeInfo(pMsg, cmd);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			case SWOSDLINK_SETBMP_AUX_DATETIME:
				SwosdLink_SaveTimeInfo(pMsg, cmd);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
			case SWOSDLINK_SETBMP_LPRINFO:
				SwosdLink_SaveTimeInfo(pMsg, cmd);
				Utils_tskAckOrFreeMsg(pMsg, status);
				break;
            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* Delete the swosd driver */
    SwosdLink_delete(pObj);

    if (ackMsg && pMsg != NULL)
    {
        Utils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* Swosd link get info */

/* ===================================================================
 *  @func     SwosdLink_getInfo
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
Int32 SwosdLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

/* Swosd get full frames */

/* ===================================================================
 *  @func     SwosdLink_getFullFrames
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
Int32 SwosdLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    Int32 status;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    status = Utils_bufGetFull(&pObj->outFrameQue, pFrameList, BIOS_NO_WAIT);

    UTILS_assert(status == FVID2_SOK);

    return FVID2_SOK;
}

/* Swosd put empty frames */

/* ===================================================================
 *  @func     SwosdLink_putEmptyFrames
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
Int32 SwosdLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                               FVID2_FrameList * pFrameList)
{
    System_LinkInQueParams *pInQueParams;

    SwosdLink_Obj *pObj = (SwosdLink_Obj *) pTsk->appData;

    pInQueParams = &pObj->createArgs.inQueParams;

    /* Put the empty buffers back to previous link */
    System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                               pInQueParams->prevLinkQueId, pFrameList);

    return FVID2_SOK;
}

/* Swosd Link init */

/* ===================================================================
 *  @func     SwosdLink_init
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
Int32 SwosdLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 swosdId;

    SwosdLink_Obj *pObj;

    char tskName[32];

    for (swosdId = 0; swosdId < SWOSD_LINK_OBJ_MAX; swosdId++)
    {
        pObj = &gSwosdLink_obj[swosdId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = SwosdLink_getFullFrames;
        linkObj.linkPutEmptyFrames = SwosdLink_putEmptyFrames;
        linkObj.getLinkInfo = SwosdLink_getInfo;

        System_registerLink(SYSTEM_LINK_ID_SWOSD_0 + swosdId, &linkObj);

        sprintf(tskName, "SWOSD%d", swosdId);

        status = Utils_tskCreate(&pObj->tsk,
                                 SwosdLink_tskMain,
                                 SWOSD_LINK_TSK_PRI,
                                 gSwosdLink_tskStack[swosdId],
                                 SWOSD_LINK_TSK_STACK_SIZE, pObj, tskName);

        UTILS_assert(status == FVID2_SOK);
    }

    return status;
}

/* Swosd link de-init */

/* ===================================================================
 *  @func     SwosdLink_deInit
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
Int32 SwosdLink_deInit()
{
    UInt32 swosdId;

    for (swosdId = 0; swosdId < SWOSD_LINK_OBJ_MAX; swosdId++)
    {
        Utils_tskDelete(&gSwosdLink_obj[swosdId].tsk);
    }

    return FVID2_SOK;
}
