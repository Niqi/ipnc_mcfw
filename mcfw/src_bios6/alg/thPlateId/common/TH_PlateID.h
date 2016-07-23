// ***************************************************************
//  TH_PlateID.h   version:  4.0     date: 2010.4.12
//  -------------------------------------------------------------
//  清华大学智能图文信息处理研究室。版权所有。
//  -------------------------------------------------------------
//  Center for Intelligent Image and Document Information Processing
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//   Author: Liu CS, Zhou J
// ***************************************************************
//		Revision history:
//			2010.4.12: v4.0, TH_PlateID车牌识别SDK的接口
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
/* TH_InitPlateIDSDK: 初始化车牌识别SDK，在使用该SDK的功能前			*/
/*			必需且仅需调用一次该函数									*/
/*		Parameters:														*/
/*			pPlateConfig[in]: 车牌识别SDK的配置							*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_InitPlateIDSDK(TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_UninitPlateIDSDK: 释放车牌识别SDK，在使用该SDK的功能后			*/
/*			必需且仅需调用一次该函数，以释放内存。						*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_UninitPlateIDSDK(TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_RecogImage: 识别内存中车牌图像(输出识别到的多个结果)	*/
/*		Parameters:														*/
/*			pbyBits[in]: 指向内存图像数据的指针，3个字节表示1个像素		*/
/*			nWidth[in]: 图像的宽度										*/
/*			nHeight[in]: 图像的高度										*/
/*			pResult[out]: 车牌识别结果数组, 调用方开辟pResult[nResultNum]内存*/
/*			nResultNum[in,out]: in 最大候选车牌个数，out 识别出的车牌个数*/
/*			prcRange[in]: 指定识别范围									*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_RecogImage(const unsigned char *pbyBits, int nWidth, int nHeight, TH_PlateIDResult *pResult,
	int *nResultNum, const TH_RECT *prcRange, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_EvaluateCarColor: 识别车的颜色									*/
