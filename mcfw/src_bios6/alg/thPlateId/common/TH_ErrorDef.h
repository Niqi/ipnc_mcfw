// ***************************************************************
//  TH_ErrorDef.h   version:  4.0     date: 2010.4.12
//  -------------------------------------------------------------
//  �廪��ѧ����ͼ����Ϣ�����о��ҡ���Ȩ���С�
//  -------------------------------------------------------------
//  Center for Intelligent Image and Document Information Processing
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//   Author: Zhou Jian
// ***************************************************************
//		Revision history:
//			2010.4.12: v4.0, ����SDK�ĳ�����Ϣ
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

//������ɫ
#define LC_UNKNOWN  0	// δ֪
#define LC_BLUE   1		// ��ɫ
#define LC_YELLOW 2		// ��ɫ
#define LC_WHITE  3		// ��ɫ
#define LC_BLACK  4		// ��ɫ
#define LC_GREEN  5		// ��ɫ
	
//��������
#define LT_UNKNOWN  0   //δ֪����
#define LT_BLUE     1   //����
#define LT_BLACK    2   //����
#define LT_YELLOW   3   //���Ż���
#define LT_YELLOW2  4   //˫�Ż��ƣ���β�ƣ�ũ�ó���
#define LT_POLICE   5   //��������
#define LT_ARMPOL   6   //�侯����
#define LT_INDIVI   7   //���Ի�����
#define LT_ARMY     8   //���ž���
#define LT_ARMY2    9   //˫�ž���
#define LT_EMBASSY  10  //ʹ����
#define LT_HONGKONG 11  //�����
#define LT_TRACTOR  12  //������
#define LT_MACAU    13  //������
#define LT_CHANGNEI 14  //������
#define LT_MINHANG  15  //����

//������ɫ
#define LGRAY_DARK	0	//��
#define LGRAY_LIGHT	1	//ǳ

#define LCOLOUR_WHITE	0	//��	
#define LCOLOUR_SILVER	1	//��(��)
#define LCOLOUR_YELLOW	2	//��
#define LCOLOUR_PINK	3	//��
#define LCOLOUR_RED		4	//��
#define LCOLOUR_GREEN	5	//��
#define LCOLOUR_BLUE	6	//��
#define LCOLOUR_BROWN	7	//�� 
#define LCOLOUR_BLACK	8	//��


//�˶�����
#define DIRECTION_UNKNOWN	0
#define DIRECTION_LEFT	1
#define DIRECTION_RIGHT	2
#define DIRECTION_UP	3
#define DIRECTION_DOWN	4

//ͼ���ʽ��TH_SetImageFormat������cImageFormat������
#define ImageFormatRGB		0			//RGBRGBRGB...
#define ImageFormatBGR		1			//BGRBGRBGR...
#define ImageFormatYUV422	2			//YYYY...UU...VV..	(YV16)
#define ImageFormatYUV420COMPASS 3		//YYYY...UV...		(NV12)
#define ImageFormatYUV420	4			//YYYY...U...V...	(YU12)
#define ImageFormatUYVY	    5			//UYVYUYVYUYVY...	(UYVY)
#define ImageFormatNV21		6			//YYYY...VU...		(NV21)
#define ImageFormatYV12		7			//YYYY...V...U		(YV12)
#define ImageFormatNV16     8           //YYYY...UVUV...    (NV16��YUV422SP) UV�з���������з��򲻱� 

//��������֧�����ö���
//��TH_SetEnabledPlateFormat������dFormat������
#define PARAM_INDIVIDUAL_ON          0		// ���Ի����ƿ���
#define PARAM_INDIVIDUAL_OFF         1		// ���Ի����ƹر�
#define PARAM_TWOROWYELLOW_ON	     2		// ˫���ɫ���ƿ���
#define PARAM_TWOROWYELLOW_OFF	     3		// ˫���ɫ���ƹر�
#define PARAM_ARMPOLICE_ON           4		// �����侯���ƿ���
#define PARAM_ARMPOLICE_OFF          5		// �����侯���ƹر�
#define PARAM_TWOROWARMY_ON          6		// ˫����ӳ��ƿ���
#define PARAM_TWOROWARMY_OFF         7		// ˫����ӳ��ƹر�
#define PARAM_TRACTOR_ON			 8		// ũ�ó����ƿ���
#define PARAM_TRACTOR_OFF			 9		// ũ�ó����ƹر�
#define PARAM_ONLY_TWOROWYELLOW_ON   10		// ֻʶ��˫����ƿ���
#define PARAM_ONLY_TWOROWYELLOW_OFF  11		// ֻʶ��˫����ƹر�
#define PARAM_EMBASSY_ON			 12		// ʹ�ݳ��ƿ���
#define PARAM_EMBASSY_OFF			 13		// ʹ�ݳ��ƹر�
#define PARAM_ONLY_LOCATION_ON		 14		// ֻ��λ���ƿ���
#define PARAM_ONLY_LOCATION_OFF		 15		// ֻ��λ���ƹر�
#define PARAM_ARMPOLICE2_ON			 16		// ˫���侯���ƿ���
#define PARAM_ARMPOLICE2_OFF		 17		// ˫���侯���ƹر�
#define PARAM_CHANGNEI_ON			 18		// ���ڳ��ƿ���
#define PARAM_CHANGNEI_OFF			 19		// ���ڳ��ƹر�
#define PARAM_MINHANG_ON			 20		// �񺽳��ƿ���
#define PARAM_MINHANG_OFF			 21		// �񺽳��ƹر�


