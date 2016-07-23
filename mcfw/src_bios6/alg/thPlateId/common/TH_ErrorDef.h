// ***************************************************************
//  TH_ErrorDef.h   version:  4.0     date: 2010.4.12
//  -------------------------------------------------------------
//  清华大学智能图文信息处理研究室。版权所有。
//  -------------------------------------------------------------
//  Center for Intelligent Image and Document Information Processing
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//   Author: Zhou Jian
// ***************************************************************
//		Revision history:
//			2010.4.12: v4.0, 定义SDK的出错信息
// ***************************************************************

#if !defined(__TH_ERRORDEF_INCLUDE_H__)
#define __TH_ERRORDEF_INCLUDE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RECOGNITION_VEHICLE_NUMBER  (8)

//车牌颜色
#define LC_UNKNOWN  0	// 未知
#define LC_BLUE   1		// 蓝色
#define LC_YELLOW 2		// 黄色
#define LC_WHITE  3		// 白色
#define LC_BLACK  4		// 黑色
#define LC_GREEN  5		// 绿色
	
//车牌类型
#define LT_UNKNOWN  0   //未知车牌
#define LT_BLUE     1   //蓝牌
#define LT_BLACK    2   //黑牌
#define LT_YELLOW   3   //单排黄牌
#define LT_YELLOW2  4   //双排黄牌（大车尾牌，农用车）
#define LT_POLICE   5   //警车车牌
#define LT_ARMPOL   6   //武警车牌
#define LT_INDIVI   7   //个性化车牌
#define LT_ARMY     8   //单排军车
#define LT_ARMY2    9   //双排军车
#define LT_EMBASSY  10  //使馆牌
#define LT_HONGKONG 11  //香港牌
#define LT_TRACTOR  12  //拖拉机
#define LT_MACAU    13  //澳门牌
#define LT_CHANGNEI 14  //厂内牌
#define LT_MINHANG  15  //民航牌

//车辆颜色
#define LGRAY_DARK	0	//深
#define LGRAY_LIGHT	1	//浅

#define LCOLOUR_WHITE	0	//白	
#define LCOLOUR_SILVER	1	//灰(银)
#define LCOLOUR_YELLOW	2	//黄
#define LCOLOUR_PINK	3	//粉
#define LCOLOUR_RED		4	//红
#define LCOLOUR_GREEN	5	//绿
#define LCOLOUR_BLUE	6	//蓝
#define LCOLOUR_BROWN	7	//棕 
#define LCOLOUR_BLACK	8	//黑


//运动方向
#define DIRECTION_UNKNOWN	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	3
#define DIRECTION_DOWN	4

//图像格式（TH_SetImageFormat函数的cImageFormat参数）
#define ImageFormatRGB		0			//RGBRGBRGB...
#define ImageFormatBGR		1			//BGRBGRBGR...
#define ImageFormatYUV422	2			//YYYY...UU...VV..	(YV16)
#define ImageFormatYUV420COMPASS 3		//YYYY...UV...		(NV12)
#define ImageFormatYUV420	4			//YYYY...U...V...	(YU12)
#define ImageFormatUYVY	    5			//UYVYUYVYUYVY...	(UYVY)
#define ImageFormatNV21		6			//YYYY...VU...		(NV21)
#define ImageFormatYV12		7			//YYYY...V...U		(YV12)
#define ImageFormatNV16     8           //YYYY...UVUV...    (NV16或YUV422SP) UV列方向抽样，行方向不变 

//车牌类型支持设置定义
//（TH_SetEnabledPlateFormat函数的dFormat参数）
#define PARAM_INDIVIDUAL_ON          0		// 个性化车牌开启
#define PARAM_INDIVIDUAL_OFF         1		// 个性化车牌关闭
#define PARAM_TWOROWYELLOW_ON	     2		// 双层黄色车牌开启
#define PARAM_TWOROWYELLOW_OFF	     3		// 双层黄色车牌关闭
#define PARAM_ARMPOLICE_ON           4		// 单层武警车牌开启
#define PARAM_ARMPOLICE_OFF          5		// 单层武警车牌关闭
#define PARAM_TWOROWARMY_ON          6		// 双层军队车牌开启
#define PARAM_TWOROWARMY_OFF         7		// 双层军队车牌关闭
#define PARAM_TRACTOR_ON			 8		// 农用车车牌开启
#define PARAM_TRACTOR_OFF			 9		// 农用车车牌关闭
#define PARAM_ONLY_TWOROWYELLOW_ON   10		// 只识别双层黄牌开启
#define PARAM_ONLY_TWOROWYELLOW_OFF  11		// 只识别双层黄牌关闭
#define PARAM_EMBASSY_ON			 12		// 使馆车牌开启
#define PARAM_EMBASSY_OFF			 13		// 使馆车牌关闭
#define PARAM_ONLY_LOCATION_ON		 14		// 只定位车牌开启
#define PARAM_ONLY_LOCATION_OFF		 15		// 只定位车牌关闭
#define PARAM_ARMPOLICE2_ON			 16		// 双层武警车牌开启
#define PARAM_ARMPOLICE2_OFF		 17		// 双层武警车牌关闭
#define PARAM_CHANGNEI_ON			 18		// 厂内车牌开启
#define PARAM_CHANGNEI_OFF			 19		// 厂内车牌关闭
#define PARAM_MINHANG_ON			 20		// 民航车牌开启
#define PARAM_MINHANG_OFF			 21		// 民航车牌关闭