/*		Parameters:														*/
/*			pbyBits[in]: 指向内存图像数据的指针，3个字节表示1个像素		*/
/*			nWidth[in]: 图像的宽度										*/
/*			nHeight[in]: 图像的高度										*/
/*			pResult[out]: 车牌识别结果数组, 调用方开辟pResult[nResultNum]内存*/
/*			nResultNum[in,out]: in 最大候选车牌个数，out 识别出的车牌个数*/
/*			prcRange[in]: 指定识别范围									*/
/*		Return Value: int(ERR_Code)										*/
/*		TH_EvaluateCarColor紧接TH_RecogImage之后调用，保持参数不变		*/
/************************************************************************/
int __stdcall TH_EvaluateCarColor(const unsigned char *pbyBits, int nWidth, int nHeight, TH_PlateIDResult *pResult,
	int *nResultNum, const TH_RECT *prcRange, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetImageFormat: 设置图像格式										*/
/*		Parameters:														*/
/*			cImageFormat[in]: 图像格式									*/
/*			bVertFlip[in]: 是否颠倒										*/
/*			bDwordAligned[in]: 是否4字节对齐							*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetImageFormat( unsigned char cImageFormat, unsigned char bVertFlip, unsigned char bDwordAligned, TH_PlateIDCfg *pPlateConfig );

/************************************************************************/
/* TH_SetEnabledPlateFormat: 设置支持的车牌类型							*/
/*		Parameters:														*/
/*			dFormat[in]: 车牌类型开关，例如PARAM_INDIVIDUAL_ON			*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnabledPlateFormat(unsigned int dFormat, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetProvinceOrder: 设置省份字符串									*/
/*		Parameters:														*/
/*			szProvince[in]: 默认省份字符串，例如"京津冀"，最多支持8个省份*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetProvinceOrder( char* szProvince, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetRecogThreshold: 设置识别阈值									*/
/*		Parameters:														*/
/*			nPlateLocate_Th[in]: 0 - 9		--   7: 默认阈值			*/
/*					nOCR_Th[in]: 0 - 9		--   5: 默认阈值			*/
/*				0: 最宽松的阈值											*/
/*				9:最严格的阈值											*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetRecogThreshold( unsigned char nPlateLocate_Th, unsigned char nOCR_Th, TH_PlateIDCfg *pPlateCfg);

//检查工作过程中最小的剩余内存，如果出现负数，则需要增加给定的初始内存
int __stdcall TH_CheckMinFreeMemory( int *pnMinFreeSRAM, int *pnMinFreeSDRAM, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* 功能: 获取版本号														*/
/*		Parameters：无													*/
/*		返回值: 字符串 格式：主版本号.副版本号.修订号.编译号			*/
/*				不用释放该指针。										*/
/************************************************************************/
const char * __stdcall TH_GetVersion();

/************************************************************************/
/* 功能: 获取加密锁ID, 加密锁ID是8字节唯一ID，							*/
/*		调用此函数前需要调用TH_InitPlateIDSDK							*/
/*		Parameters：存在唯一ID时，ID1是前4字节,ID2是后四字节			*/
/*		返回值: 0-有唯一ID												*/
/*				1-无唯一ID												*/
/************************************************************************/
int __stdcall TH_GetKeyID(unsigned int* ID1, unsigned int* ID2);

/************************************************************************/
/* 功能: 获取加密锁路数信息，											*/
/*		使用TH_InitPlateIDSDK前调用此函数								*/
/*		Parameters：nMaxThread[out]:									*/
/*		返回值:  int(ERR_Code)											*/
/************************************************************************/
int __stdcall TH_GetKeyMaxThread(int* nMaxThread);


/************************************************************************/
/* 功能: 设置当前识别的对比度阈值										*/
/*		Parameters：nContrast[int]:										*/
/*		对比度指数 nContrast[in]: 0 - 9									*/
/*		最模糊时设为1;最清晰时设为9;自动探测设为0;默认值为0				*/
/*		返回值:  int(ERR_Code)											*/
/************************************************************************/
int __stdcall TH_SetContrast( unsigned char nContrast, TH_PlateIDCfg *pPlateCfg );
																			


/************************************************************************/
/* TH_SetEnableCarTypeClassify: 设置是否车辆类型判别					*/
/*		Parameters:														*/
/*			bCarTypeClass[in]: true:车型分类;							*/
/*								false:不进行车型分类					*/
/*		Return Value: int(ERR_Code)										*/
/*		支持格式: ImageFormatBGR, ImageFormatRGB,ImageFormatYUV422,
				ImageFormatUYVY,ImageFormatNV21*/
/************************************************************************/
int __stdcall TH_SetEnableCarTypeClassify( unsigned char bCarTypeClass, TH_PlateIDCfg *pPlateCfg);


/************************************************************************/
/* TH_SetEnableCarLogo: 设置是否车标识别								*/
/*		Parameters:														*/
/*			bCarLogo[in]: true:车标识别;								*/
/*								false:不进行车标识别					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableCarLogo( unsigned char bCarLogo, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_SetEnableCarWidth: 设置是否测量车辆宽度							*/
/*		Parameters:														*/
/*			bCarLogo[in]: true:测量车辆宽度;							*/
/*							false:不测量车辆宽度						*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableCarWidth( unsigned char bCarWidth, TH_PlateIDCfg *pPlateCfg);

int __stdcall TH_SetReserveInfo(unsigned char* pSN);

// TH_SetDayNightMode: V4.3.13.0以后函数无实际意义，为保证兼容性保留此函数
int __stdcall TH_SetDayNightMode( unsigned char bIsNight, TH_PlateIDCfg *pPlateConfig);

/************************************************************************/
/* TH_SetVideoModeOutPutPosRatio: 设置视频模式车牌输出位置系数				*/
/*		Parameters:														*/
/*			ratio[in]: (0.1, 0.9);										*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetVideoModeOutPutPosRatio(float ratio);

/************************************************************************/
/* TH_GetVehicleModelName: 输出车型字符串，在调用TH_RecogImage后调用该函数*/
/*		Parameters:														*/
/*			nModel[in]: 车型类型值，TH_PlateIDResult结构体中的nCarModel;*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
const char *  __stdcall TH_GetVehicleModelName(int nModel);

/************************************************************************/
/* TH_SetEnableLeanCorrection: 设置是否打开倾斜校正								*/
/*		Parameters:														*/
/*			bLeanCorrection[in]: true:打开倾斜校正;								*/
/*								 false:关闭倾斜校正					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableLeanCorrection( unsigned char bLeanCorrection, TH_PlateIDCfg *pPlateCfg);

/************************************************************************/
/* TH_SetEnableShadow: 设置是否打开阴阳牌识别								*/
/*		Parameters:														*/
/*			bShadow[in]: true:打开阴阳牌;								*/
/*						 false:关闭阴阳牌					*/
/*		Return Value: int(ERR_Code)										*/
/************************************************************************/
int __stdcall TH_SetEnableShadow( unsigned char bShadow, TH_PlateIDCfg *pPlateCfg);

#ifdef __cplusplus
}
#endif

#endif	//	__TH_PLATEID_H_INCLUDE__