#define RECOG_STAGE_ALL			0		// �޴���
#define RECOG_STAGE_FINDPLATE	1		// û���ҵ�����
#define RECOG_STAGE_PLATESCORE_ZERO	2	// ��������ֵ(0��)
#define RECOG_STAGE_PLATESCORE_LOW	3	// ��������ֵ(������)
#define RECOG_STAGE_RECOGSCORE_ZERO	4	// ����ʶ�����(0��)
#define RECOG_STAGE_RECOGSCORE_LOW	5	// ����ʶ�����(������)

	
//��������
#define CarLogo_UNKNOWN       0    //δ֪
#define CarLogo_AUDI          1    //�µ�
#define CarLogo_BMW           2    //����
#define CarLogo_BENZ          3    //����
#define CarLogo_HONDA         4    //����
#define CarLogo_PEUGEOT       5    //��־
#define CarLogo_BUICK         6    //���
#define CarLogo_DASAUTO       7    //����
#define CarLogo_TOYOTA        8    //����
#define CarLogo_FORD          9    //����
#define CarLogo_SUZUKI        10   //��ľ
#define CarLogo_MAZDA         11   //���Դ�
#define CarLogo_KIA           12   //����
#define CarLogo_NISSAN        13   //�ղ���ɣ
#define CarLogo_HYUNDAI       14   //�ִ�
#define CarLogo_CHEVROLET     15   //ѩ����
#define CarLogo_CITROEN       16   //ѩ����

#define CarLogo_QIRUI         17   //����
#define CarLogo_WULING        18   //����
#define CarLogo_DONGFENG      19   //����
#define CarLogo_JIANGHUAI     20   //����
#define CarLogo_BEIQI         21   //����
#define CarLogo_CHANGAN       22   //����
#define CarLogo_AOCHI         23   //�³�
#define CarLogo_SHAOLING      24   //����
#define CarLogo_SHANQI        25   //����
#define CarLogo_SANLING       26   //����
#define CarLogo_JILI          27   //����
#define CarLogo_HAOWO         28   //����
#define CarLogo_HAIMA         29   //����
#define CarLogo_HAFEI         30   //����
#define CarLogo_CHANGCHENG    31   //����
#define CarLogo_FUTIAN        32   //����
#define CarLogo_NANJUN        33   //�Ͽ�
#define CarLogo_LIUQI         34   //����

// ��������
#define CARTYPE_UNKNOWN		0	// δ֪
#define CARTYPE_SALOON		1	// �γ�
#define CARTYPE_VAN			2	// �����

typedef struct TH_RECT
{
	int left;
	int top;
	int right;
	int bottom;
}TH_RECT;


typedef struct TH_PlateIDCfg
{
	int nMinPlateWidth;					// ������С���ƿ�ȣ�������Ϊ��λ
	int nMaxPlateWidth;					// ��������ƿ�ȣ�������Ϊ��λ
	
	int nMaxImageWidth;					// ���ͼ����
	int nMaxImageHeight;				// ���ͼ��߶�

	unsigned char bVertCompress;		// �Ƿ�ֱ����ѹ��1����ʶ��
	unsigned char bIsFieldImage;		// �Ƿ��ǳ�ͼ��
	unsigned char bOutputSingleFrame;	// �Ƿ���Ƶͼ����ͬһ�����Ķ��ͼ��ֻ���һ�ν��		
	unsigned char bMovingImage;			// ʶ���˶�or��ֹͼ��

	unsigned char bIsNight;
	unsigned char nImageFormat;

	unsigned char * pFastMemory;		// DSP�ȵ�Ƭ���ڴ棬��ʱ�����������ʹ����Щ�ڴ�
	int nFastMemorySize;				// �����ڴ�Ĵ�С

	unsigned char *pMemory;				// ��ͨ�ڴ�ĵ�ַ���ڽ����ڴ���������ڴ�й©������
	int nMemorySize;					// ��ͨ�ڴ�Ĵ�С

	int (*DMA_DataCopy)(void *dst, void *src,int nSize);
	int (*Check_DMA_Finished)();

	int nLastError;			// ���ڴ��ݴ�����Ϣ
							// 0: �޴���
							// 1: FindPlate(û���ҵ�����)
							// 2: ��������ֵ(0��)
							// 3: ��������ֵ(������)
							// 4: ����ʶ�����(0��)
	                        // 5: ����ʶ�����(������)
	int nErrorModelSN;		// �����ģ����
	unsigned char nOrderOpt;			//���˳��ѡ�� 0-���Ŷ� 1-���϶��� 2-���¶���
	unsigned char bLeanCorrection;		// �Ƿ����ó�����ת���ܣ�Ĭ�Ͽ���
	unsigned char bMovingOutputOpt;   	// 0-�ڲ�����+�ⲿ��ȡ 1:�ⲿ��ȡ	
	unsigned char nImproveSpeed;        // 0: ʶ�������� 1:ʶ���ٶ�����
	unsigned char bCarLogo;             // 0: ����⳵�� 1: ��⳵��
	unsigned char bLotDetect;			// 0: ����⳵λ 1: ��⳵λ
	unsigned char bShadow;              // 0: �������Ӱ�ĳ��� 1���������Ӱ�ĳ��ƣ�Ĭ�Ͽ���
	unsigned char bUTF8;				// 0:����GBK,1:����UTF-8
	unsigned char bShieldRailing;		// 0: �������˸��ţ� 1:���������˸���
	unsigned char bCarModel;		// 0: ��ʶ���ͣ� 1: ʶ����
	char reserved[110];				// WIN_X86
	//char reserved[110+128];				// WIN_X64

}TH_PlateIDCfg;


