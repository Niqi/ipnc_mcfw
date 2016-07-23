// ***************************************************************
//  TH_PlateID.h   version:  4.0     date: 2010.4.12
//  -------------------------------------------------------------
//  �廪��ѧ����ͼ����Ϣ�����о��ҡ���Ȩ���С�
//  -------------------------------------------------------------
//  Center for Intelligent Image and Document Information Processing
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//   Author: Liu CS, Zhou J
// ***************************************************************
//		Revision history:
//			2010.4.12: v4.0, TH_PlateID����ʶ��SDK�Ľӿ�
// ***************************************************************

#ifndef __TH_PLATEID_H_INCLUDE__
#define __TH_PLATEID_H_INCLUDE__

#if !defined(WIN32) && !defined(__stdcall)
#define __stdcall
#endif

#include "TH_ErrorDef.h"

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************/
/* TH_InitPlateIDSDK: ��ʼ������ʶ��SDK����ʹ�ø�SDK�Ĺ���ǰ			*/
/*			�����ҽ������һ�θú���									*/
/*		Parameters:														*/
/*			pPlateConfig[in]: ����ʶ��SDK������							*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_InitPlateIDSDK(TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_UninitPlateIDSDK: �ͷų���ʶ��SDK����ʹ�ø�SDK�Ĺ��ܺ�			*/
/*			�����ҽ������һ�θú��������ͷ��ڴ档						*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_UninitPlateIDSDK(TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_RecogImage: ʶ���ڴ��г���ͼ��(���ʶ�𵽵Ķ�����)	*/
/*		Parameters:														*/
/*			pbyBits[in]: ָ���ڴ�ͼ�����ݵ�ָ�룬3���ֽڱ�ʾ1������		*/
/*			nWidth[in]: ͼ��Ŀ��										*/
/*			nHeight[in]: ͼ��ĸ߶�										*/
/*			pResult[out]: ����ʶ��������, ���÷�����pResult[nResultNum]�ڴ�*/
/*			nResultNum[in,out]: in ����ѡ���Ƹ�����out ʶ����ĳ��Ƹ���*/
/*			prcRange[in]: ָ��ʶ��Χ									*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_RecogImage(const unsigned char *pbyBits, int nWidth, int nHeight, TH_PlateIDResult *pResult,
	int *nResultNum, const TH_RECT *prcRange, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_EvaluateCarColor: ʶ�𳵵���ɫ									*/