#define RECOG_STAGE_ALL			0		// 无错误
#define RECOG_STAGE_FINDPLATE	1		// 没有找到车牌
#define RECOG_STAGE_PLATESCORE_ZERO	2	// 车牌评价值(0分)
#define RECOG_STAGE_PLATESCORE_LOW	3	// 车牌评价值(不及格)
#define RECOG_STAGE_RECOGSCORE_ZERO	4	// 车牌识别分数(0分)
#define RECOG_STAGE_RECOGSCORE_LOW	5	// 车牌识别分数(不及格)

	
//车标类型
#define CarLogo_UNKNOWN       0    //未知
#define CarLogo_AUDI          1    //奥迪
#define CarLogo_BMW           2    //宝马
#define CarLogo_BENZ          3    //奔驰
#define CarLogo_HONDA         4    //本田
#define CarLogo_PEUGEOT       5    //标志
#define CarLogo_BUICK         6    //别克
#define CarLogo_DASAUTO       7    //大众
#define CarLogo_TOYOTA        8    //丰田
#define CarLogo_FORD          9    //福特
#define CarLogo_SUZUKI        10   //铃木
#define CarLogo_MAZDA         11   //马自达
#define CarLogo_KIA           12   //起亚
#define CarLogo_NISSAN        13   //日产尼桑
#define CarLogo_HYUNDAI       14   //现代
#define CarLogo_CHEVROLET     15   //雪佛兰
#define CarLogo_CITROEN       16   //雪铁龙

#define CarLogo_QIRUI         17   //奇瑞
#define CarLogo_WULING        18   //五菱
#define CarLogo_DONGFENG      19   //东风
#define CarLogo_JIANGHUAI     20   //江淮
#define CarLogo_BEIQI         21   //北汽
#define CarLogo_CHANGAN       22   //长安
#define CarLogo_AOCHI         23   //奥驰
#define CarLogo_SHAOLING      24   //少林
#define CarLogo_SHANQI        25   //陕汽
#define CarLogo_SANLING       26   //三菱
#define CarLogo_JILI          27   //吉利
#define CarLogo_HAOWO         28   //豪沃
#define CarLogo_HAIMA         29   //海马
#define CarLogo_HAFEI         30   //哈飞
#define CarLogo_CHANGCHENG    31   //长城
#define CarLogo_FUTIAN        32   //福田
#define CarLogo_NANJUN        33   //南骏
#define CarLogo_LIUQI         34   //柳汽

// 车辆类型
#define CARTYPE_UNKNOWN		0	// 未知
#define CARTYPE_SALOON		1	// 轿车
#define CARTYPE_VAN			2	// 面包车

typedef struct TH_RECT
{
	int left;
	int top;
	int right;
	int bottom;
}TH_RECT;


typedef struct TH_PlateIDCfg
{
	int nMinPlateWidth;					// 检测的最小车牌宽度，以像素为单位
	int nMaxPlateWidth;					// 检测的最大车牌宽度，以像素为单位
	
	int nMaxImageWidth;					// 最大图像宽度
	int nMaxImageHeight;				// 最大图像高度

	unsigned char bVertCompress;		// 是否垂直方向压缩1倍后识别
	unsigned char bIsFieldImage;		// 是否是场图像
	unsigned char bOutputSingleFrame;	// 是否视频图像中同一个车的多幅图像只输出一次结果		
	unsigned char bMovingImage;			// 识别运动or静止图像

	unsigned char bIsNight;
	unsigned char nImageFormat;

	unsigned char * pFastMemory;		// DSP等的片内内存，耗时多的运算优先使用这些内存
	int nFastMemorySize;				// 快速内存的大小

	unsigned char *pMemory;				// 普通内存的地址，内建的内存管理，避免内存泄漏等问题
	int nMemorySize;					// 普通内存的大小

	int (*DMA_DataCopy)(void *dst, void *src,int nSize);
	int (*Check_DMA_Finished)();

	int nLastError;			// 用于传递错误信息
							// 0: 无错误
							// 1: FindPlate(没有找到车牌)
							// 2: 车牌评价值(0分)
							// 3: 车牌评价值(不及格)
							// 4: 车牌识别分数(0分)
	                        // 5: 车牌识别分数(不及格)
	int nErrorModelSN;		// 出错的模块编号
	unsigned char nOrderOpt;			//输出顺序选项 0-置信度 1-自上而下 2-自下而上
	unsigned char bLeanCorrection;		// 是否启用车牌旋转功能，默认开启
	unsigned char bMovingOutputOpt;   	// 0-内部推送+外部获取 1:外部获取	
	unsigned char nImproveSpeed;        // 0: 识别率优先 1:识别速度优先
	unsigned char bCarLogo;             // 0: 不检测车标 1: 检测车标
	unsigned char bLotDetect;			// 0: 不检测车位 1: 检测车位
	unsigned char bShadow;              // 0: 针对无阴影的车牌 1：针对有阴影的车牌，默认开启
	unsigned char bUTF8;				// 0:汉字GBK,1:汉字UTF-8
	unsigned char bShieldRailing;		// 0: 屏蔽栏杆干扰， 1:不屏蔽栏杆干扰
	unsigned char bCarModel;		// 0: 不识别车型， 1: 识别车型
	char reserved[110];				// WIN_X86
	//char reserved[110+128];				// WIN_X64

}TH_PlateIDCfg;