typedef struct TH_PlateIDResult 
{
	char license[16];	// �����ַ���
	char color[8];		// ������ɫ

	int nColor;			// ������ɫ
	int nType;			// ��������
	int nConfidence;	// ���ƿ��Ŷ�
	int nBright;		// ��������
	int nDirection;		// �����˶�����0 unknown, 1 left, 2 right, 3 up, 4 down 
	
	TH_RECT rcLocation;				// ��������
	const unsigned char *pbyBits;	/* ��ʶ������Ӧ��ͼƬ�Ļ�����, ֻ�е� bOutputSingleFrame = true ʱ����ָ�����Ч��
									�´�ʶ��󣬸û��������ݱ����ǡ����ó��������ͷŸû�������
									��������С���ڴ��ݽ�����ͼƬ���ݵĳ���*/
	int nTime;						// ʶ���ʱ
	unsigned char nCarBright;		//��������
	unsigned char nCarColor;		//������ɫ
	unsigned char nCarLogo;         //��������
	unsigned char nCarType;			//��������
	unsigned char *pbyPlateBin;     //���ƶ�ֵ���������bit�洢��
	unsigned short nBinPlateWidth;  //��ֵ������г��ƿ��
	unsigned short nBinPlateHeight; //��ֵ������г��Ƹ߶�
	char reserved[70];				//����
	TH_RECT rcLogoLocation;			// ��������
	unsigned short nCarModel;		// ��������
	unsigned short nCarModelConfidence;		//���Ϳ��Ŷ�	
}TH_PlateIDResult;

// The errors that may occur during the use of the SDK
#define		TH_ERR_NONE								0		//û�д���
#define		TH_ERR_GENERIC							1		//ʡ�����ô���
#define		TH_ERR_MEMORYALLOC						2		//�ڴ�������
#define		TH_ERR_INVALIDFORMAT					7		//��֧�ֵ�ͼ���ʽ
#define		TH_ERR_INVALIDWIDTH						8		//ͼ���ȱ�����8��������
#define     TH_ERR_THREADLIMIT						20		//�����߳��������涨����
#define		TH_ERR_NODOG							-1		//û���ҵ����ܹ�
#define		TH_ERR_CARTYPEERROR						-2		//��������ʶ��ģ�����
#define		TH_ERR_READDOG							-3		//��ȡ���ܹ�����
#define		TH_ERR_INVALIDDOG						-4		//���ǺϷ��ļ��ܹ�
#define		TH_ERR_INVALIDUSER						-6		//���ǺϷ��ļ��ܹ��û�
#define		TH_ERR_MOUDLEERROR						-7		//����ʶ��ģ�����
#define     TH_ERR_INVALIDMOUDLE					-8		//ģ��û�кϷ���Ȩ
#define     TH_ERR_BUFFULL							-9		//ʶ�𻺳�������
#define		TH_ERR_INITVEHDETECT					-10		//��ʼ���������ģ�����
#define		TH_ERR_VEHDETECT						-11		//�������ģ�����
#define     TH_ERR_INVALIDCALL						-99		//�Ƿ�����
#define     TH_ERR_EXCEPTION						-100	//�쳣
#define		TH_ERR_INITLIMIT						21		//��ʼ�������������ܹ���� 
#define		TH_ERR_MULTIINSTANCE					22		//����ʶ��ʵ��������

//����Ϊ����ʶ���㷨���ش���
#define		TH_ERR_CARMODEL_PLATELOC_ERR			1001	//����������Ϣ�쳣
#define     TH_ERR_READMODEL						1002	//������ģ���쳣

#ifdef __cplusplus
}	// extern "C"
#endif

#endif // !defined(__TH_ERRORDEF_INCLUDE_H__)