/*		Parameters:														*/
/*			pbyBits[in]: ָ���ڴ�ͼ�����ݵ�ָ�룬3���ֽڱ�ʾ1������		*/
/*			nWidth[in]: ͼ��Ŀ��										*/
/*			nHeight[in]: ͼ��ĸ߶�										*/
/*			pResult[out]: ����ʶ��������, ���÷�����pResult[nResultNum]�ڴ�*/
/*			nResultNum[in,out]: in ����ѡ���Ƹ�����out ʶ����ĳ��Ƹ���*/
/*			prcRange[in]: ָ��ʶ��Χ									*/
/*		Return Value: int(ERR_Code)										*/
/*		TH_EvaluateCarColor����TH_RecogImage֮����ã����ֲ�������		*/
/************************************************************************/
int __stdcall TH_EvaluateCarColor(const unsigned char *pbyBits, int nWidth, int nHeight, TH_PlateIDResult *pResult,
	int *nResultNum, const TH_RECT *prcRange, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetImageFormat: ����ͼ���ʽ										*/
/*		Parameters:														*/
/*			cImageFormat[in]: ͼ���ʽ									*/
/*			bVertFlip[in]: �Ƿ�ߵ�										*/
/*			bDwordAligned[in]: �Ƿ�4�ֽڶ���							*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetImageFormat( unsigned char cImageFormat, unsigned char bVertFlip, unsigned char bDwordAligned, TH_PlateIDCfg *pPlateConfig );

/************************************************************************/
/* TH_SetEnabledPlateFormat: ����֧�ֵĳ�������							*/
/*		Parameters:														*/
/*			dFormat[in]: �������Ϳ��أ�����PARAM_INDIVIDUAL_ON			*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnabledPlateFormat(unsigned int dFormat, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetProvinceOrder: ����ʡ���ַ���									*/
/*		Parameters:														*/
/*			szProvince[in]: Ĭ��ʡ���ַ���������"����"�����֧��8��ʡ��*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetProvinceOrder( char* szProvince, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetRecogThreshold: ����ʶ����ֵ									*/
/*		Parameters:														*/
/*			nPlateLocate_Th[in]: 0 - 9		--   7: Ĭ����ֵ			*/
/*					nOCR_Th[in]: 0 - 9		--   5: Ĭ����ֵ			*/
/*				0: ����ɵ���ֵ											*/
/*				9:���ϸ����ֵ											*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetRecogThreshold( unsigned char nPlateLocate_Th, unsigned char nOCR_Th, TH_PlateIDCfg *pPlateCfg);

//��鹤����������С��ʣ���ڴ棬������ָ���������Ҫ���Ӹ����ĳ�ʼ�ڴ�
int __stdcall TH_CheckMinFreeMemory( int *pnMinFreeSRAM, int *pnMinFreeSDRAM, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* ����: ��ȡ�汾��														*/
/*		Parameters����													*/
/*		����ֵ: �ַ��� ��ʽ�����汾��.���汾��.�޶���.�����			*/
/*				�����ͷŸ�ָ�롣										*/
/************************************************************************/
const char * __stdcall TH_GetVersion();

/************************************************************************/
/* ����: ��ȡ������ID, ������ID��8�ֽ�ΨһID��							*/
/*		���ô˺���ǰ��Ҫ����TH_InitPlateIDSDK							*/
/*		Parameters������ΨһIDʱ��ID1��ǰ4�ֽ�,ID2�Ǻ����ֽ�			*/
/*		����ֵ: 0-��ΨһID												*/
/*				1-��ΨһID												*/
/************************************************************************/
int __stdcall TH_GetKeyID(unsigned int* ID1, unsigned int* ID2);

/************************************************************************/
/* ����: ��ȡ������·����Ϣ��											*/
/*		ʹ��TH_InitPlateIDSDKǰ���ô˺���								*/
/*		Parameters��nMaxThread[out]:									*/
/*		����ֵ:  int(ERR_Code)											*/
/************************************************************************/
int __stdcall TH_GetKeyMaxThread(int* nMaxThread);


/************************************************************************/
/* ����: ���õ�ǰʶ��ĶԱȶ���ֵ										*/
/*		Parameters��nContrast[int]:										*/
/*		�Աȶ�ָ�� nContrast[in]: 0 - 9									*/
/*		��ģ��ʱ��Ϊ1;������ʱ��Ϊ9;�Զ�̽����Ϊ0;Ĭ��ֵΪ0				*/
/*		����ֵ:  int(ERR_Code)											*/
/************************************************************************/
int __stdcall TH_SetContrast( unsigned char nContrast, TH_PlateIDCfg *pPlateCfg );
																			


/************************************************************************/
/* TH_SetEnableCarTypeClassify: �����Ƿ��������б�					*/
/*		Parameters:														*/
/*			bCarTypeClass[in]: true:���ͷ���;							*/
/*								false:�����г��ͷ���					*/
/*		Return Value: int(ERR_Code)										*/
/*		֧�ָ�ʽ: ImageFormatBGR, ImageFormatRGB,ImageFormatYUV422,
				ImageFormatUYVY,ImageFormatNV21*/
/************************************************************************/
int __stdcall TH_SetEnableCarTypeClassify( unsigned char bCarTypeClass, TH_PlateIDCfg *pPlateCfg);


/************************************************************************/
/* TH_SetEnableCarLogo: �����Ƿ񳵱�ʶ��								*/
/*		Parameters:														*/
/*			bCarLogo[in]: true:����ʶ��;								*/
/*								false:�����г���ʶ��					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableCarLogo( unsigned char bCarLogo, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_SetEnableCarWidth: �����Ƿ�����������							*/
/*		Parameters:														*/
/*			bCarLogo[in]: true:�����������;							*/
/*							false:�������������						*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableCarWidth( unsigned char bCarWidth, TH_PlateIDCfg *pPlateCfg);

int __stdcall TH_SetReserveInfo(unsigned char* pSN);

// TH_SetDayNightMode: V4.3.13.0�Ժ�����ʵ�����壬Ϊ��֤�����Ա����˺���
int __stdcall TH_SetDayNightMode( unsigned char bIsNight, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetVideoModeOutPutPosRatio: ������Ƶģʽ�������λ��ϵ��				*/
/*		Parameters:														*/
/*			ratio[in]: (0.1, 0.9);										*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetVideoModeOutPutPosRatio(float ratio);

/************************************************************************/
/* TH_GetVehicleModelName: ��������ַ������ڵ���TH_RecogImage����øú���*/
/*		Parameters:														*/
/*			nModel[in]: ��������ֵ��TH_PlateIDResult�ṹ���е�nCarModel;*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
const char *  __stdcall TH_GetVehicleModelName(int nModel);

/************************************************************************/
/* TH_SetEnableLeanCorrection: �����Ƿ����бУ��								*/
/*		Parameters:														*/
/*			bLeanCorrection[in]: true:����бУ��;								*/
/*								 false:�ر���бУ��					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableLeanCorrection( unsigned char bLeanCorrection, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_SetEnableShadow: �����Ƿ��������ʶ��								*/
/*		Parameters:														*/
/*			bShadow[in]: true:��������;								*/
/*						 false:�ر�������					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableShadow( unsigned char bShadow, TH_PlateIDCfg *pPlateCfg);

#ifdef __cplusplus
}
#endif

#endif	//	__TH_PLATEID_H_INCLUDE__
