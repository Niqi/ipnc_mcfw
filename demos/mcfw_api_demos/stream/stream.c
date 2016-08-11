/** ===========================================================================
* @file stream.c
*
* @path $(IPNCPATH)\multimedia\encode_stream\stream
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include <mcfw/interfaces/ti_media_std.h>
#include <encode.h>
#include <stream.h>
#include <aewb_xdm.h>
#include <sys/time.h>
#include <osa.h>
#include "ti_vsys.h"
#include "ti_vcam.h"
#include "ti_venc.h"
#include "ti_mjpeg.h"
#include "ti_mcfw_ipnc_main.h"
#include "ti_vdis.h"
#include <mcfw/interfaces/link_api/system_linkId.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/vaLink.h>
#include <mcfw/interfaces/link_api/algVehicleLink.h>
#include <mcfw/interfaces/link_api/system_const.h>
#include <cmem.h>
#include <str2bmp.h>
#include <mcfw/interfaces/link_api/communicationIpcLink.h>

//#define DYN_CODEC_CHANGE

// #define __STREAM_DEBUG
typedef struct _OsdTextPrm {
    int nLegnth;
    char strText[16];
} OsdTextPrm;

/* VA specific function prototypes */
int ShareMemInit(int key); ///< Initial shared memory.
void ShareMemRead(int offset,void *buf, int length); ///< Read shared memory.
Int32 Vsys_setVaGuiPrm(VSYS_VA_GUISETPARAM vaGuiSetPrm,
                       VaLink_GuiParams *pVaPrm);
Int32 Vsys_setLprDynPrm(VSYS_LPR_DYN_PARAM_MODE lprPrmMode, 
						AlgVehicleLink_ThPlateIdDynParams *lprDynPrm);

#ifdef __STREAM_DEBUG
#define DBG(fmt, args...) fprintf(stderr, "Stream Debug: " fmt, ## args)
#endif

#define __E(fmt, args...) fprintf(stderr, "Stream Error: " fmt, ## args)

/* ===================================================================
 *  @func     Init_Msg_Func
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
static int Init_Msg_Func(STREAM_PARM * pParm);
/* ===================================================================
 *  @func     System_ipcNotifyRegisterAppCb
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
Void System_ipcNotifyRegisterAppCb(UInt32 procId,Void *pNotifyAppCb);
/* ===================================================================
 *  @func     System_ipcNotifyUnregisterAppCb
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
Void System_ipcNotifyUnregisterAppCb(UInt32 procId);

static STREAM_PARM stream_parm;

/* ===================================================================
 *  @func     stream_feature_setup
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
void stream_feature_setup(int nFeature, void *pParm);

/* extern */ AEW_EXT_PARAM Aew_ext_parameter;

static Int32 GetStrToBmpNum(const Int8 *str, Int32 len, Int32 *num)
{
	UInt8 *sbuf;
	Int32 loop = 0;
	Int32 cnt = 0;
	sbuf = (UInt8 *)str;

	if(0 == len || 128 < len){
		OSA_ERROR("string len is invalid, len: %d!\n", len);
		*num = 0;
		return OSA_EFAIL;
	}

	while(loop < len){
		if((sbuf[loop] < 128)){
			loop += 1;
			cnt += 1;
		}
		else{
			loop += 2;
			cnt += 1;
		}
	}
	if (MAX_OSD_BMP_NUM < cnt){
		OSA_printf("BMP number is overload, len: %d!\n", cnt);
		return OSA_EFAIL;
	}
	*num = cnt;

	return OSA_SOK;
}

Int32 HalCodecSetOsdDisplayTime(UInt8 u8EnableSteam0, UInt8 u8EnableSteam1, UInt32 u32X, UInt32 u32Y)
{
	Int32 Int32Ret = FAILURE;
	Int32 strBmpNum = 0;
	Int32 Int32X,Int32Y;
	TOSDPrm g_atOSDprm_datetime;
	static const Int8 datetime[] ={0x20,0x3a,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
		0x37,0x38,0x39,0x2f,0x2d,0xc4,0xea,0xd4,0xc2,0xc8,0xd5,0xca,0xb1,0xb7,0xd6,
		0xc3,0xeb,0xd0,0xc7,0xc6,0xda,0xc8,0xd5,0xd2,0xbb,0xb6,0xfe,0xc8,0xfd,0xcb,
		0xc4,0xce,0xe5,0xc1,0xf9};

    //OSA_printf("HalCodecSetOsdDisplayTime: u8Enable = %d u32X = %d u32Y = %d\n\n",u8EnableSteam0, u32X, u32Y);

    Int32X = (Int32)u32X;
    Int32Y = (Int32)u32Y;

	if (OSA_SOK != GetStrToBmpNum(datetime, sizeof(datetime), &strBmpNum)){
		OSA_ERROR("HalCodecSetOsdDisplayTime GetStrToBmpNum error!!! \n");
		return OSA_EFAIL;
	}

	/* main channel 1080p */
	g_atOSDprm_datetime.bmpNum = strBmpNum;
	g_atOSDprm_datetime.bmp = (osd_char *)calloc(strBmpNum, sizeof(osd_char));

	if (NULL == g_atOSDprm_datetime.bmp){
		OSA_ERROR("HalCodecSetOsdDisplayTime malloc g_atOSDprm_datetime.bmp error!!! \n");
		return OSA_EFAIL;
	}

	g_atOSDprm_datetime.bmp->len      = strBmpNum;
	g_atOSDprm_datetime.bmp->fontsize = g_fontsize;
	g_atOSDprm_datetime.bmp->textPosX = Int32X;
	g_atOSDprm_datetime.bmp->textPosY = Int32Y;
	g_atOSDprm_datetime.bmp->flag     = (char)u8EnableSteam0;
	Int32Ret = Str2Bmp_String2BmpBuffer(datetime, sizeof(datetime), &g_atOSDprm_datetime.bmp, OSD_MENU_1080P);
	if(Int32Ret != OSA_SOK){
		OSA_ERROR("Str2Bmp_String2BmpBuffer!\n");
		return OSA_EFAIL;
	}
	if(g_atOSDprm_datetime.bmp == NULL){
		OSA_printf("g_atOSDprm_datetime.bmp!\n");
		return OSA_EFAIL;
	}
	stream_feature_setup(STREAM_FEATURE_DATETIMEPRM, &g_atOSDprm_datetime);
	g_atOSDprm_datetime.bmp = NULL;

	/* aux channel D1 */
	g_atOSDprm_datetime.bmpNum = strBmpNum;
	g_atOSDprm_datetime.bmp = (osd_char *)calloc(strBmpNum, sizeof(osd_char));

	if (NULL == g_atOSDprm_datetime.bmp){
		OSA_ERROR("HalCodecSetOsdDisplayTime malloc g_atOSDprm_datetime.bmp error!!! \n");
		return OSA_EFAIL;
	}

	g_atOSDprm_datetime.bmp->len      = strBmpNum;
	g_atOSDprm_datetime.bmp->fontsize = 0;
	g_atOSDprm_datetime.bmp->textPosX = Int32X;
	g_atOSDprm_datetime.bmp->textPosY = Int32Y;
	g_atOSDprm_datetime.bmp->flag     = (char)u8EnableSteam1;
	Int32Ret = Str2Bmp_String2BmpBuffer(datetime, sizeof(datetime), &g_atOSDprm_datetime.bmp, OSD_MENU_D1);
	if(Int32Ret != OSA_SOK){
		OSA_ERROR("Str2Bmp_String2BmpBuffer!\n");
		return OSA_EFAIL;
	}
	if(g_atOSDprm_datetime.bmp == NULL){
		OSA_printf("g_atOSDprm_datetime.bmp!\n");
		return OSA_EFAIL;
	}
	stream_feature_setup(STREAM_FEATURE_AUX_DATETIMEPRM, &g_atOSDprm_datetime);
	g_atOSDprm_datetime.bmp = NULL;

	OSA_printf( "HalCodecSetOsdDisplayTime: Successful\n");
	return OSA_SOK;
}

/*
给用户设定字符串的接口
u8StreamID 表示码流的通道,0-主码流 1080p, 1-辅码流 480p, 2-JPEG码流 1080p
u8Enable 表示使能显示该码流的文本信息,0-disable,1-enable
String 将要显示的字符串,限制字节为32Bytes,即最大允许16个汉字或者32个英文
u32X,u32Y 显示字符串在图像的开始坐标,取值范围0-1000,
*/
Int32 HalCodecSetOsdUserTextString(UInt8 u8StreamID, UInt8 u8Enable, Int8 *String,UInt32 u32X, UInt32 u32Y)
{
	Int32 Int32Ret = FAILURE;
	Int32 strBmpNum = 0;
	UInt32 strLen=0,cmdID=STREAM_FEATURE_OSD_USERTEXT_AUX;
	TOSDPrm mUserTextOSD;
	Int8 mString[32]={0};

	if((u8StreamID != 0) && (u8StreamID != 1) && (u8StreamID != 2)){
		return OSA_EFAIL;
	}
	if((u8Enable != 0) && (u8Enable != 1)){
		return OSA_EFAIL;
	}
	if(u8Enable == 1){
		if(String == NULL){
			return OSA_EFAIL;
		}
		if(strlen(String)>32){
			return OSA_EFAIL;
		}
	}

	if((u32X > 1000) || (u32Y > 1000)){
		return OSA_EFAIL;
	}

    OSA_printf("HalCodecSetOsdUserTextString: ID=%d,EN=%d,STR=%s,(X,Y)=(%d,%d)\n",
		u8StreamID, u8Enable,String,u32X,u32Y);

	if(u8Enable){
		memcpy(mString,String,strlen(String));
	}
	else{
		memcpy(mString,"  ",2); /**/
	}
    strLen = strlen(mString);
	if (OSA_SOK != GetStrToBmpNum(mString, strLen, &strBmpNum)){
		OSA_ERROR("GetStrToBmpNum!!! \n");
		return OSA_EFAIL;
	}

	/* main channel 1080p */
	mUserTextOSD.bmpNum = strBmpNum;
	mUserTextOSD.bmp = (osd_char *)calloc(strBmpNum, sizeof(osd_char));

	if (NULL == mUserTextOSD.bmp){
		OSA_ERROR("NULL == mUserTextOSD.bmp!!! \n");
		return OSA_EFAIL;
	}
	/* 根据图像的分辨率确定显示的位置,即将0-1000映射到对应的坐标 */
	/* 以下约定主码流和三码流的分辨率为1920x1080,赋码流为720x480*/
	if(u8StreamID == 1)
	{
		Int32 x=0,y=0;
		x = u32X*720/1000;
		y = u32Y*480/1000;
		if(x > (720-strBmpNum*16)) x = (720-strBmpNum*16);/* 16表示文字的宽和高*/
		if(y > (480-16)) y = 480-16;
		cmdID = STREAM_FEATURE_OSD_USERTEXT_AUX;
		mUserTextOSD.bmp->len      = strBmpNum;
		mUserTextOSD.bmp->fontsize = 0;
		mUserTextOSD.bmp->textPosX = x;
		mUserTextOSD.bmp->textPosY = y;
		mUserTextOSD.bmp->flag     = u8Enable;
		Int32Ret = Str2Bmp_String2BmpBuffer(mString, strLen, &mUserTextOSD.bmp, OSD_MENU_D1);
	}
	else {
		Int32 x=0,y=0;
		x = u32X*1920/1000;
		y = u32Y*1080/1000;
		if(x > (1920-strBmpNum*40)) x = (1920-strBmpNum*40);/* 40表示文字的宽和高*/
		if(y > (1080-40)) y = 1080-40;
		if(u8StreamID == 0)
			cmdID = STREAM_FEATURE_OSD_USERTEXT;
		else
			cmdID = STREAM_FEATURE_OSD_USERTEXT_LPRINFO;
		mUserTextOSD.bmp->len      = strBmpNum;
		mUserTextOSD.bmp->fontsize = g_fontsize;
		mUserTextOSD.bmp->textPosX = x;
		mUserTextOSD.bmp->textPosY = y;
		mUserTextOSD.bmp->flag     = u8Enable;
		Int32Ret = Str2Bmp_String2BmpBuffer(mString, strLen, &mUserTextOSD.bmp, OSD_MENU_1080P);
	}
	if(Int32Ret != OSA_SOK){
		OSA_ERROR("Str2Bmp_String2BmpBuffer!\n");
		return OSA_EFAIL;
	}
	if(mUserTextOSD.bmp == NULL){
		OSA_printf("mUserTextOSD.bmp == NULL!\n");
		return OSA_EFAIL;
	}
	stream_feature_setup(cmdID, &mUserTextOSD);
	mUserTextOSD.bmp = NULL;

	OSA_printf( "HalCodecSetOsdUserTextString: Successful\n");
	return OSA_SOK;
}

static Int32 getCurTimeInString(Int8 *str)
{
	time_t now;
	struct tm *timenow;

	if(!str){
		return OSA_EFAIL;
	}
	time(&now);
	timenow = gmtime(&now);
	sprintf(str,"%04d%02d%02d%02d%02d%02d  ",\
		timenow->tm_year+1900,
		timenow->tm_mon+1,
		timenow->tm_mday,
		timenow->tm_hour,
		timenow->tm_min,
		timenow->tm_sec);
	return OSA_SOK;
}

Int32 HalCodecSetOsdDisplayLprInfo(UInt32 timeStamp, DSP_LPR_RESULT *result)
{
	Int32 Int32Ret = FAILURE;
	Int32 strBmpNum = 0;
	TOSDPrm g_atOSDprm_lprInfo;
	Int32 ret, strLen;
	Int8 tmpStr[128]={0};
	
	ret = getCurTimeInString(tmpStr);
	if(ret == OSA_SOK){
		strcat(tmpStr, result->license);
	}
	else
	{
		return OSA_EFAIL;
	}

	strLen = strlen(tmpStr);

	if (OSA_SOK != GetStrToBmpNum(tmpStr, strLen, &strBmpNum)){
		OSA_ERROR("GetStrToBmpNum!!! \n");
		return OSA_EFAIL;
	}

	/* main channel 1080p */
	g_atOSDprm_lprInfo.bmpNum = strBmpNum;
	g_atOSDprm_lprInfo.bmp = (osd_char *)calloc(strBmpNum, sizeof(osd_char));

	if (NULL == g_atOSDprm_lprInfo.bmp){
		OSA_ERROR("NULL == g_atOSDprm_lprInfo.bmp!!! \n");
		return OSA_EFAIL;
	}

	g_atOSDprm_lprInfo.bmp->len      = strBmpNum;
	g_atOSDprm_lprInfo.bmp->fontsize = g_fontsize;
	g_atOSDprm_lprInfo.bmp->textPosX = 20;
	g_atOSDprm_lprInfo.bmp->textPosY = 10;
	g_atOSDprm_lprInfo.bmp->flag     = TRUE;
	g_atOSDprm_lprInfo.bmp->timeStamp = timeStamp;
	g_atOSDprm_lprInfo.bmp->nRectLeftX = result->nRectLeftX;
	g_atOSDprm_lprInfo.bmp->nRectLeftY = result->nRectLeftY;
	g_atOSDprm_lprInfo.bmp->nRectWidth= result->nRectWidth;
	g_atOSDprm_lprInfo.bmp->nRectHeight= result->nRectHeight;
	Int32Ret = Str2Bmp_String2BmpBuffer(tmpStr, strLen, &g_atOSDprm_lprInfo.bmp, OSD_MENU_1080P);
	if(Int32Ret != OSA_SOK){
		OSA_ERROR("Str2Bmp_String2BmpBuffer!\n");
		return OSA_EFAIL;
	}
	if(g_atOSDprm_lprInfo.bmp == NULL){
		OSA_printf("g_atOSDprm_lprInfo.bmp == NULL!\n");
		return OSA_EFAIL;
	}
	stream_feature_setup(STREAM_FEATURE_LPRINFOPRM, &g_atOSDprm_lprInfo);
	g_atOSDprm_lprInfo.bmp = NULL;

	return OSA_SOK;
}