typedef struct TH_PlateIDResult 
{
	char license[16];	// 车牌字符串
	char color[8];		// 车牌颜色

	int nColor;			// 车牌颜色
	int nType;			// 车牌类型
	int nConfidence;	// 整牌可信度
	int nBright;		// 亮度评价
	int nDirection;		// 车牌运动方向，0 unknown, 1 left, 2 right, 3 up, 4 down 
	
	TH_RECT rcLocation;				// 车牌坐标
	const unsigned char *pbyBits;	/* 该识别结果对应的图片的缓冲区, 只有当 bOutputSingleFrame = true 时，该指针才有效。
									下次识别后，该缓冲区内容被覆盖。调用程序无需释放该缓冲区。
									缓冲区大小等于传递进来的图片数据的长度*/
	int nTime;						// 识别耗时
	unsigned char nCarBright;		//车的亮度
	unsigned char nCarColor;		//车的颜色
	unsigned char nCarLogo;         //车标类型
	unsigned char nCarType;			//车辆类型
	unsigned char *pbyPlateBin;     //车牌二值化结果（按bit存储）
	unsigned short nBinPlateWidth;  //二值化结果中车牌宽度
	unsigned short nBinPlateHeight; //二值化结果中车牌高度
	char reserved[70];				//保留
	TH_RECT rcLogoLocation;			// 车标坐标
	unsigned short nCarModel;		// 车辆类型
	unsigned short nCarModelConfidence;		//车型可信度	
}TH_PlateIDResult;

// The errors that may occur during the use of the SDK
#define		TH_ERR_NONE								0		//没有错误
#define		TH_ERR_GENERIC							1		//省份设置错误
#define		TH_ERR_MEMORYALLOC						2		//内存分配错误
#define		TH_ERR_INVALIDFORMAT					7		//不支持的图像格式
#define		TH_ERR_INVALIDWIDTH						8		//图像宽度必须是8的整数倍
#define     TH_ERR_THREADLIMIT						20		//调用线程数超过规定数量
#define		TH_ERR_NODOG							-1		//没有找到加密狗
#define		TH_ERR_CARTYPEERROR						-2		//车辆类型识别模块错误
#define		TH_ERR_READDOG							-3		//读取加密狗出错
#define		TH_ERR_INVALIDDOG						-4		//不是合法的加密狗
#define		TH_ERR_INVALIDUSER						-6		//不是合法的加密狗用户
#define		TH_ERR_MOUDLEERROR						-7		//车标识别模块错误
#define     TH_ERR_INVALIDMOUDLE					-8		//模块没有合法授权
#define     TH_ERR_BUFFULL							-9		//识别缓冲区已满
#define		TH_ERR_INITVEHDETECT					-10		//初始化车辆检测模块错误
#define		TH_ERR_VEHDETECT						-11		//车辆检测模块错误
#define     TH_ERR_INVALIDCALL						-99		//非法调用
#define     TH_ERR_EXCEPTION						-100	//异常
#define		TH_ERR_INITLIMIT						21		//初始化次数超过加密狗许可 
#define		TH_ERR_MULTIINSTANCE					22		//车牌识别实例超限制

//以下为车型识别算法返回错误
#define		TH_ERR_CARMODEL_PLATELOC_ERR			1001	//车牌坐标信息异常
#define     TH_ERR_READMODEL						1002	//读车型模型异常

#ifdef __cplusplus
}	// extern "C"
#endif

#endif // !defined(__TH_ERRORDEF_INCLUDE_H__)