Int32 Halcodec_RegisterCallbackFunc(void)
{
	HOST_CB_FUNC tmpCb = (HOST_CB_FUNC)HalCodecSetOsdDisplayLprInfo;
	System_linkControl(SYSTEM_HOST_LINK_ID_IPC_COMMUNICATION,
							   IPCCOMMUNICATION_CMD_CALLBACK,
							   &tmpCb,
							   sizeof(HOST_CB_FUNC),
							   TRUE);
	return OSA_SOK;
}
/*
static unsigned int GetTimeStamp(void)
{
    struct timeval timeval;

    gettimeofday(&timeval, NULL);

    return ((timeval.tv_sec * 1000) + ((timeval.tv_usec + 500) / 1000));
}
*/

/**
 * @brief   Get stream handle
 * @param   none
 * @return  pointer to stream_parm
 */
/* ===================================================================
 *  @func     stream_get_handle
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
STREAM_PARM *stream_get_handle(void)
{
    return &stream_parm;
}

extern void SendAlarmMotionrDetect(int Serial);
extern void SendDMVAAlarmEventStart(int EvtType);
extern void SendDMVAAlarmEventStop();

/* Notify callback function from Video M3 */
Void NotifyCbVideoM3(UInt16 procId, UInt16 lineId, UInt32 eventId, UArg arg,
                     UInt32 payload)
{
    if (payload == SYSTEM_IPC_EVTTYPE_MD)
    {
        SendAlarmMotionrDetect(0);
    }
}

Void NotifyCbVpssM3(UInt16 procId, UInt16 lineId, UInt32 eventId, UArg arg,
                    UInt32 payload)
{
    if(payload == VALINK_EVT_STOP)
    {
        SendDMVAAlarmEventStop();
    }
    else
    {
        SendDMVAAlarmEventStart(payload);
    }
}

Void NotifyCbDsp(UInt16 procId, UInt16 lineId, UInt32 eventId, UArg arg,
                 UInt32 payload)
{
	Ptr ptBuf = NULL;
    TDSPLprResualt *ptsOsdPayload = NULL;
    ptBuf = SharedRegion_getPtr(payload);
	ptsOsdPayload = (TDSPLprResualt *)ptBuf;
	struct timeval start,end;

	if(ptsOsdPayload!=NULL){
		if(ptsOsdPayload->payload == 0x00000001){
			gettimeofday(&start, NULL); 
			//HalCodecSetOsdDisplayLprInfo((Int32)SharedRegion_getPtr(ptsOsdPayload->frameAddr), &ptsOsdPayload->lprResult);
			HalCodecSetOsdDisplayLprInfo(ptsOsdPayload->timeStamp, &ptsOsdPayload->lprResult);
			gettimeofday(&end, NULL); 
			//OSA_printf("HalCodecSetOsdDisplayLprInfo:%ld \n", end.tv_usec - start.tv_usec);
		}
	}
}

extern int AlarmDrvInit(int proc_id);

/**
 * @brief   Stream initialization
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @param   "STREAM_SET *pSet"
 * @return  0 : success ;
 */
/* ===================================================================
 *  @func     stream_init
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
int stream_init(STREAM_PARM * pParm, STREAM_SET * pSet)
{
    int cnt = 0;

    /* Init memory manager */
    pParm->MemInfo.mem_layout = pSet->Mem_layout;
    if (MemMng_Init(&(pParm->MemInfo)) != STREAM_SUCCESS)
    {
        ERR("Memory manager init fail\n");
        return STREAM_FAILURE;
    }
    if (CacheMng_Init(&(pParm->MemInfo)) != STREAM_SUCCESS)
    {
        ERR("Cache manager init fail\n");
        return STREAM_FAILURE;
    }

    GopInit();

    for (cnt = 0; cnt < STREAM_SEM_NUM; cnt++)
    {
        pParm->MemMngSemId[cnt] = Sem_Creat(cnt);
    }

    for (cnt = 0; cnt < GOP_INDEX_NUM; cnt++)
    {
        pParm->lockNewFrame[cnt] = 0;
        pParm->checkNewFrame[cnt] = 0;
        Rendezvous_open(&(pParm->objRv[cnt]), 2);
    }

    pParm->ImageWidth = pSet->ImageWidth;
    pParm->ImageHeight = pSet->ImageHeight;

    for (cnt = 0; cnt < STREAM_EXT_NUM; cnt++)
    {
        pParm->ImageWidth_Ext[cnt] = pSet->ImageWidth_Ext[cnt];
        pParm->ImageHeight_Ext[cnt] = pSet->ImageHeight_Ext[cnt];
    }
    pParm->JpgQuality = pSet->JpgQuality;
    pParm->Mpeg4Quality = pSet->Mpeg4Quality;

    /* Register Notify Cb for the APP */
    System_ipcNotifyRegisterAppCb(SYSTEM_PROC_M3VPSS,(void*)NotifyCbVpssM3);
    System_ipcNotifyRegisterAppCb(SYSTEM_PROC_M3VIDEO,(void*)NotifyCbVideoM3);

#ifdef IPNC_DSP_ON
    System_ipcNotifyRegisterAppCb(SYSTEM_PROC_DSP,(void*)NotifyCbDsp);
#endif

	Str2Bmp_Initial(VideoStd_1080I_25);
    return STREAM_SUCCESS;
}

/**
 * @brief   App Message Handler Init
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @param   "STREAM_SET *pSet"
 * @return  0 : success ;
 */
/* ===================================================================
 *  @func     App_msgHandlerInit
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
int App_msgHandlerInit(STREAM_PARM *pParm)
{
	int ret = 0;

	CMEM_init();

	ShareMemInit(SYS_MSG_KEY);

    pParm->IsQuit = 0;
    pParm->qid = Msg_Init(MSG_KEY);

    ret = Init_Msg_Func(pParm);
    if (ret < 0)
    {
        ERR("Init_Msg_Func init fail\n");
        return STREAM_FAILURE;
    }

    /* Alarm Drv Init */
    AlarmDrvInit(ALARM_MCFW_MSG);

	return STREAM_SUCCESS;
}

/**
 * @brief   Stream initialization
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @param   "STREAM_SET *pSet"
 * @return  0 : success ;
 */
/* ===================================================================
 *  @func     stream_update_vol
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

int stream_update_vol(STREAM_PARM * pParm,int streamType)
{
	VIDEO_BLK_INFO *pVidInfo = NULL;
	int sem_id = 0;

    switch(streamType)
    {
        case STREAM_H264_1:
        case STREAM_MP4:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MP4]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
            break;

        case STREAM_MJPG:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MJPG]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
            break;
        case STREAM_H264_2:
        case STREAM_MP4_EXT:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
            break;

        case STREAM_AUDIO:
            pVidInfo = &(pParm->MemInfo.video_info[AUDIO_INFO_G711]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
            break;

        default:
            break;
    }

	Sem_lock(sem_id);
	pVidInfo->updateExtraData = 1;
	Sem_unlock(sem_id);

	return STREAM_SUCCESS;
}

// int sem_status = 0;
/**
 * @brief   Stream write
 * @param   "void *pAddr"
 * @param   "int size"
 * @param   "int frame_type"
 * @param   "int stream_type"
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @return  0 : success ; -1 : fial
 */
/* ===================================================================
 *  @func     stream_write
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
int stream_write(void *pAddr, int size, int frame_type, int stream_type,
                 unsigned int timestamp, unsigned int temporalId,
                 char * license, STREAM_PARM * pParm)
{
    VIDEO_BLK_INFO *pVidInfo = NULL;

    int ret = -1;

    int sem_id = 0;

    int cnt = 0;

    switch (stream_type)
    {
        case STREAM_H264_1:
        case STREAM_MP4:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MP4]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
            break;

        case STREAM_MJPG:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MJPG]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
            break;
        case STREAM_H264_2:
        case STREAM_MP4_EXT:
            pVidInfo = &(pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
            break;

        case STREAM_AUDIO:
            pVidInfo = &(pParm->MemInfo.video_info[AUDIO_INFO_G711]);
            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
            break;

        default:

            break;

    }

    // OSA_printf("%s:pAddr = 0x%X,size = %d,frame_type = %d,timestamp =
    // %d\n",__func__,pAddr,size,frame_type,timestamp);
    // OSA_printf( "frame_type : %d, size : %d, stream_type : %d, timestamp : %u
    // \n",frame_type, size,stream_type, timestamp );
    // OSA_printf( "width : %d, height: %d \n",pVidInfo->width,pVidInfo->height);
    // OSA_printf( "pVidInfo->extrasize: %d \n",pVidInfo->extrasize);

    if (stream_type == STREAM_H264_1 || stream_type == STREAM_H264_2)
    {
        // pVidInfo->extrasize = 14;
    }
    else if (stream_type == STREAM_MP4 || stream_type == STREAM_MP4_EXT)
    {
        // pVidInfo->extrasize = 18;
    }
    else
    {
        pVidInfo->extrasize = 0;
    }

    Sem_lock(sem_id);
    // sem_status--;

    pVidInfo->timestamp = timestamp;
    // pVidInfo->timestamp = GetTimeStamp();
    pVidInfo->temporalId = temporalId;
    if(STREAM_MJPG == stream_type)
    {
        strcpy(pVidInfo->license, license);
    }

    ret = MemMng_Video_Write(pAddr, size, frame_type, pVidInfo);
    if (ret < 0)
    {
        OSA_printf("Error code: %d at %d\n", ret, __LINE__);
    }

    for (cnt = 0; cnt < VIDOE_INFO_END; cnt++)
    {
        pVidInfo->frame[pVidInfo->cur_frame].ref_serial[cnt] =
            pParm->MemInfo.video_info[cnt].cur_serial;
    }

    /* post process */
    switch (stream_type)
    {
        case STREAM_H264_1:
        case STREAM_MP4:

            if (pParm->lockNewFrame[GOP_INDEX_MP4] && ret == 0)
            {
                if (frame_type == P_FRAME)
                {
                    ret = LockCurrentGopP(pVidInfo, GOP_INDEX_MP4);
                    if (ret)
                    {
                        OSA_printf("Error code: %d at %d\n", ret, __LINE__);
                        pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
                    }
                }
                else if (frame_type == I_FRAME)
                {
                    // DBG(": Line %d\n", __LINE__);
                    pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
                }
            }
            if (pParm->checkNewFrame[GOP_INDEX_MP4])
            {
                Rendezvous_meet(&(pParm->objRv[GOP_INDEX_MP4]));
                pParm->checkNewFrame[GOP_INDEX_MP4] = 0;
                Rendezvous_reset(&(pParm->objRv[GOP_INDEX_MP4]));
            }

            break;

        case STREAM_AUDIO:

            if (pParm->checkNewFrame[GOP_INDEX_AUDIO])
            {
                Rendezvous_meet(&(pParm->objRv[GOP_INDEX_AUDIO]));
                pParm->checkNewFrame[GOP_INDEX_AUDIO] = 0;
                Rendezvous_reset(&(pParm->objRv[GOP_INDEX_AUDIO]));
            }

            break;

        case STREAM_H264_2:
        case STREAM_MP4_EXT:
            if (pParm->lockNewFrame[GOP_INDEX_MP4_EXT] && ret == 0)
            {
                if (frame_type == P_FRAME)
                {
                    ret = LockCurrentGopP(pVidInfo, GOP_INDEX_MP4_EXT);
                    if (ret)
                    {
                        OSA_printf("Error code: %d at %d\n", ret, __LINE__);
                        pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
                    }
                }
                else if (frame_type == I_FRAME)
                {
                    // DBG(": Line %d\n", __LINE__);
                    pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
                }
            }
            if (pParm->checkNewFrame[GOP_INDEX_MP4_EXT])
            {
                Rendezvous_meet(&(pParm->objRv[GOP_INDEX_MP4_EXT]));
                pParm->checkNewFrame[GOP_INDEX_MP4_EXT] = 0;
                Rendezvous_reset(&(pParm->objRv[GOP_INDEX_MP4_EXT]));
            }
            break;

        case STREAM_MJPG:
            if (pParm->checkNewFrame[GOP_INDEX_JPEG])
            {
                Rendezvous_meet(&(pParm->objRv[GOP_INDEX_JPEG]));
                pParm->checkNewFrame[GOP_INDEX_JPEG] = 0;
                Rendezvous_reset(&(pParm->objRv[GOP_INDEX_JPEG]));
            }
            break;

        default:

            break;

    }

    // sem_status++;
    Sem_unlock(sem_id);

    if (ret == GOP_ERROR_OP)
        ret = 0;

    /* if( ret < 0 ) { return STREAM_FAILURE; } else { return STREAM_SUCCESS;
     * } */
    return STREAM_SUCCESS;

}

/**
 * @brief   Send quit command
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @return  none
 */
/* ===================================================================
 *  @func     SendQuitCmd
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
static void SendQuitCmd(STREAM_PARM * pParm)
{

    MSG_BUF msgbuf;

    memset(&msgbuf, 0, sizeof(msgbuf));
    msgbuf.Des = MSG_TYPE_MSG1;
    msgbuf.Src = 0;
    msgbuf.cmd = MSG_CMD_QUIT;
    Msg_Send(pParm->qid, &msgbuf, sizeof(msgbuf));

    pParm->IsQuit = 1;

}

extern int AlarmDrvExit();

/**
 * @brief   Stream end
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @return  none
 */
/* ===================================================================
 *  @func     stream_end
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
int stream_end(STREAM_PARM * pParm)
{
    int cnt = 0;

    for (cnt = 0; cnt < GOP_INDEX_NUM; cnt++)
    {
        Rendezvous_force(&(pParm->objRv[cnt]));
        Rendezvous_close(&(pParm->objRv[cnt]));
    }

    for (cnt = 0; cnt < STREAM_SEM_NUM; cnt++)
    {
        Sem_kill(pParm->MemMngSemId[cnt]);
    }

    GopCleanup(&pParm->MemInfo.video_info[VIDOE_INFO_MP4], GOP_INDEX_MP4);
    GopCleanup(&pParm->MemInfo.video_info[VIDOE_INFO_MP4], GOP_INDEX_MP4_EXT);

    CacheMng_Release(&(pParm->MemInfo));
    MemMng_release(&(pParm->MemInfo));

    /* Unregister Notify Cb used for APP */
    System_ipcNotifyUnregisterAppCb(SYSTEM_PROC_M3VPSS);
    System_ipcNotifyUnregisterAppCb(SYSTEM_PROC_M3VIDEO);

#ifdef IPNC_DSP_ON
    System_ipcNotifyUnregisterAppCb(SYSTEM_PROC_DSP);
#endif

	Str2Bmp_Deinitial();

    return STREAM_SUCCESS;
}

/**
 * @brief   App Message Handler Exit
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @param   "STREAM_SET *pSet"
 * @return  0 : success ;
 */
/* ===================================================================
 *  @func     App_msgHandlerExit
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
int App_msgHandlerExit(STREAM_PARM *pParm)
{
    SendQuitCmd(pParm);

    pthread_cancel(pParm->threadControl);
    pthread_join(pParm->threadControl, (void **) 0);

    fprintf(stderr, "Msg_ThrFxn closing...\n");

    Msg_Kill(pParm->qid);
    memset(pParm, 0, sizeof(STREAM_PARM));

	fprintf(stderr, "Msg_Kill done...\n");

	/* This delay is required otherwise there is system hang */
	usleep(50 * 1000);

    /* Alarm Drv Exit */
    AlarmDrvExit();

	CMEM_exit();

	return STREAM_SUCCESS;
}

/**
 * @brief   Show info
 * @param   "STREAM_PARM *pParm" : stream parameter
 * @param   "int id"
 * @return  none
 */
/* ===================================================================
 *  @func     ShowInfo
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
void ShowInfo(STREAM_PARM * pParm, int id)
{
    DBG("ID = %d \n", id);
    DBG("pParm->ImageHeight = %d \n", pParm->ImageHeight);
    DBG("pParm->ImageWidth = %d \n", pParm->ImageWidth);
    DBG("pParm->JpgQuality = %d \n", pParm->JpgQuality);
    DBG("pParm->Mpeg4Quality = %d \n", pParm->Mpeg4Quality);
    DBG("pParm->qid = %d \n", pParm->qid);

}

/**
 * @brief   Message control thread
 * @param   "void* args" : arguments
 * @return  none
 */
/* ===================================================================
 *  @func     Msg_CTRL
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
void *Msg_CTRL(void *args)
{
    STREAM_PARM *pParm = (STREAM_PARM *) args;

    int qid;

    int msg_size = 0;

    MSG_BUF msgbuf;

    // ShowInfo(pParm, 1);

    qid = (int) pParm->qid;
    Rendezvous_Handle hRv = &pParm->objRv[0];

    while ( /* (!gblGetQuit())&& */ (pParm->IsQuit == 0))
    {
        msg_size = msgrcv(qid, &msgbuf, sizeof(msgbuf) - sizeof(long),
                          MSG_TYPE_MSG1, 0);
        if (msg_size < 0)
        {
            ERR("Receive msg fail \n");
            break;
        }
        else
        {
            switch (msgbuf.cmd)
            {
                case MSG_CMD_GET_MEM:
                    DBG("MSG_CMD_GET_MEM case \n");
                    msgbuf.mem_info.addr =
                        GetMemMngPhyBaseAddr(&(pParm->MemInfo));
                    msgbuf.mem_info.size =
                        GetMemMngTotalMemSize(&(pParm->MemInfo));
                    if (msgbuf.mem_info.addr != 0 && msgbuf.mem_info.size > 0)
                        msgbuf.ret = 0;
                    else
                        msgbuf.ret = -1;
                    break;
                case MSG_CMD_SET_MEM:
                    DBG("MSG_CMD_SET_MEM case \n");
                    break;
                case MSG_CMD_PRINT_MEM:
                    DBG("MSG_CMD_PRINT_MEM case \n");
                    break;
                case MSG_CMD_QUIT:
                    pParm->IsQuit = 1;
                    break;
                case MSG_CMD_GET_NEW_FRAME:

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            pParm->checkNewFrame[GOP_INDEX_JPEG] = 1;
                            Rendezvous_meet(&hRv[GOP_INDEX_JPEG]);
                            break;

                        case FMT_MPEG4:
                            pParm->checkNewFrame[GOP_INDEX_MP4] = 1;
                            Rendezvous_meet(&hRv[GOP_INDEX_MP4]);
                            break;

                        case FMT_MPEG4_EXT:
                            pParm->checkNewFrame[GOP_INDEX_MP4_EXT] = 1;
                            Rendezvous_meet(&hRv[GOP_INDEX_MP4_EXT]);
                            break;

                        case FMT_AUDIO:
                            pParm->checkNewFrame[GOP_INDEX_AUDIO] = 1;
                            Rendezvous_meet(&hRv[GOP_INDEX_AUDIO]);
                            break;
                    }

                    /* Break through */
                case MSG_CMD_GET_CUR_FRAME:
                {
                    VIDEO_FRAME *pFrame = NULL;

                    VIDEO_BLK_INFO *pVidInfo = NULL;

                    int sem_id = 0;

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
                            break;

                        case FMT_MPEG4:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
                            break;

                        case FMT_MPEG4_EXT:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
                            break;

                        case FMT_AUDIO:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            break;
                    }

                    Sem_lock(sem_id);

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MJPG];
                            pFrame = GetCurrentFrame(pVidInfo);
                            break;
                        case FMT_MPEG4:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4];
                            pFrame = GetLastI_Frame(pVidInfo);
                            break;
                        case FMT_MPEG4_EXT:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT];
                            pFrame = GetLastI_Frame(pVidInfo);
                            break;
                        case FMT_AUDIO:
                            pVidInfo =
                                &pParm->MemInfo.video_info[AUDIO_INFO_G711];
                            pFrame = GetCurrentFrame(pVidInfo);
                            break;
                    }

                    if (pFrame == NULL)
                    {
                        // DBG("MSG_CMD_GET_CUR_FRAME : pFrame = NULL \n");
                        msgbuf.ret = -1;
                    }
                    else
                    {
                        int cnt = 0;

                        msgbuf.frame_info.serial_no = pFrame->serial;
                        msgbuf.frame_info.size = pFrame->realsize;
                        msgbuf.frame_info.offset = GetCurrentOffset(pVidInfo);
                        switch (msgbuf.frame_info.format)
                        {
                            case FMT_MJPEG:
                                msgbuf.frame_info.width =
                                    pParm->ImageWidth_Ext[STREAM_EXT_JPG];
                                msgbuf.frame_info.height =
                                    pParm->ImageHeight_Ext[STREAM_EXT_JPG];
                                break;
                            case FMT_MPEG4:
                                msgbuf.frame_info.width = pParm->ImageWidth;
                                msgbuf.frame_info.height = pParm->ImageHeight;
                                break;
                            case FMT_MPEG4_EXT:
                                msgbuf.frame_info.width =
                                    pParm->ImageWidth_Ext[STREAM_EXT_MP4_CIF];
                                msgbuf.frame_info.height =
                                    pParm->ImageHeight_Ext[STREAM_EXT_MP4_CIF];
                                break;
                        }
                        msgbuf.frame_info.flags = pFrame->flag;
                        msgbuf.frame_info.frameType = pFrame->fram_type;
                        msgbuf.frame_info.timestamp = pFrame->timestamp;
                        msgbuf.frame_info.temporalId = pFrame->temporalId;
						if(FMT_MJPEG == msgbuf.frame_info.format)
						{
							strcpy(msgbuf.frame_info.license, pFrame->license);
						}
                        for (cnt = 0; cnt < VIDOE_INFO_END; cnt++)
                        {
                            msgbuf.frame_info.ref_serial[cnt] =
                                pFrame->ref_serial[cnt];
                        }

                        msgbuf.ret = 0;
                    }
                    Sem_unlock(sem_id);
                    break;
                }
                case MSG_CMD_LOCK_FRAME:
                {
                    VIDEO_BLK_INFO *pVidInfo = NULL;

                    int sem_id = 0;

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
                            break;

                        case FMT_MPEG4:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
                            break;

                        case FMT_MPEG4_EXT:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
                            break;

                        case FMT_AUDIO:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            break;
                    }
                    Sem_lock(sem_id);

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MJPG];
                            msgbuf.frame_info.quality = pParm->JpgQuality;
                            msgbuf.ret =
                                CacheMng_Video_CacheLock(msgbuf.frame_info.
                                                         serial_no, pVidInfo);
                            break;
                        case FMT_AUDIO:
                            pVidInfo =
                                &pParm->MemInfo.video_info[AUDIO_INFO_G711];
                            msgbuf.frame_info.quality = 0;
                            msgbuf.ret =
                                CacheMng_Video_CacheLock(msgbuf.frame_info.
                                                         serial_no, pVidInfo);
                            break;
                        case FMT_MPEG4:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4];
                            msgbuf.frame_info.quality = pParm->Mpeg4Quality;
                            msgbuf.ret =
                                LockGopBySerial(msgbuf.frame_info.serial_no,
                                                pVidInfo, GOP_INDEX_MP4);

                            if (msgbuf.ret == GOP_INCOMPLETE)
                            {
                                msgbuf.ret = 0;
                                pParm->lockNewFrame[GOP_INDEX_MP4] = 1;
                            }
                            break;
                        case FMT_MPEG4_EXT:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT];
                            msgbuf.frame_info.quality = pParm->Mpeg4Quality;
                            msgbuf.ret =
                                LockGopBySerial(msgbuf.frame_info.serial_no,
                                                pVidInfo, GOP_INDEX_MP4_EXT);

                            if (msgbuf.ret == GOP_INCOMPLETE)
                            {
                                msgbuf.ret = 0;
                                pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 1;
                            }
                            break;
                    }
                    if (msgbuf.ret == 0)
                    {
                        CACHE_DATA_INFO Cache_info;

                        int cnt = 0;

                        CacheMng_GetCacheInfoBySerial(msgbuf.
                                                      frame_info.serial_no,
                                                      &Cache_info, pVidInfo);
                        msgbuf.frame_info.offset =
                            Cache_info.start_phy -
                            GetMemMngPhyBaseAddr(&pParm->MemInfo);
                        msgbuf.frame_info.size = Cache_info.realsize;

                        switch (msgbuf.frame_info.format)
                        {
                            case FMT_MJPEG:
                                msgbuf.frame_info.width =
                                    pParm->ImageWidth_Ext[STREAM_EXT_JPG];
                                msgbuf.frame_info.height =
                                    pParm->ImageHeight_Ext[STREAM_EXT_JPG];
                                break;
                            case FMT_MPEG4:
                                msgbuf.frame_info.width = pParm->ImageWidth;
                                msgbuf.frame_info.height = pParm->ImageHeight;
                                break;
                            case FMT_MPEG4_EXT:
                                msgbuf.frame_info.width =
                                    pParm->ImageWidth_Ext[STREAM_EXT_MP4_CIF];
                                msgbuf.frame_info.height =
                                    pParm->ImageHeight_Ext[STREAM_EXT_MP4_CIF];
                                break;
                        }
                        msgbuf.frame_info.flags = Cache_info.flag;
                        msgbuf.frame_info.frameType = Cache_info.fram_type;
                        msgbuf.frame_info.timestamp = Cache_info.timestamp;
                        msgbuf.frame_info.temporalId = Cache_info.temporalId;
                        for (cnt = 0; cnt < VIDOE_INFO_END; cnt++)
                        {
                            msgbuf.frame_info.ref_serial[cnt] =
                                Cache_info.ref_serial[cnt];
                        }
                    }
                    Sem_unlock(sem_id);
                    break;
                }
                case MSG_CMD_UNLOCK_FRAME:
                {
                    VIDEO_BLK_INFO *pVidInfo = NULL;

                    int sem_id = 0, ret;

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
                            break;

                        case FMT_MPEG4:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
                            break;

                        case FMT_MPEG4_EXT:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
                            break;

                        case FMT_AUDIO:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            break;
                    }
                    Sem_lock(sem_id);

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MJPG];
                            if (CacheMng_Video_CacheUnlock
                                (msgbuf.frame_info.serial_no, pVidInfo) == 0)
                            {
                                /* No return */
                            }
                            break;
                        case FMT_AUDIO:
                            pVidInfo =
                                &pParm->MemInfo.video_info[AUDIO_INFO_G711];
                            if (CacheMng_Video_CacheUnlock
                                (msgbuf.frame_info.serial_no, pVidInfo) == 0)
                            {
                                /* No return */
                            }
                            break;
                        case FMT_MPEG4:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4];
                            ret =
                                UnlockGopBySerial(msgbuf.frame_info.serial_no,
                                                  pVidInfo, GOP_INDEX_MP4);
                            if (ret == GOP_INCOMPLETE)
                            {
                                pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
                                ret = 0;
                            }
                            if (ret == 0)
                            {
                                /* No return */
                            }
                            break;
                        case FMT_MPEG4_EXT:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT];
                            ret =
                                UnlockGopBySerial(msgbuf.frame_info.serial_no,
                                                  pVidInfo, GOP_INDEX_MP4_EXT);
                            if (ret == GOP_INCOMPLETE)
                            {
                                pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
                                ret = 0;
                            }
                            if (ret == 0)
                            {
                                /* No return */
                            }
                            break;
                    }
                    Sem_unlock(sem_id);
                    break;
                }
                case MSG_CMD_GET_VOL:
                {
                    VIDEO_BLK_INFO *pVidInfo = NULL;

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MPEG4:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4];
                            break;

                        case FMT_MPEG4_EXT:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT];
                            break;

                    }
                    if (pVidInfo == NULL)
                        break;
                    msgbuf.frame_info.offset =
                        (unsigned long) (pVidInfo->extradata) -
                        pParm->MemInfo.start_addr;
                    msgbuf.frame_info.size = pVidInfo->extrasize;

                    // printheader(pVidInfo->extradata,
                    // msgbuf.frame_info.size);

                    break;
                }
                case MSG_CMD_GET_MEDIA_INFO:
                {
                    msgbuf.frame_info.format = 0;          // VIDEO_streamGetMediaInfo();

                    break;
                }
                case MSG_CMD_GET_AND_LOCK_IFRAME:
                {
                    VIDEO_FRAME *pFrame = NULL;

                    VIDEO_BLK_INFO *pVidInfo = NULL;

                    int sem_id = 0;

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_JPEG];
                            break;

                        case FMT_MPEG4:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4];
                            break;

                        case FMT_MPEG4_EXT:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
                            break;

                        case FMT_AUDIO:
                            sem_id = pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            break;
                    }
                    Sem_lock(sem_id);

                    switch (msgbuf.frame_info.format)
                    {
                        case FMT_MJPEG:
                            break;
                        case FMT_MPEG4:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4];
                            pFrame = GetLastI_Frame(pVidInfo);
                            break;

                        case FMT_MPEG4_EXT:
                            pVidInfo =
                                &pParm->MemInfo.video_info[VIDOE_INFO_MP4_EXT];
                            pFrame = GetLastI_Frame(pVidInfo);
                            break;

                    }
                    if (pFrame == NULL)
                    {
                        msgbuf.ret = -1;
                    }
                    else
                    {
                        switch (msgbuf.frame_info.format)
                        {
                            case FMT_MPEG4:
                                msgbuf.frame_info.serial_no = pFrame->serial;
                                msgbuf.frame_info.quality = pParm->Mpeg4Quality;
                                msgbuf.ret =
                                    LockGopBySerial(msgbuf.frame_info.serial_no,
                                                    pVidInfo, GOP_INDEX_MP4);

                                if (msgbuf.ret == GOP_INCOMPLETE)
                                {
                                    msgbuf.ret = GOP_COMPLETE;
                                    pParm->lockNewFrame[GOP_INDEX_MP4] = 1;
                                }
                                break;
                            case FMT_MPEG4_EXT:
                                msgbuf.frame_info.serial_no = pFrame->serial;
                                msgbuf.frame_info.quality = pParm->Mpeg4Quality;
                                msgbuf.ret =
                                    LockGopBySerial(msgbuf.frame_info.serial_no,
                                                    pVidInfo,
                                                    GOP_INDEX_MP4_EXT);

                                if (msgbuf.ret == GOP_INCOMPLETE)
                                {
                                    msgbuf.ret = GOP_COMPLETE;
                                    pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 1;
                                }
                                break;
                        }

                        if (msgbuf.ret == GOP_COMPLETE)
                        {
                            CACHE_DATA_INFO Cache_info;

                            int cnt = 0;

                            CacheMng_GetCacheInfoBySerial(pFrame->serial,
                                                          &Cache_info,
                                                          pVidInfo);
                            msgbuf.frame_info.offset =
                                Cache_info.start_phy -
                                GetMemMngPhyBaseAddr(&pParm->MemInfo);
                            msgbuf.frame_info.size = Cache_info.realsize;
                            switch (msgbuf.frame_info.format)
                            {
                                case FMT_MPEG4:
                                    msgbuf.frame_info.width = pParm->ImageWidth;
                                    msgbuf.frame_info.height =
                                        pParm->ImageHeight;
                                    break;
                                case FMT_MPEG4_EXT:
                                    msgbuf.frame_info.width =
                                        pParm->
                                        ImageWidth_Ext[STREAM_EXT_MP4_CIF];
                                    msgbuf.frame_info.height =
                                        pParm->
                                        ImageHeight_Ext[STREAM_EXT_MP4_CIF];
                                    break;
                            }
                            msgbuf.frame_info.flags = Cache_info.flag;
                            msgbuf.frame_info.frameType = Cache_info.fram_type;
                            msgbuf.frame_info.timestamp = Cache_info.timestamp;
                            msgbuf.frame_info.temporalId =
                                Cache_info.temporalId;
                            for (cnt = 0; cnt < VIDOE_INFO_END; cnt++)
                            {
                                msgbuf.frame_info.ref_serial[cnt] =
                                    Cache_info.ref_serial[cnt];
                            }
                        }
                    }
                    Sem_unlock(sem_id);
                    break;
                }
                case MSG_CMD_SET_DAY_NIGHT:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_AE_MODE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_BACKLIGHT:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_BLC, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_BRIGHTNESS:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_BRIGHTNESS, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_RESOLUTION:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_RESOLUTION, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FRAMERATE:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_FRAMERATE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_CONTRAST:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_CONTRAST, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_SATURATION:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_SATURATION, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_SHARPNESS:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = (temp == 0) ? 1 : temp;
                    stream_feature_setup(STREAM_FEATURE_SHARPNESS, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_WHITE_BALANCE:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_AWB_MODE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_AAC_BITRATE:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_AAC_BITRATE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_BITRATE1:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_BIT_RATE1, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_BITRATE2:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_BIT_RATE2, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_RATE_CONTROL1:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_RATE_CONTROL1, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_RATE_CONTROL2:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_RATE_CONTROL2, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_JPEG_QUALITY:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_JPG_QUALITY, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FRAMERATE1:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_FRAMERATE1, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FRAMERATE2:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_FRAMERATE2, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FRAMERATE3:
                {
                    unsigned int value;

                    value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));
                    stream_feature_setup(STREAM_FEATURE_FRAMERATE3, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_VENDOR_2A:                   /* img2a */
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_AEW_VENDOR, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_TYPE_2A:                     /* img2a */
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_AEW_TYPE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_2A_PRIORITY:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_AEW_PRIORITY, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_IRIS:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_IRIS, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FRAMECTRL:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_FRAMECTRL, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_TV_SYSTEM:               /* ntsc/pal */
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_ENV_50_60HZ, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_BINNING_SKIP:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_BINNING_SKIP, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_LOCALDISPLAY:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_LOCALDISPLAY, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_OSDENABLE:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = (int) temp;
                    stream_feature_setup(STREAM_FEATURE_OSDENABLE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_TSTAMP:
                {
                    int value;

                    unsigned char temp;

                    temp = (unsigned char)(*(unsigned long *)(&msgbuf.mem_info));
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_TSTAMPENABLE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_PTZ:
                {
                    int value = (int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_PTZ, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_MOTION:
                {
                    ApproMotionPrm tMotionPrm;

                    memcpy(&tMotionPrm, &msgbuf.mem_info,
                           sizeof(ApproMotionPrm));
                    stream_feature_setup(STREAM_FEATURE_MOTION, &tMotionPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_LDC_ENABLE:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_LDC_ENABLE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_VNF_PARAM:
                {
                    VnfParam vnfParam;
                    memcpy(&vnfParam, &msgbuf.mem_info, sizeof(VnfParam));
                    stream_feature_setup(STREAM_FEATURE_VNF_PARAM, &vnfParam);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_DYNRANGE:
                {
                    DynRangePrm dynRangePrm;

                    memcpy(&dynRangePrm, &msgbuf.mem_info, sizeof(DynRangePrm));
                    stream_feature_setup(STREAM_FEATURE_DYNRANGE, &dynRangePrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ROICFG:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_ROICFG, &value);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_OSDTEXT_EN:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_OSDTEXT_EN, &value);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_HIST_EN:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_HIST_EN, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_OSDLOGO_EN:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_OSDLOGO_EN, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_FDETECT:
                {
                    FaceDetectParam faceParam;

                    memcpy(&faceParam, &msgbuf.mem_info,
                           sizeof(FaceDetectParam));
                    stream_feature_setup(STREAM_FEATURE_FACE_SETUP, &faceParam);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_DATETIMEPRM:
                {
                    DateTimePrm datetimeParam;

                    memcpy(&datetimeParam, &msgbuf.mem_info,
                           sizeof(DateTimePrm));
                    //stream_feature_setup(STREAM_FEATURE_DATETIMEPRM,
                    //                     &datetimeParam);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_OSDPRM1:
                {
                    OSDPrm osdPrm;

                    memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
                    stream_feature_setup(STREAM_FEATURE_OSDPRM1, &osdPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_OSDPRM2:
                {
                    OSDPrm osdPrm;

                    memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
                    stream_feature_setup(STREAM_FEATURE_OSDPRM2, &osdPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_OSDPRM3:
                {
                    OSDPrm osdPrm;

                    memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
                    stream_feature_setup(STREAM_FEATURE_OSDPRM3, &osdPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_CLICKNAME:
                {
                    OsdTextPrm *pPrm = (OsdTextPrm *) & msgbuf.mem_info;

                    stream_feature_setup(STREAM_FEATURE_CLICKNAME, pPrm);
                    msgbuf.ret = pPrm->nLegnth;
                    break;
                }
                case MSG_CMD_SET_CLICKSTORAGE:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_CLICKSTORAGE, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_IFRAME:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_IFRAME, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_AUDIO_ALARM_LEVEL:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_AUDIO_ALARM_LEVEL,
                                         &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_AUDIO_ALARM_FLAG:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_AUDIO_ALARM_FLAG,
                                         &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_MIRROR:
                {
                    unsigned int value = (unsigned int)(*(unsigned long *)(&msgbuf.mem_info));

                    stream_feature_setup(STREAM_FEATURE_MIRROR, &value);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_ROIPRM1:
                {
                    CodecROIPrm codecROIPrm;

                    memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
                    stream_feature_setup(STREAM_FEATURE_ROIPRM1, &codecROIPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ROIPRM2:
                {
                    CodecROIPrm codecROIPrm;

                    memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
                    stream_feature_setup(STREAM_FEATURE_ROIPRM2, &codecROIPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ROIPRM3:
                {
                    CodecROIPrm codecROIPrm;

                    memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
                    stream_feature_setup(STREAM_FEATURE_ROIPRM3, &codecROIPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ADVCODECPRM1:
                {
                    CodecAdvPrm codecAdvPrm;

                    memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
                    stream_feature_setup(STREAM_FEATURE_ADVCODECPRM1,
                                         &codecAdvPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ADVCODECPRM2:
                {
                    CodecAdvPrm codecAdvPrm;

                    memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
                    stream_feature_setup(STREAM_FEATURE_ADVCODECPRM2,
                                         &codecAdvPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_ADVCODECPRM3:
                {
                    CodecAdvPrm codecAdvPrm;

                    memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
                    stream_feature_setup(STREAM_FEATURE_ADVCODECPRM3,
                                         &codecAdvPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_STREAMSTACK:
                {
                    int value;
                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_STREAMSTACK, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_STOPDEMO:
                {
                    int value;

                    unsigned char temp;

                    temp = *(unsigned char *) (&msgbuf.mem_info);
                    value = temp;
                    stream_feature_setup(STREAM_FEATURE_STOPDEMO, &value);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_MCFW_CREATE_PARAM:
                {
                    McfwCreatePrm mcfwCreatePrm;

                    memcpy(&mcfwCreatePrm, &msgbuf.mem_info,
                           sizeof(McfwCreatePrm));
                    stream_feature_setup(STREAM_FEATURE_MCFW_CREATE_PARAM,
                                         &mcfwCreatePrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_POLLING:
                {
                    msgbuf.ret = 0;
                    break;
                }

                /* DMVA Specific Messages */

                case MSG_CMD_SET_DMVAENABLE:
                {
                    DmvaParam dmvaParam;
                    //FaceDetectParam faceParam;
                    //dmvaParam.dmvaenable = faceParam.fdetect;//Harmeet: Check for DMVA parameters

                    memcpy(&dmvaParam, &msgbuf.mem_info, sizeof(DmvaParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_SETUP, &dmvaParam);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_DMVAALGOFRAMERATE:
                {
                    DmvaParam dmvaParam;

                    memcpy(&dmvaParam, &msgbuf.mem_info, sizeof(DmvaParam));
                    stream_feature_setup(STREAM_FEATURE_DMVAALGOFRAMERATE_SETUP, &dmvaParam);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_DMVAALGODETECTFREQ:
                {
                    DmvaParam dmvaParam;

                    memcpy(&dmvaParam, &msgbuf.mem_info, sizeof(DmvaParam));
                    stream_feature_setup(STREAM_FEATURE_DMVAALGODETECTFREQ_SETUP, &dmvaParam);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_DMVAEVTRECORDENABLE:
                {
                    DmvaParam dmvaParam;

                    memcpy(&dmvaParam, &msgbuf.mem_info, sizeof(DmvaParam));
                    stream_feature_setup(STREAM_FEATURE_DMVAEVTRECORDENABLE_SETUP, &dmvaParam);
                    msgbuf.ret = 0;
                    break;
                }

                case MSG_CMD_SET_DMVASMETATRACKERENABLE:
                {
                    DmvaParam dmvaParam;

                    memcpy(&dmvaParam, &msgbuf.mem_info, sizeof(DmvaParam));
                    stream_feature_setup(STREAM_FEATURE_DMVASMETATRACKERENABLE_SETUP, &dmvaParam);
                    msgbuf.ret = 0;
                    break;
                }

                /* DMVA TZ Specific Messages */

                case MSG_CMD_SET_DMVA_TZ:
                {
                    DmvaTZParam dmvaTZParam;

                    /* Data sharing: Shared memory based */
                    ShareMemRead(msgbuf.mem_info.addr,&dmvaTZParam,sizeof(DmvaTZParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_TZ_SETUP, &dmvaTZParam);
                    msgbuf.ret = 0;
                    break;
                }

                /* DMVA MAIN PAGE Specific Messages */

                case MSG_CMD_SET_DMVA_MAIN:
                {
                    DmvaMainParam dmvaMainParam;

                    ShareMemRead(msgbuf.mem_info.addr,&dmvaMainParam,sizeof(DmvaMainParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_MAIN_SETUP, &dmvaMainParam);
                    msgbuf.ret = 0;
                    break;
                }
                /* DMVA CTD PAGE Specific Messages */

                case MSG_CMD_SET_DMVA_CTD:
                {
                    DmvaCTDParam dmvaCTDParam;

                    memcpy(&dmvaCTDParam, &msgbuf.mem_info, sizeof(DmvaCTDParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_CTD_SETUP, &dmvaCTDParam);
                    msgbuf.ret = 0;
                    break;
                }
                /* DMVA OC PAGE Specific Messages */

                case MSG_CMD_SET_DMVA_OC:
                {
                    DmvaOCParam dmvaOCParam;

                    memcpy(&dmvaOCParam, &msgbuf.mem_info, sizeof(DmvaOCParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_OC_SETUP, &dmvaOCParam);
                    msgbuf.ret = 0;
                    break;
                }
                /* DMVA SMETA PAGE Specific Messages */

                case MSG_CMD_SET_DMVA_SMETA:
                {
                    DmvaSMETAParam dmvaSMETAParam;

                    ShareMemRead(msgbuf.mem_info.addr,&dmvaSMETAParam,sizeof(DmvaSMETAParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_SMETA_SETUP, &dmvaSMETAParam);
                    msgbuf.ret = 0;
                    break;
                }
                /* DMVA IMD PAGE Specific Messages */

                case MSG_CMD_SET_DMVA_IMD:
                {
                    DmvaIMDParam dmvaIMDParam;

                    ShareMemRead(msgbuf.mem_info.addr,&dmvaIMDParam,sizeof(DmvaIMDParam));
                    stream_feature_setup(STREAM_FEATURE_DMVA_IMD_SETUP, &dmvaIMDParam);
                    msgbuf.ret = 0;
                    break;
                }

                /* DMVA Enc Roi Enable */
                case MSG_CMD_SET_DMVA_ENCROI_ENABLE:
                {
                    int enable;

                    memcpy(&enable, &msgbuf.mem_info, sizeof(int));
                    stream_feature_setup(STREAM_FEATURE_DMVA_ENCROI_ENABLE, &enable);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_VIDEO_CODEC_RES:
                {
                    StreamResolution strmPrm;

                    memcpy(&strmPrm, &msgbuf.mem_info, sizeof(StreamResolution));

                    stream_feature_setup(STREAM_FEATURE_SET_CAMERA_RES, &strmPrm);

                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_VIDEO_CODEC_CFG:
                {
                    VideoCodecCfg vidCodecCfgPrm;
					VideoCodecCfg *pVidCodecCfgPrm = (VideoCodecCfg*)&msgbuf.mem_info;

                    memcpy(&vidCodecCfgPrm, &msgbuf.mem_info, sizeof(VideoCodecCfg));

                    stream_feature_setup(STREAM_FEATURE_SET_CAMERA_CODECCFG, &vidCodecCfgPrm);

					pVidCodecCfgPrm->restartFlag = vidCodecCfgPrm.restartFlag;
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_GET_MCFW_INIT_STATUS:
                {
                    McFWInitStatus *pStatus = (McFWInitStatus*)&msgbuf.mem_info;
                    pStatus->initDone = App_getInitStatus();
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_LPR_RECOGNITION_AREA:
                {
                    AlgPolygonArea recogAreaPrm;
                    memcpy(&recogAreaPrm, &msgbuf.mem_info, sizeof(recogAreaPrm));
					OSA_printf("stramRecogArea:%d,%d \n", recogAreaPrm.arr[1].x, recogAreaPrm.arr[2].y);
                    stream_feature_setup(STREAM_FEATURE_SET_LPR_RECOGNITION_AREA, &recogAreaPrm);
                    msgbuf.ret = 0;
                    break;
                }			
                case MSG_CMD_SET_LPR_TRIGGER_INFO:
                {
                    AlgTriggerInfo triggerInfoPrm;
                    memcpy(&triggerInfoPrm, &msgbuf.mem_info, sizeof(triggerInfoPrm));
					OSA_printf("stramTrigInfoArea:%d,%d \n", triggerInfoPrm.trigArea.arr[1].x, triggerInfoPrm.trigArea.arr[2].y);
                    stream_feature_setup(STREAM_FEATURE_SET_LPR_TRIGGER_INFO, &triggerInfoPrm);
                    msgbuf.ret = 0;
                    break;
                }
                case MSG_CMD_SET_LPR_PLATE_WIDTH:
                {
                    AlgPlateWidth plateWidthPrm;
                    memcpy(&plateWidthPrm, &msgbuf.mem_info, sizeof(plateWidthPrm));
					OSA_printf("stramPlateWidht:%d,%d \n", plateWidthPrm.nMax, plateWidthPrm.nMin);
                    stream_feature_setup(STREAM_FEATURE_SET_LPR_PLATE_WIDTH, &plateWidthPrm);
                    msgbuf.ret = 0;
                    break;
                }	
                case MSG_CMD_SET_LPR_DEFAULT_PROVINCE:
                {
                    char szProvince[16];
                    memcpy(&szProvince[0], &msgbuf.mem_info, sizeof(szProvince));
					OSA_printf("stramPlateWidht:%s \n", szProvince);
                    stream_feature_setup(STREAM_FEATURE_SET_LPR_DEFAULT_PROVINCE, &szProvince[0]);
                    msgbuf.ret = 0;
                    break;
                }				
                default:
                    DBG("default case \n");
                    break;
            }
            if (msgbuf.Src != 0)
            {
                /* response */
                msgbuf.Des = msgbuf.Src;
                msgbuf.Src = MSG_TYPE_MSG1;
                msgsnd(qid, &msgbuf, sizeof(msgbuf) - sizeof(long), 0);
            }
        }
    }

    pthread_exit(NULL);
}

/**
 * @brief   Initialize message function
 * @param   "STREAM_PARM *pParm" : stream param
 * @return  none
 */
/* ===================================================================
 *  @func     Init_Msg_Func
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
static int Init_Msg_Func(STREAM_PARM * pParm)
{

    pthread_attr_t attr;

    struct sched_param schedParam;

    /* Initialize the thread attributes */
    if (pthread_attr_init(&attr))
    {
        ERR("Failed to initialize thread attrs\n");
        return STREAM_FAILURE;
    }

    if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO))
    {
        ERR("Failed to pthread_attr_setschedpolicy\n");
        return STREAM_FAILURE;
    }

    if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO))
    {
        ERR("Failed to setschedpolicy\n");
        return STREAM_FAILURE;
    }

    /* Set the capture thread priority */
    pthread_attr_getschedparam(&attr, &schedParam);;
    schedParam.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
    if (pthread_attr_setschedparam(&attr, &schedParam))
    {
        ERR("Failed to setschedparam\n");
        return STREAM_FAILURE;
    }
    DBG(" Init_Msg_Func \n");
    // ShowInfo(pParm, 0);
    pthread_create(&(pParm->threadControl), &attr, Msg_CTRL, (void *) pParm);

    return STREAM_SUCCESS;
}

/**
 * @brief   Setup stream feature
 * @param   "int nFeature" : feature select
 * @param   "STREAM_PARM *pParm" : input value
 * @return  none
 */
/* ===================================================================
 *  @func     stream_feature_setup
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
void stream_feature_setup(int nFeature, void *pParm)
{
    static int baseFrameRate=60*1000;
    Vsys_swOsdPrm swosdGuiPrm;

    Vsys_fdPrm fdGuiPrm;
    VaLink_GuiParams vaGuiPrm;
    AlgVehicleLink_ThPlateIdDynParams lprDynPrm;
    int i;

    if (nFeature < 0 || nFeature >= STREAM_FEATURE_NUM)
        return;

    switch (nFeature)
    {
        case STREAM_FEATURE_BIT_RATE1:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.targetBitRate = *(int *) pParm;
            Venc_setDynamicParam(0, 0, &params, VENC_BITRATE);

            swosdGuiPrm.streamId = 0;
            swosdGuiPrm.bitRate = params.targetBitRate;
            Vsys_setSwOsdPrm(VSYS_SWOSDBR, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_BIT_RATE2:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.targetBitRate = *(int *) pParm;
            Venc_setDynamicParam(1, 0, &params, VENC_BITRATE);

            swosdGuiPrm.streamId = 1;
            swosdGuiPrm.bitRate = params.targetBitRate;
            Vsys_setSwOsdPrm(VSYS_SWOSDBR, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_RATE_CONTROL1:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.rateControl = *(int *) pParm;
            Venc_setDynamicParam(0, 0, &params, VENC_RATECONTROL);

            swosdGuiPrm.streamId = 0;
            swosdGuiPrm.rateControl = params.rateControl;
            Vsys_setSwOsdPrm(VSYS_SWOSDRC, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_RATE_CONTROL2:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.rateControl = *(int *) pParm;
            Venc_setDynamicParam(1, 0, &params, VENC_RATECONTROL);

            swosdGuiPrm.streamId = 1;
            swosdGuiPrm.rateControl = params.rateControl;
            Vsys_setSwOsdPrm(VSYS_SWOSDRC, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_JPG_QUALITY:
        {
            unsigned int qpval = *(int *) pParm;
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            /* 0=> VBR, 1=> CBR, 2=> OFF */
#ifdef MJPEG_VBR_MODE
            params.rateControl = RATECONTROL_VBR;
#elif defined MJPEG_CBR_MODE
            params.rateControl = RATECONTROL_CBR;
#else
            params.rateControl = RATECONTROL_OFF;
#endif
            if(params.rateControl == RATECONTROL_OFF)
            {
                params.qpMin    = 1;
                params.qpMax    = 51;
                params.qpInit   = qpval;
                Venc_setDynamicParam(2, 0, &params, VENC_QPVAL_I);
                Venc_setDynamicParam(2, 0, &params, VENC_RATECONTROL);

                OSA_printf("MJPEG in Rate Control OFF Mode\n");
            }
            else
            {
                /* this values are as an example, since web based GUI interface is not available */
                params.targetBitRate = 8 * 1000 * 1000;

                Venc_setDynamicParam(2, 0, &params, VENC_RATECONTROL);
                Venc_setDynamicParam(2, 0, &params, VENC_BITRATE);

                /* if the QP param needs to be tuned, this needs to be used */

                params.qpMin    = 1;
                params.qpMax    = 51;
                params.qpInit   = 28;
                Venc_setDynamicParam(2, 0, &params, VENC_QPVAL_I);

#ifdef MJPEG_CBR_MODE
                OSA_printf("MJPEG in Rate Control CBR Mode\n");
#else
                OSA_printf("MJPEG in Rate Control VBR Mode\n");
#endif
            }

            break;
        }
        case STREAM_FEATURE_FRAMERATE1:
        {
            VCAM_CHN_DYNAMIC_PARAM_S params_camera = { 0 };
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.frameRate = (*(int *) pParm)/1000;
            Venc_setInputFrameRate(0, params.frameRate);
            Venc_setDynamicParam(0, 0, &params, VENC_FRAMERATE);

            params_camera.frameRate = *(int *) pParm;
            Vcam_setDynamicParamChn(0, &params_camera, VCAM_FRAMERATE);

            swosdGuiPrm.streamId = 0;
            swosdGuiPrm.frameRate = params_camera.frameRate / 1000;
            Vsys_setSwOsdPrm(VSYS_SWOSDFR, &swosdGuiPrm);

            if ((params_camera.frameRate /1000 ) == 30)
                params_camera.frameCtrl = 30;
            else if ((params_camera.frameRate /1000 ) == 60)
                params_camera.frameCtrl = 60;
            else
                params_camera.frameCtrl = 60;

            baseFrameRate = params_camera.frameRate;
            Vcam_setDynamicParamChn(0, &params_camera, VCAM_FRAMECTRL);
            OSA_printf("\n\n========== Stream FrameRate1 = %d =================\n", params.frameRate);
            Venc_setInputFrameRate(2, params.frameRate);
            break;
        }
        case STREAM_FEATURE_FRAMERATE2:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };
            VCAM_CHN_DYNAMIC_PARAM_S params_camera = { 0 };

            params.frameRate = (*(int *) pParm)/1000;
            Venc_setInputFrameRate(1, params.frameRate);
            Venc_setDynamicParam(1, 0, &params, VENC_FRAMERATE);
            params_camera.frameRate = *(int *) pParm;
            Vcam_setDynamicParamChn(1, &params_camera, VCAM_FRAMERATE);

            swosdGuiPrm.streamId = 1;
            swosdGuiPrm.frameRate = params_camera.frameRate / 1000;
            Vsys_setSwOsdPrm(VSYS_SWOSDFR, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_FRAMERATE3:
        {
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.frameRate = (*(int *) pParm)/1000;
            Venc_setDynamicParam(2, 0, &params, VENC_FRAMERATE);

            break;
        }
        case STREAM_FEATURE_ADVCODECPRM1:
        {
            CodecAdvPrm *codecAdvPrm = (CodecAdvPrm *) pParm;
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            /* leave qp parameter changes of I-frame unchanged*/
            params.qpMin    = 10;
            params.qpMax    = 36;
            params.qpInit   = -1;
            Venc_setDynamicParam(0, 0, &params, VENC_QPVAL_I);

            params.intraFrameInterval   = codecAdvPrm->ipRatio;
            params.qpMin                = codecAdvPrm->qpMin;
            params.qpMax                = codecAdvPrm->qpMax;
            params.qpInit               = codecAdvPrm->qpInit;
            params.packetSize           = codecAdvPrm->packetSize;
            params.encPreset            = codecAdvPrm->meConfig;

            Venc_setDynamicParam(0, 0, &params, VENC_IPRATIO);
            if (codecAdvPrm->fIframe)
            {
                Venc_forceIDR(0, 0);
            }
            Venc_setDynamicParam(0, 0, &params, VENC_QPVAL_P);
            Venc_setDynamicParam(0, 0, &params, VENC_PACKETSIZE);
            Venc_setDynamicParam(0, 0, &params, VENC_ENCPRESET);

            break;
        }
        case STREAM_FEATURE_ADVCODECPRM2:
        {
            CodecAdvPrm *codecAdvPrm = (CodecAdvPrm *) pParm;
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            /* leave qp parameter changes of I-frame unchanged*/
            params.qpMin    = 10;
            params.qpMax    = 36;
            params.qpInit   = -1;
            Venc_setDynamicParam(1, 0, &params, VENC_QPVAL_I);

            params.intraFrameInterval   = codecAdvPrm->ipRatio;
            params.qpMin                = codecAdvPrm->qpMin;
            params.qpMax                = codecAdvPrm->qpMax;
            params.qpInit               = codecAdvPrm->qpInit;
            params.packetSize           = codecAdvPrm->packetSize;
            params.encPreset            = codecAdvPrm->meConfig;

            Venc_setDynamicParam(1, 0, &params, VENC_IPRATIO);
            if (codecAdvPrm->fIframe)
            {
                Venc_forceIDR(1, 0);
            }
            Venc_setDynamicParam(1, 0, &params, VENC_QPVAL_P);
            Venc_setDynamicParam(1, 0, &params, VENC_PACKETSIZE);
            Venc_setDynamicParam(1, 0, &params, VENC_ENCPRESET);

            break;
        }
        case STREAM_FEATURE_ADVCODECPRM3:
        {
            break;
        }
        case STREAM_FEATURE_ROIPRM1:
        {
            CodecROIPrm *codecROIPrm = (CodecROIPrm *) pParm;
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.roiNumOfRegion = codecROIPrm->numROI;
            int i = 0;

            for (i = 0; i < codecROIPrm->numROI; i++)
            {
                params.roiStartX[i] = codecROIPrm->roi[i].startx;
                params.roiStartY[i] = codecROIPrm->roi[i].starty;
                params.roiWidth[i] = codecROIPrm->roi[i].width;
                params.roiHeight[i] = codecROIPrm->roi[i].height;
                params.roiType[i] = codecROIPrm->roi[i].type;
            }

            //if(codecROIPrm->numROI) disable ROI also need this
            Venc_setDynamicParam(0, 0, &params, VENC_ROI);

            break;
        }
        case STREAM_FEATURE_ROIPRM2:
        {
            CodecROIPrm *codecROIPrm = (CodecROIPrm *) pParm;
            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.roiNumOfRegion = codecROIPrm->numROI;
            int i = 0;

            for (i = 0; i < codecROIPrm->numROI; i++)
            {
                params.roiStartX[i] = codecROIPrm->roi[i].startx;
                params.roiStartY[i] = codecROIPrm->roi[i].starty;
                params.roiWidth[i] = codecROIPrm->roi[i].width;
                params.roiHeight[i] = codecROIPrm->roi[i].height;
                params.roiType[i] = codecROIPrm->roi[i].type;
            }

            //if(codecROIPrm->numROI)  disable ROI also need this
            Venc_setDynamicParam(1, 0, &params, VENC_ROI);

            break;
        }
        case STREAM_FEATURE_ROIPRM3:
        {
            // VIDEO_streamROIPrm((CodecROIPrm*)pParm, 2);
            break;
        }
        case STREAM_FEATURE_AAC_BITRATE:
        {
            // int input_val = *(int *)pParm;
            // AUDIO_aac_bitRate_setparm(input_val);

            break;
        }
        case STREAM_FEATURE_SHARPNESS:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.sharpness = *(int *) pParm;
            Aew_ext_parameter.sharpness = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_SHARPNESS);

            break;
        }

        case STREAM_FEATURE_CONTRAST:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.contrast = *(int *) pParm;
            Aew_ext_parameter.contrast = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_CONTRAST);

            break;
        }
        case STREAM_FEATURE_BRIGHTNESS:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.brightness = *(int *) pParm;

            Aew_ext_parameter.brightness = input_val;

            Vcam_setDynamicParamChn(0, &params, VCAM_BRIGHTNESS);

            break;
        }
        case STREAM_FEATURE_BLC:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            if (input_val == 0)
            {
                Aew_ext_parameter.blc = BACKLIGHT_LOW;
                params.blc = BACKLIGHT_LOW;
            }
            else if (input_val == 1)
            {
                Aew_ext_parameter.blc = BACKLIGHT_NORMAL;
                params.blc = BACKLIGHT_NORMAL;
            }
            else if (input_val == 2)
            {
                Aew_ext_parameter.blc = BACKLIGHT_HIGH;
                params.blc = BACKLIGHT_HIGH;
            }
            else
            {
                Aew_ext_parameter.blc = BACKLIGHT_NORMAL;
                params.blc = BACKLIGHT_NORMAL;
            }

            Vcam_setDynamicParamChn(0, &params, VCAM_BLC);

            break;
        }

        case STREAM_FEATURE_SATURATION:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.saturation = *(int *) pParm;
            Aew_ext_parameter.saturation = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_SATURATION);

            break;
        }

        case STREAM_FEATURE_AWB_MODE:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            if (input_val == 0)
            {
                Aew_ext_parameter.awb_mode = AWB_AUTO;
                params.awbMode = AWB_AUTO;
            }
            else if (input_val == 1)
            {
                Aew_ext_parameter.awb_mode = Day_D65;
                params.awbMode = Day_D65;
            }
            else if (input_val == 2)
            {
                Aew_ext_parameter.awb_mode = Day_D55;
                params.awbMode = Day_D55;
            }
            else if (input_val == 3)
            {
                Aew_ext_parameter.awb_mode = Florescent;
                params.awbMode = Florescent;
            }
            else if (input_val == 4)
            {
                Aew_ext_parameter.awb_mode = Incandescent;
                params.awbMode = Incandescent;
            }
            else
            {
                Aew_ext_parameter.awb_mode = AWB_AUTO;
                params.awbMode = AWB_AUTO;
            }

            Vcam_setDynamicParamChn(0, &params, VCAM_AWBMODE);

            break;
        }

        case STREAM_FEATURE_AE_MODE:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            params.AEMode = *(int *) pParm;
            if (Aew_ext_parameter.SENSOR_FRM_RATE_SETUP)
                Aew_ext_parameter.SENSOR_FRM_RATE_SETUP(0);
            Aew_ext_parameter.sensor_frame_rate = 0;
            if (input_val == 0)
            {
                Aew_ext_parameter.day_night = AE_NIGHT;
                params.AEMode = AE_NIGHT;
            }
            else if (input_val == 1)
            {
                Aew_ext_parameter.day_night = AE_DAY;
                params.AEMode = AE_DAY;
            }
            else
            {
                Aew_ext_parameter.day_night = AE_NIGHT;
                params.AEMode = AE_NIGHT;
            }
            Vcam_setDynamicParamChn(0, &params, VCAM_AEMODE);

            break;
        }
        case STREAM_FEATURE_AEW_VENDOR:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.aewbVendor = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_AEWB_VENDOR);

            break;
        }
        case STREAM_FEATURE_AEW_TYPE:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.aewbMode = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_AEWB_MODE);

            break;
        }
        case STREAM_FEATURE_AEW_PRIORITY:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.aewbPriority = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_AEWB_PRIORITY);

            break;
        }
        case STREAM_FEATURE_IRIS:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.IRIS = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_IRIS);

            break;
        }
        case STREAM_FEATURE_FRAMECTRL:
        {
/*            int input_val = *(int *) pParm;

            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            if (input_val == 0)
                params.frameCtrl = 30;
            else if (input_val == 1)
                params.frameCtrl = 60;
            else
                params.frameCtrl = 60;

            Vcam_setDynamicParamChn(0, &params, VCAM_FRAMECTRL);*/

            break;
        }
        case STREAM_FEATURE_ENV_50_60HZ:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            if (input_val == 0)
            {
                Aew_ext_parameter.env_50_60Hz = VIDEO_NTSC;
                params.env50_60hz = VIDEO_NTSC;
            }
            else if (input_val == 1)
            {
                Aew_ext_parameter.env_50_60Hz = VIDEO_PAL;
                params.env50_60hz = VIDEO_PAL;
            }
            else
            {
                Aew_ext_parameter.env_50_60Hz = VIDEO_NTSC;
                params.env50_60hz = VIDEO_NTSC;
            }
            Vcam_setDynamicParamChn(0, &params, VCAM_ENV_50_60HZ);

            break;
        }
        case STREAM_FEATURE_BINNING_SKIP:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            if (input_val == 0)
            {
                Aew_ext_parameter.binning_mode = SENSOR_BINNING;
                params.binningMode = SENSOR_BINNING;
            }
            else if (input_val == 1)
            {
                Aew_ext_parameter.binning_mode = SENSOR_SKIP;
                params.binningMode = SENSOR_SKIP;
            }
            else
            {
                Aew_ext_parameter.binning_mode = SENSOR_BINNING;
                params.binningMode = SENSOR_BINNING;
            }
            Vcam_setDynamicParamChn(0, &params, VCAM_BINNING_SKIP);

            break;
        }
        case STREAM_FEATURE_LOCALDISPLAY:
        {
#if 0//modified by niqi, cancel display       
            int input_val = *(int *)pParm;

                if(input_val==0)
				{
					Vdis_startDevAll();
                    Vsys_switchFormatSD(SYSTEM_STD_NTSC);
				}
                else if(input_val==1)
				{
					Vdis_startDevAll();
                    Vsys_switchFormatSD(SYSTEM_STD_PAL);
				}
				else
				{
					Vdis_stopDevAll();
				}	
#endif
            break;
        }
        case STREAM_FEATURE_OSDENABLE:
        {
            // int input_val = *(int *)pParm;
            // VIDEO_streamSetOSDEnable(input_val);
            break;
        }

        case STREAM_FEATURE_TSTAMPENABLE:
        {
            // int input_val = *(int *)pParm;
            // SetDrawDateTimeStatus(input_val);
            break;
        }
        case STREAM_FEATURE_PTZ:
        {
            // int input_val = *(int *)pParm;
            // SetPtzCmd(input_val);
            break;
        }
        case STREAM_FEATURE_MOTION:
        {
            // VIDEO_motion_setparm((ApproMotionPrm*) pParm);
            ApproMotionPrm *pMotionPrm = (ApproMotionPrm *) pParm;

            VENC_CHN_DYNAMIC_PARAM_S params = { 0 };

            params.bMotionEnable = pMotionPrm->bMotionEnable;
            params.bMotionCEnale = pMotionPrm->bMotionCEnale;
            params.MotionLevel = pMotionPrm->MotionLevel;
            params.MotionCValue = pMotionPrm->MotionCValue;
            params.MotionBlock = pMotionPrm->MotionBlock;

            Venc_setDynamicParam(0, 0, &params, VENC_MDPARAM);

            break;
        }
         case STREAM_FEATURE_LDC_ENABLE:
        {
            unsigned int input_val = *(unsigned int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

			/* LDC is supported only in ISS NF mode */
            if(gUI_mcfw_config.noisefilterMode == ISS_VNF_ON)
			{
                params.enableLDC = input_val;
                if (gUI_mcfw_config.ldcEnable != params.enableLDC)
                {
                    gUI_mcfw_config.ldcEnable = params.enableLDC;
                }
            }

            break;
        }
       case STREAM_FEATURE_VNF_PARAM:
        {
            VnfParam* pPrm = (VnfParam*)pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            if(pPrm->enable)
            {
                params.enableTNF = ((pPrm->mode!=1)&&(pPrm->mode!=4)) ? 1 : 0;
                //params.enableTNF = ((pPrm->mode!=1)&&(pPrm->mode!=4)&&(pPrm->mode!=6)) ? 1 : 0;
                if (gUI_mcfw_config.tnfEnable != params.enableTNF)
                {
                    gUI_mcfw_config.tnfEnable = params.enableTNF;
                    if(gUI_mcfw_config.ldcEnable == 0)
						Vcam_setDynamicParamChn(0, &params, VCAM_TNF);
                }

                params.enableSNF = ((pPrm->mode!=0)&&(pPrm->mode!=3)) ? 1 : 0;
                if (gUI_mcfw_config.snfEnable != params.enableSNF)
                {
                    gUI_mcfw_config.snfEnable = params.enableSNF;
					if(gUI_mcfw_config.ldcEnable == 0)
						Vcam_setDynamicParamChn(0, &params, VCAM_SNF);
                }

                gUI_mcfw_config.vnfStrength = pPrm->strength;
                params.nfStrength = pPrm->strength;
				if(gUI_mcfw_config.ldcEnable == 0)
					Vcam_setDynamicParamChn(0, &params, VCAM_NF_STRENGTH);
            }
            else {
                gUI_mcfw_config.snfEnable = 0;
				if(gUI_mcfw_config.ldcEnable == 0)
					Vcam_setDynamicParamChn(0, &params, VCAM_SNF);

                gUI_mcfw_config.tnfEnable = 0;
				if(gUI_mcfw_config.ldcEnable == 0)
					Vcam_setDynamicParamChn(0, &params, VCAM_TNF);
            }

			/* When LDC is enabled then don't switch to DSS NF */
			if(gUI_mcfw_config.ldcEnable == 0)
			{
				if((pPrm->mode >= 0) && (pPrm->mode <= 2))
				{
					/* DSS VNF mode */
					Vsys_setNoiseFilter(0,DSS_VNF_ON);
				}

				if((pPrm->mode >= 3) && (pPrm->mode <= 5))
				{
					/* ISS VNF mode */
					Vsys_setNoiseFilter(0,ISS_VNF_ON);
				}
			}

            OSA_printf("VNF MESSAGE RECIEVED: (Enable:%d) (Mode:%d) (Strength:%d) (EnableTNF:%d) (EnableSNF:%d)\n",
                            pPrm->enable, pPrm->mode, pPrm->strength, params.enableTNF, params.enableSNF);

            break;
        }
        case STREAM_FEATURE_ROICFG:
        {
            // unsigned int input_val = *(unsigned int *)pParm;
            // SetROICfgEnable(input_val);
            break;
        }
        case STREAM_FEATURE_OSDTEXT_EN:
        {
            // int input_val = *(int *)pParm;
            // SetOsdTextEnable(input_val);
            break;
        }
        case STREAM_FEATURE_HIST_EN:
        {
            int input_val = *(int *)pParm;

            swosdGuiPrm.histEnable = input_val;
            Vsys_setSwOsdPrm(VSYS_SWOSDHISTEN, &swosdGuiPrm);
            break;
        }
        case STREAM_FEATURE_OSDLOGO_EN:
        {
            // int input_val = *(int *)pParm;
            // SetOsdLogoEnable(input_val);
            break;
        }
        case STREAM_FEATURE_FACE_SETUP:
        {
            // VIDEO_streamSetFace((FaceDetectParam*)pParm);
            FaceDetectParam *pFacePrm = (FaceDetectParam *) pParm;

            fdGuiPrm.fdetect = pFacePrm->fdetect;
            fdGuiPrm.startX = (pFacePrm->startX * 100) / 1920;
            fdGuiPrm.startY = (pFacePrm->startY * 100) / 1080;
            fdGuiPrm.width = (pFacePrm->width * 100) / 1920;
            fdGuiPrm.height = (pFacePrm->height * 100) / 1080;
            fdGuiPrm.fdconflevel = pFacePrm->fdconflevel;
            fdGuiPrm.fddirection = pFacePrm->fddirection;
            fdGuiPrm.pmask = pFacePrm->pmask;

            if ((Vsys_getSystemUseCase() == VSYS_USECASE_MULTICHN_TRISTREAM_FULLFTR) ||
                (Vsys_getSystemUseCase() == VSYS_USECASE_TRISTREAM_SMARTANALYTICS))
            {
                Vsys_setFdPrm(&fdGuiPrm);
            }

            break;
        }
        case STREAM_FEATURE_DATETIMEPRM:
		case STREAM_FEATURE_AUX_DATETIMEPRM:
		case STREAM_FEATURE_LPRINFOPRM:
        {
			Vsys_swOsdPrm *pswosdGuiPrm_datetime;
			TOSDPrm *pPrm_datetime = (TOSDPrm *) pParm;
			UInt32 cmdID = 0;
			pswosdGuiPrm_datetime = ( Vsys_swOsdPrm *)malloc(sizeof(Vsys_swOsdPrm));
			pswosdGuiPrm_datetime->streamId = 0;
			pswosdGuiPrm_datetime->dateEnable = pPrm_datetime->dateEnable;
			pswosdGuiPrm_datetime->timeEnable = pPrm_datetime->timeEnable;
			pswosdGuiPrm_datetime->logoEnable = pPrm_datetime->logoEnable;
			pswosdGuiPrm_datetime->logoPos = pPrm_datetime->logoPos;
			pswosdGuiPrm_datetime->textEnable = pPrm_datetime->textEnable;
			pswosdGuiPrm_datetime->textPos = pPrm_datetime->textPos;
			pswosdGuiPrm_datetime->detailedInfo = pPrm_datetime->detailedInfo;
			pswosdGuiPrm_datetime->osd_bmp_num = pPrm_datetime->bmpNum;
			pswosdGuiPrm_datetime->bmp = (TOSD_Char_sys *)pPrm_datetime->bmp;
			if(pswosdGuiPrm_datetime->bmp == NULL){
				OSA_ERROR("pswosdGuiPrm_datetime->bmp == NULL !\n");
			}
			switch(nFeature){
				case STREAM_FEATURE_DATETIMEPRM:
					cmdID = VSYS_OSD_DATETIME_BMP;
					break;
				case STREAM_FEATURE_AUX_DATETIMEPRM:
					cmdID = VSYS_OSD_AUX_DATETIME_BMP;
					break;
				case STREAM_FEATURE_LPRINFOPRM:
					cmdID = VSYS_OSD_LPRINFO_BMP;
					break;
				default:
					break;
			}
			Vsys_setSwOsdBmp(cmdID, &pswosdGuiPrm_datetime);
			pswosdGuiPrm_datetime =NULL;
			break;
        }
		case STREAM_FEATURE_OSD_USERTEXT:
		case STREAM_FEATURE_OSD_USERTEXT_AUX:
		case STREAM_FEATURE_OSD_USERTEXT_LPRINFO:
        {
			Vsys_swOsdPrm *pswosdGuiPrm_usertext;
			TOSDPrm *pPrm_datetime = (TOSDPrm *) pParm;
			UInt32 cmdID = 0;
			pswosdGuiPrm_usertext = ( Vsys_swOsdPrm *)malloc(sizeof(Vsys_swOsdPrm));
			pswosdGuiPrm_usertext->streamId = 0;
			pswosdGuiPrm_usertext->dateEnable = pPrm_datetime->dateEnable;
			pswosdGuiPrm_usertext->timeEnable = pPrm_datetime->timeEnable;
			pswosdGuiPrm_usertext->logoEnable = pPrm_datetime->logoEnable;
			pswosdGuiPrm_usertext->logoPos = pPrm_datetime->logoPos;
			pswosdGuiPrm_usertext->textEnable = pPrm_datetime->textEnable;
			pswosdGuiPrm_usertext->textPos = pPrm_datetime->textPos;
			pswosdGuiPrm_usertext->detailedInfo = pPrm_datetime->detailedInfo;
			pswosdGuiPrm_usertext->osd_bmp_num = pPrm_datetime->bmpNum;
			pswosdGuiPrm_usertext->bmp = (TOSD_Char_sys *)pPrm_datetime->bmp;
			if(pswosdGuiPrm_usertext->bmp == NULL){
				OSA_ERROR("pswosdGuiPrm_usertext->bmp == NULL !\n");
			}
			switch(nFeature){
				case STREAM_FEATURE_OSD_USERTEXT:
					cmdID = VSYS_OSD_USERTEXT_BMP;
					break;
				case STREAM_FEATURE_OSD_USERTEXT_AUX:
					cmdID = VSYS_OSD_AUX_USERTEXT_BMP;
					break;
				case STREAM_FEATURE_OSD_USERTEXT_LPRINFO:
					cmdID = VSYS_OSD_LPRINFO_USERTEXT_BMP;
					break;
			}
			Vsys_setSwOsdBmp(cmdID, &pswosdGuiPrm_usertext);
			pswosdGuiPrm_usertext =NULL;
			break;
        }


        case STREAM_FEATURE_OSDPRM1:
        {
            OSDPrm *pPrm = (OSDPrm *) pParm;

            // VIDEO_streamOsdPrm((OSDPrm*)pParm, 0);

            swosdGuiPrm.streamId = 0;
            swosdGuiPrm.transparencyEnable = 0;  //pPrm->transparencyEnable;
            //Would be read from GUI later from the New GUI


            swosdGuiPrm.dateEnable = pPrm->dateEnable;
            swosdGuiPrm.timeEnable = pPrm->timeEnable;
            swosdGuiPrm.logoEnable = pPrm->logoEnable;
            swosdGuiPrm.logoPos = pPrm->logoPos;
            swosdGuiPrm.textEnable = pPrm->textEnable;
            swosdGuiPrm.textPos = pPrm->textPos;
            swosdGuiPrm.detailedInfo = pPrm->detailedInfo;
            swosdGuiPrm.pUsrString = (UInt8 *) pPrm->text;

            Vsys_setSwOsdPrm(VSYS_SWOSDGUIPRM, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_OSDPRM2:
        {
            OSDPrm *pPrm = (OSDPrm *) pParm;

            // VIDEO_streamOsdPrm((OSDPrm*)pParm, 1);

            swosdGuiPrm.streamId = 1;
            swosdGuiPrm.transparencyEnable = 0;  //pPrm->transparencyEnable;
            //Would be read from GUI later from the New GUI


            swosdGuiPrm.dateEnable = pPrm->dateEnable;
            swosdGuiPrm.timeEnable = pPrm->timeEnable;
            swosdGuiPrm.logoEnable = pPrm->logoEnable;
            swosdGuiPrm.logoPos = pPrm->logoPos;
            swosdGuiPrm.textEnable = pPrm->textEnable;
            swosdGuiPrm.textPos = pPrm->textPos;
            swosdGuiPrm.detailedInfo = pPrm->detailedInfo;
            swosdGuiPrm.pUsrString = (UInt8 *) pPrm->text;

            Vsys_setSwOsdPrm(VSYS_SWOSDGUIPRM, &swosdGuiPrm);

            break;
        }
        case STREAM_FEATURE_OSDPRM3:
        {
            // OSDPrm* pPrm = (OSDPrm*)pParm;
            // VIDEO_streamOsdPrm((OSDPrm*)pParm, 2);
            break;
        }
        case STREAM_FEATURE_CLICKNAME:
        {
            OsdTextPrm *pPrm = (OsdTextPrm *) pParm;

            memcpy((char *) snapFileName, (char *) pPrm->strText,
                   pPrm->nLegnth);
            snapEnable = 1;
            break;
        }
        case STREAM_FEATURE_CLICKSTORAGE:
        {
            // int input_val = *(int *)pParm;
            // SetSnapLocation(input_val);
            break;
        }
        case STREAM_FEATURE_IFRAME:
        {
            int input_val = *(int *) pParm;

            if ((input_val) && (Vsys_getSystemUseCase() != VSYS_USECASE_DUALCHN_DISPLAY))
            {
                Venc_forceIDR(0, 0);
                Venc_forceIDR(1, 0);
            }

            break;
        }
        case STREAM_FEATURE_AUDIO_ALARM_LEVEL:
        {
            int input_val = *(int *)pParm;
            AUDIO_alarmLevel(input_val);
            break;
        }
        case STREAM_FEATURE_AUDIO_ALARM_FLAG:
        {
            int input_val = *(int *)pParm;
            AUDIO_alarmFlag(input_val);
            break;
        }
        case STREAM_FEATURE_STREAMSTACK:
        {
            int input_val = *(int *) pParm;

            gUI_mcfw_config.demoCfg.usecase_id = input_val;

            break;
        }
        case STREAM_FEATURE_MIRROR:
        {
            int input_val = *(int *) pParm;
            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };
            /* mode 4 not used, mode 5 = 270 rotation, mode 6 = 90 rotation*/
            if (input_val >= 4)
                input_val++;
            params.mirrorMode = input_val;
            Vcam_setDynamicParamChn(0, &params, VCAM_MIRROR_MODE);
            break;
        }
        case STREAM_FEATURE_DYNRANGE:
        {
            DynRangePrm* pPrm = (DynRangePrm*)pParm;

            int enable = (unsigned int)pPrm->enable;
            gUI_mcfw_config.glbceStrength = (unsigned int)pPrm->level;

            VCAM_CHN_DYNAMIC_PARAM_S params = { 0 };

            if((gUI_mcfw_config.glbceEnable)&&(enable))
            {
                /* change the glbceEnable definition to the enum of glbcePresets as defined in glbceLink_glbce.h */
                if((unsigned int)pPrm->mode==2)
                    params.enableGlbce = 4+gUI_mcfw_config.glbceStrength;
                else if((unsigned int)pPrm->mode==1)
                    params.enableGlbce = 1+gUI_mcfw_config.glbceStrength;
                else
                    params.enableGlbce = 0;

                if (gUI_mcfw_config.glbceEnable != params.enableGlbce)
                {
                    gUI_mcfw_config.glbceEnable = params.enableGlbce;
                    Vcam_setDynamicParamChn(0, &params, VCAM_GLBCE);
                }
            }

            break;
        }
        case STREAM_FEATURE_STOPDEMO:
        {
            int input_val = *(int *) pParm;

            if (input_val)
            {
                gUI_mcfw_config.demoCfg.stopDemo = TRUE;
                gUI_mcfw_config.demoCfg.unloadDemo = TRUE;
                gUI_mcfw_config.demoCfg.delImgTune = TRUE;
            }

            break;
        }
        case STREAM_FEATURE_MCFW_CREATE_PARAM:
        {
            McfwCreatePrm *mcfwCreatePrm = (McfwCreatePrm *) pParm;

            gUI_mcfw_config.demoCfg.usecase_id = mcfwCreatePrm->usecase_id;
            gUI_mcfw_config.audioCfg.enable = mcfwCreatePrm->audioCreatePrm.enable;
            gUI_mcfw_config.audioCfg.sampleRate =
                mcfwCreatePrm->audioCreatePrm.samplingRate;
            gUI_mcfw_config.vsEnable = mcfwCreatePrm->advFtrCreatePrm.vsEnable;
            gUI_mcfw_config.glbceEnable = mcfwCreatePrm->advFtrCreatePrm.glbceEnable;
            gUI_mcfw_config.ldcEnable =
                mcfwCreatePrm->advFtrCreatePrm.ldcEnable;
            gUI_mcfw_config.snfEnable =
                mcfwCreatePrm->advFtrCreatePrm.snfEnable;
            gUI_mcfw_config.tnfEnable =
                mcfwCreatePrm->advFtrCreatePrm.tnfEnable;
            gUI_mcfw_config.n2A_vendor = mcfwCreatePrm->vendor2A;
            gUI_mcfw_config.n2A_mode = mcfwCreatePrm->mode2A;
            gUI_mcfw_config.StreamPreset[0] = mcfwCreatePrm->StreamPreset[0];
            gUI_mcfw_config.StreamPreset[1] = mcfwCreatePrm->StreamPreset[1];
            gUI_mcfw_config.demoUseCase = mcfwCreatePrm->demoId;
            gUI_mcfw_config.vaUseCase = (mcfwCreatePrm->demoId == VA_DEMO) ? TRUE : FALSE;

            break;
        }

        /* DMVA Specific parameters */
        case STREAM_FEATURE_DMVA_SETUP:
        {
            DmvaParam *pDmvaParam = (DmvaParam*)pParm;

            vaGuiPrm.dmvaenable = pDmvaParam->dmvaenable;
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUP,&vaGuiPrm);

            break;
        }
        case STREAM_FEATURE_DMVAALGOFRAMERATE_SETUP:
        {
            DmvaParam *pDmvaParam = (DmvaParam*)pParm;

            vaGuiPrm.dmvaexptalgoframerate = pDmvaParam->dmvaexptalgoframerate;
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPALGOFRAMERATE,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVAALGODETECTFREQ_SETUP:
        {
            DmvaParam *pDmvaParam = (DmvaParam*)pParm;

            vaGuiPrm.dmvaexptalgodetectfreq = pDmvaParam->dmvaexptalgodetectfreq;
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPALGODETECTFREQ,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVAEVTRECORDENABLE_SETUP:
        {
            DmvaParam *pDmvaParam = (DmvaParam*)pParm;

            vaGuiPrm.dmvaexptevtrecordenable = pDmvaParam->dmvaexptevtrecordenable;
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPEVTRECORDENABLE,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVASMETATRACKERENABLE_SETUP:
        {
            DmvaParam *pDmvaParam = (DmvaParam*)pParm;

            vaGuiPrm.dmvaexptsmetatrackerenable = pDmvaParam->dmvaexptsmetatrackerenable;
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPSMETATRACKENABLE,&vaGuiPrm);
            break;
        }

        case STREAM_FEATURE_DMVA_TZ_SETUP:
        {
            DmvaTZParam *pDmvaTZParam = (DmvaTZParam*)pParm;

            vaGuiPrm.dmvaTZPrm.dmvaTZSensitivity        = pDmvaTZParam->dmvaTZSensitivity;
            vaGuiPrm.dmvaTZPrm.dmvaTZPersonMinWidth     = pDmvaTZParam->dmvaTZPersonMinWidth;
            vaGuiPrm.dmvaTZPrm.dmvaTZPersonMinHeight    = pDmvaTZParam->dmvaTZPersonMinHeight;
            vaGuiPrm.dmvaTZPrm.dmvaTZVehicleMinWidth    = pDmvaTZParam->dmvaTZVehicleMinWidth;
            vaGuiPrm.dmvaTZPrm.dmvaTZVehicleMinHeight   = pDmvaTZParam->dmvaTZVehicleMinHeight;
            vaGuiPrm.dmvaTZPrm.dmvaTZPersonMaxWidth     = pDmvaTZParam->dmvaTZPersonMaxWidth;
            vaGuiPrm.dmvaTZPrm.dmvaTZPersonMaxHeight    = pDmvaTZParam->dmvaTZPersonMaxHeight;
            vaGuiPrm.dmvaTZPrm.dmvaTZVehicleMaxWidth    = pDmvaTZParam->dmvaTZVehicleMaxWidth;
            vaGuiPrm.dmvaTZPrm.dmvaTZVehicleMaxHeight   = pDmvaTZParam->dmvaTZVehicleMaxHeight;
            vaGuiPrm.dmvaTZPrm.dmvaTZDirection          = pDmvaTZParam->dmvaTZDirection;
            vaGuiPrm.dmvaTZPrm.dmvaTZEnable             = pDmvaTZParam->dmvaTZEnable;
            vaGuiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides = pDmvaTZParam->dmvaTZZone1_ROI_numSides;
            vaGuiPrm.dmvaTZPrm.dmvaTZZone2_ROI_numSides = pDmvaTZParam->dmvaTZZone2_ROI_numSides;

            for(i = 0; i < vaGuiPrm.dmvaTZPrm.dmvaTZZone1_ROI_numSides;i++)
            {
                vaGuiPrm.dmvaTZPrm.dmvaTZZone1_x[i] = pDmvaTZParam->dmvaTZZone1_x[i];
                vaGuiPrm.dmvaTZPrm.dmvaTZZone1_y[i] = pDmvaTZParam->dmvaTZZone1_y[i];
            }
            for(i = 0; i < vaGuiPrm.dmvaTZPrm.dmvaTZZone2_ROI_numSides;i++)
            {
                vaGuiPrm.dmvaTZPrm.dmvaTZZone2_x[i] = pDmvaTZParam->dmvaTZZone2_x[i];
                vaGuiPrm.dmvaTZPrm.dmvaTZZone2_y[i] = pDmvaTZParam->dmvaTZZone2_y[i];
            }

            vaGuiPrm.dmvaTZPrm.dmvaTZPresentAdjust = pDmvaTZParam->dmvaTZPresentAdjust;

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPTZPRM,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVA_MAIN_SETUP:
        {
            DmvaMainParam *pDmvaMainParam = (DmvaMainParam*)pParm;

            vaGuiPrm.dmvaMainPrm.dmvaEventListEventType = pDmvaMainParam->dmvaEventListEventType;
            vaGuiPrm.dmvaMainPrm.dmvaEventRecordingVAME = pDmvaMainParam->dmvaEventRecordingVAME;
            vaGuiPrm.dmvaMainPrm.dmvaDisplayOptions     = pDmvaMainParam->dmvaDisplayOptions;
            vaGuiPrm.dmvaMainPrm.dmvaCfgCTDImdSmetaTzOc = pDmvaMainParam->dmvaCfgCTDImdSmetaTzOc;

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPMAINPRM,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVA_CTD_SETUP:
        {
            DmvaCTDParam *pDmvaCTDParam = (DmvaCTDParam*)pParm;

            vaGuiPrm.dmvaCTDPrm.dmvaCfgTDEnv1        = pDmvaCTDParam->dmvaCfgTDEnv1;
            vaGuiPrm.dmvaCTDPrm.dmvaCfgTDEnv2        = pDmvaCTDParam->dmvaCfgTDEnv2;
            vaGuiPrm.dmvaCTDPrm.dmvaCfgTDSensitivity = pDmvaCTDParam->dmvaCfgTDSensitivity;
            vaGuiPrm.dmvaCTDPrm.dmvaCfgTDResetTime   = pDmvaCTDParam->dmvaCfgTDResetTime;

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPCTDPRM,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVA_OC_SETUP:
        {
            DmvaOCParam *pDmvaOCParam = (DmvaOCParam*)pParm;

            vaGuiPrm.dmvaOCPrm.dmvaOCSensitivity  = pDmvaOCParam->dmvaOCSensitivity;
            vaGuiPrm.dmvaOCPrm.dmvaOCObjectWidth  = pDmvaOCParam->dmvaOCObjectWidth;
            vaGuiPrm.dmvaOCPrm.dmvaOCObjectHeight = pDmvaOCParam->dmvaOCObjectHeight;
            vaGuiPrm.dmvaOCPrm.dmvaOCDirection    = pDmvaOCParam->dmvaOCDirection;
            vaGuiPrm.dmvaOCPrm.dmvaOCEnable       = pDmvaOCParam->dmvaOCEnable;
            vaGuiPrm.dmvaOCPrm.dmvaOCLeftRight    = pDmvaOCParam->dmvaOCLeftRight;
            vaGuiPrm.dmvaOCPrm.dmvaOCTopBottom    = pDmvaOCParam->dmvaOCTopBottom;

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPOCPRM,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVA_SMETA_SETUP:
        {
            DmvaSMETAParam *pDmvaSMETAParam = (DmvaSMETAParam*)pParm;

            vaGuiPrm.dmvaSMETAPrm.dmvaSMETASensitivity         = pDmvaSMETAParam->dmvaSMETASensitivity;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAEnableStreamingData = pDmvaSMETAParam->dmvaSMETAEnableStreamingData;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAStreamBB            = pDmvaSMETAParam->dmvaSMETAStreamBB;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAStreamVelocity      = pDmvaSMETAParam->dmvaSMETAStreamVelocity;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAStreamHistogram     = pDmvaSMETAParam->dmvaSMETAStreamHistogram;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAStreamMoments       = pDmvaSMETAParam->dmvaSMETAStreamMoments;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAPresentAdjust       = pDmvaSMETAParam->dmvaSMETAPresentAdjust;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinWidth      = pDmvaSMETAParam->dmvaSMETAPersonMinWidth;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMinHeight     = pDmvaSMETAParam->dmvaSMETAPersonMinHeight;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinWidth     = pDmvaSMETAParam->dmvaSMETAVehicleMinWidth;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMinHeight    = pDmvaSMETAParam->dmvaSMETAVehicleMinHeight;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxWidth      = pDmvaSMETAParam->dmvaSMETAPersonMaxWidth;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAPersonMaxHeight     = pDmvaSMETAParam->dmvaSMETAPersonMaxHeight;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxWidth     = pDmvaSMETAParam->dmvaSMETAVehicleMaxWidth;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAVehicleMaxHeight    = pDmvaSMETAParam->dmvaSMETAVehicleMaxHeight;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETANumZones            = pDmvaSMETAParam->dmvaSMETANumZones;
            vaGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides  = pDmvaSMETAParam->dmvaSMETAZone1_ROI_numSides;

            for(i = 0;i < vaGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_ROI_numSides;i ++)
            {
                vaGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_x[i] = pDmvaSMETAParam->dmvaSMETAZone1_x[i];
                vaGuiPrm.dmvaSMETAPrm.dmvaSMETAZone1_y[i] = pDmvaSMETAParam->dmvaSMETAZone1_y[i];
            }

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPSMETAPRM,&vaGuiPrm);
            break;
        }
        case STREAM_FEATURE_DMVA_IMD_SETUP:
        {
            DmvaIMDParam *pDmvaIMDParam = (DmvaIMDParam*)pParm;

            vaGuiPrm.dmvaIMDPrm.dmvaIMDSensitivity        = pDmvaIMDParam->dmvaIMDSensitivity;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDPresentAdjust      = pDmvaIMDParam->dmvaIMDPresentAdjust;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDPersonMinWidth     = pDmvaIMDParam->dmvaIMDPersonMinWidth;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDPersonMinHeight    = pDmvaIMDParam->dmvaIMDPersonMinHeight;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMinWidth    = pDmvaIMDParam->dmvaIMDVehicleMinWidth;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMinHeight   = pDmvaIMDParam->dmvaIMDVehicleMinHeight;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDPersonMaxWidth     = pDmvaIMDParam->dmvaIMDPersonMaxWidth;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDPersonMaxHeight    = pDmvaIMDParam->dmvaIMDPersonMaxHeight;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxWidth    = pDmvaIMDParam->dmvaIMDVehicleMaxWidth;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDVehicleMaxHeight   = pDmvaIMDParam->dmvaIMDVehicleMaxHeight;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDNumZones           = pDmvaIMDParam->dmvaIMDNumZones;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDZoneEnable         = pDmvaIMDParam->dmvaIMDZoneEnable;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides = pDmvaIMDParam->dmvaIMDZone1_ROI_numSides;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides = pDmvaIMDParam->dmvaIMDZone2_ROI_numSides;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides = pDmvaIMDParam->dmvaIMDZone3_ROI_numSides;
            vaGuiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides = pDmvaIMDParam->dmvaIMDZone4_ROI_numSides;

            for(i = 0;i < vaGuiPrm.dmvaIMDPrm.dmvaIMDZone1_ROI_numSides;i ++)
            {
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone1_x[i] = pDmvaIMDParam->dmvaIMDZone1_x[i];
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone1_y[i] = pDmvaIMDParam->dmvaIMDZone1_y[i];
            }
            for(i = 0;i < vaGuiPrm.dmvaIMDPrm.dmvaIMDZone2_ROI_numSides;i ++)
            {
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone2_x[i] = pDmvaIMDParam->dmvaIMDZone2_x[i];
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone2_y[i] = pDmvaIMDParam->dmvaIMDZone2_y[i];
            }
            for(i = 0;i < vaGuiPrm.dmvaIMDPrm.dmvaIMDZone3_ROI_numSides;i ++)
            {
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone3_x[i] = pDmvaIMDParam->dmvaIMDZone3_x[i];
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone3_y[i] = pDmvaIMDParam->dmvaIMDZone3_y[i];
            }
            for(i = 0;i < vaGuiPrm.dmvaIMDPrm.dmvaIMDZone4_ROI_numSides;i ++)
            {
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone4_x[i] = pDmvaIMDParam->dmvaIMDZone4_x[i];
                vaGuiPrm.dmvaIMDPrm.dmvaIMDZone4_y[i] = pDmvaIMDParam->dmvaIMDZone4_y[i];
            }

            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPIMDPRM,&vaGuiPrm);
            break;
        }

        /* DMVA Enc Roi Enable */
        case STREAM_FEATURE_DMVA_ENCROI_ENABLE:
        {
            vaGuiPrm.dmvaEncRoiEnable = *((unsigned int*)pParm);
            Vsys_setVaGuiPrm(VSYS_VAGUI_SETUPENCROIENABLE,&vaGuiPrm);
            break;
        }

        case STREAM_FEATURE_SET_CAMERA_RES:
        {
            UInt32 cnt;
            VCAM_CHN_PARAM_S    chPrms;
            StreamResolution *strmPrms;

            strmPrms = (StreamResolution *)pParm;

            /* At Max, Two streams are supported */
            if (strmPrms->numStrm >= 3)
            {
                strmPrms->numStrm = 2;
            }

            for (cnt = 0u; cnt < strmPrms->numStrm; cnt ++)
            {
                chPrms.strmEnable[cnt] = TRUE;
                chPrms.strmResolution[cnt].start_X = 0;
                chPrms.strmResolution[cnt].start_Y = 0;
                chPrms.strmResolution[cnt].width = strmPrms->resolution[cnt].width;
                chPrms.strmResolution[cnt].height = strmPrms->resolution[cnt].height;
                printf("\n stream.c StreamId %d InSize %dx%d \n", cnt,
                													chPrms.strmResolution[cnt].width,
                													chPrms.strmResolution[cnt].height);
            }
            /* Input Frame size is same as streams0 size */
            chPrms.inputWidth = strmPrms->resolution[0].width;
            chPrms.inputHeight = strmPrms->resolution[0].height;

            Vcam_changeCaptMode(0, &chPrms);

            break;
        }
		case STREAM_FEATURE_SET_CAMERA_CODECCFG:
		{
			UInt32 i;
			VCODEC_TYPE_E codecType;
			VideoCodecCfg *pVidCodecCfgPrm = (VideoCodecCfg*)pParm;
			STREAM_PARM *pParm = stream_get_handle();

			pVidCodecCfgPrm->restartFlag = 0;

			if(((pVidCodecCfgPrm->codecCfg[0].width == 1920) && (pVidCodecCfgPrm->codecCfg[0].height == 1080)) &&
			   ((pVidCodecCfgPrm->codecCfg[1].width == 1920) && (pVidCodecCfgPrm->codecCfg[1].height == 1080)))
			{
				/* Dual 1080p use case */
#ifdef MEMORY_256MB
				if(pParm->MemInfo.mem_layout != MEM_LAYOUT_256MB_DUALSTREAM)
				{
					/* Teardown cycle will be performed */
					pVidCodecCfgPrm->restartFlag = 1;
				}
#else
				if(pParm->MemInfo.mem_layout != MEM_LAYOUT_512MB_DUALSTREAM)
				{
					/* Teardown cycle will be performed */
					pVidCodecCfgPrm->restartFlag = 1;
				}
#endif
			}
			else
			{
#ifdef MEMORY_256MB
				if(pParm->MemInfo.mem_layout == MEM_LAYOUT_256MB_DUALSTREAM)
				{
					/* Teardown cycle will be performed */
					pVidCodecCfgPrm->restartFlag = 1;
				}
#else
				if(pParm->MemInfo.mem_layout == MEM_LAYOUT_512MB_DUALSTREAM)
				{
					/* Teardown cycle will be performed */
					pVidCodecCfgPrm->restartFlag = 1;
				}
#endif
			}

			if(pVidCodecCfgPrm->numStream == 3)
				gUI_mcfw_config.demoCfg.usecase_id = TRI_STREAM_USECASE;
			if(pVidCodecCfgPrm->numStream == 2)
				gUI_mcfw_config.demoCfg.usecase_id = DUAL_STREAM_USECASE;
			if(pVidCodecCfgPrm->numStream == 1)
				gUI_mcfw_config.demoCfg.usecase_id = SINGLE_STREAM_USECASE;

			for(i = 0;i < pVidCodecCfgPrm->numStream;i++)
			{
				if(pVidCodecCfgPrm->codecCfg[i].codecType == H264_CODEC)
				{
					codecType = VCODEC_TYPE_H264;
				}
				else if(pVidCodecCfgPrm->codecCfg[i].codecType == MPEG4_CODEC)
					 {
					     codecType = VCODEC_TYPE_MPEG4;
					 }
					 else
					 {
					     codecType = VCODEC_TYPE_MJPEG;
					 }

				Venc_switchCodecAlgCh(i,
									  codecType,
									  pVidCodecCfgPrm->codecCfg[i].encPreset);
			}

			break;
		}
		case STREAM_FEATURE_SET_LPR_RECOGNITION_AREA:
		{
			AlgPolygonArea * pLprRecogArea = (AlgPolygonArea *) pParm;
			lprDynPrm.recogArea.arr[0].x = pLprRecogArea->arr[0].x;
			lprDynPrm.recogArea.arr[0].y = pLprRecogArea->arr[0].y;
			lprDynPrm.recogArea.arr[1].x = pLprRecogArea->arr[1].x;
			lprDynPrm.recogArea.arr[1].y = pLprRecogArea->arr[1].y;
			lprDynPrm.recogArea.arr[2].x = pLprRecogArea->arr[2].x;
			lprDynPrm.recogArea.arr[2].y = pLprRecogArea->arr[2].y;
			lprDynPrm.recogArea.arr[3].x = pLprRecogArea->arr[3].x;
			lprDynPrm.recogArea.arr[3].y = pLprRecogArea->arr[3].y;			
			
			Vsys_setLprDynPrm(VSYS_SET_LPR_RECOGNITION_AREA, &lprDynPrm);	
			
			break;
		}
		case STREAM_FEATURE_SET_LPR_TRIGGER_INFO:
		{
			AlgTriggerInfo * pLprTrigInfo = (AlgTriggerInfo *) pParm;
			lprDynPrm.trigInfo.trigArea.arr[0].x = pLprTrigInfo->trigArea.arr[0].x;
			lprDynPrm.trigInfo.trigArea.arr[0].y = pLprTrigInfo->trigArea.arr[0].y;
			lprDynPrm.trigInfo.trigArea.arr[1].x = pLprTrigInfo->trigArea.arr[1].x;
			lprDynPrm.trigInfo.trigArea.arr[1].y = pLprTrigInfo->trigArea.arr[1].y;
			lprDynPrm.trigInfo.trigArea.arr[2].x = pLprTrigInfo->trigArea.arr[2].x;
			lprDynPrm.trigInfo.trigArea.arr[2].y = pLprTrigInfo->trigArea.arr[2].y;
			lprDynPrm.trigInfo.trigArea.arr[3].x = pLprTrigInfo->trigArea.arr[3].x;
			lprDynPrm.trigInfo.trigArea.arr[3].y = pLprTrigInfo->trigArea.arr[3].y;
			lprDynPrm.trigInfo.nTrigInterval = pLprTrigInfo->nTrigInterval;
			lprDynPrm.trigInfo.nTrigMode = pLprTrigInfo->nTrigMode;
			lprDynPrm.trigInfo.nVehicleDirection = pLprTrigInfo->nVehicleDirection;		
			
			Vsys_setLprDynPrm(VSYS_SET_LPR_TRIGGER_INFO, &lprDynPrm);
			
			break;
		}
		case STREAM_FEATURE_SET_LPR_TRIGGER_MODE:
		{
			break;
		}
		case STREAM_FEATURE_SET_LPR_TRIGGER_INTERVAL:
		{
			break;
		}
		case STREAM_FEATURE_SET_LPR_DEFAULT_PROVINCE:
		{
			char *pLprDefaultProvince = (char *) pParm;
			memcpy(&lprDynPrm.szProvince[0], pLprDefaultProvince, sizeof(lprDynPrm.szProvince));	
			
			Vsys_setLprDynPrm(VSYS_SET_LPR_DEFAULT_PROVINCE, &lprDynPrm);	
			break;
		}
		case STREAM_FEATURE_SET_LPR_PLATE_TYPE:
		{
			break;
		}
		case STREAM_FEATURE_SET_LPR_PLATE_WIDTH:
		{
			AlgPlateWidth * pLprPlateWidth = (AlgPlateWidth *) pParm;
			lprDynPrm.plateWidth.nMax = pLprPlateWidth->nMax;
			lprDynPrm.plateWidth.nMin = pLprPlateWidth->nMin;			
			
			Vsys_setLprDynPrm(VSYS_SET_LPR_PLATE_WIDTH, &lprDynPrm);			
			break;
		}	
		case STREAM_FEATURE_SET_LPR_MOVING_DIRECTION:
		{
			break;
		}
		case STREAM_FEATURE_SET_LPR_RECOGNITION_SCHEDULE:
		{
			break;
		}
        default:
            ERR("unknow feature setup num = %d\n", nFeature);
            break;
    }

}
