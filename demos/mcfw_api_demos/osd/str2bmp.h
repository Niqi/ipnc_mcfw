#ifndef __STR2BMP_H__
#define __STR2BMP_H__

#define OSD_FONT_SIZE_D1   	       	16
#define OSD_MENU_1080P				0
#define OSD_MENU_D1					1
#define OSD_MENU_TYPE_COUNT			2

#define MAX_OSD_BMP_NUM				(30)
#define MAX_OSD_LINE_NUM			(5)
#define MAX_OSD_STR_LEN				(64)

typedef enum {
    VideoStd_AUTO = 0,      // Automatically select standard (if supported)
    VideoStd_CIF,           // CIF @ 30 frames per second
    VideoStd_SIF_NTSC,      // SIF @ 30 frames per second
    VideoStd_SIF_PAL,       // SIF @ 25 frames per second
    VideoStd_VGA,           // VGA (640x480) @ 60 frames per second
    VideoStd_D1_NTSC,       // D1 NTSC @ 30 frames per second
    VideoStd_D1_PAL,        // D1 PAL @ 25 frames per second
    VideoStd_480P,          // D1 Progressive NTSC @ 60 frames per second
    VideoStd_576P,          // D1 Progressive PAL @ 50 frames per second
    VideoStd_720P_60,       // 720P @ 60 frames per second
    VideoStd_720P_50,       // 720P @ 50 frames per second
    VideoStd_720P_30,       // 720P @ 30 frames per second
    VideoStd_1080I_30,      // 1080I @ 30 frames per second
    VideoStd_1080I_25,      // 1080I @ 25 frames per second
    VideoStd_1080P_30,      // 1080P @ 30 frames per second
    VideoStd_1080P_25,      // 1080P @ 25 frames per second
    VideoStd_1080P_24,      // 1080P @ 24 frames per second
    VideoStd_3M_15,      	// 3M(2048*1536) @ 15 frames per second
    VideoStd_COUNT
} VideoStd_Type;

typedef struct osd_char_s {
    Int32   rows;
    Int32   width;
    Int32   pitch;
    Int8  	buffer[45*45];
    Int8  	pixel_mode;
    Int8  	len;
    Int8  	bmp_num;
    Int8  	text_len;
    Int8  	flag;
    Int32   bitmap_left;
    Int32   bitmap_top;
    long  	advance_x;
    long  	advance_y;
    Int32 	textPosX;
    Int32 	textPosY;
    Int32 	fontsize;
    UInt32   timeStamp;
    Int16	nRectLeftX;
    Int16	nRectLeftY;
    Int16	nRectWidth;
    Int16	nRectHeight;	
    UInt32 	ackNum;
}osd_char;

typedef enum
{
    OSD_RED = 0,           //��ɫ
    OSD_ORANGE,           //��ɫ
    OSD_YELLOW,           //��ɫ
    OSD_GREEN,           //��ɫ
    OSD_BLUE,           //��ɫ
    OSD_CYAN,           //�壬��ɫ
    OSD_PURPLE,           //��ɫ
    OSD_WHITE,          //��ɫ
    OSD_BLACK,          //��ɫ
    OSD_ORIGIN = 9           //����ɫ���ṩ��osd����ɫʹ��
}TOSDColor;

typedef struct TagOsdRowInfo{
	UInt32 *u32X;	    /* ÿ����ʼ������ */
	UInt32 *u32Y;      /* ÿ�������� */
	UInt32 u32RowCnt;  /* �����õ����� */
}TOsdRowInfo;

typedef struct TagOsdBGInfo{
	TOSDColor 	Background;     /* ������ɫ */
	UInt32 		u32Height;      /* �����߶� */
}TOsdBGInfo;

typedef struct TagOsdWordInfo{
	TOSDColor 	 WordColor;     /* ������ɫ */
	UInt32 		u32WordSize;    /* �����С */
}TOsdWordInfo;

typedef struct TagOsdTextParam{
	TOsdRowInfo	    tRowInfo;
	TOsdBGInfo	    tBGInfo;
	TOsdWordInfo	tWDInfo;
}TOsdTextPram;

typedef struct _ST_OSD_OPT
{
	unsigned char enable;
    TOsdTextPram  param;
}ST_OSD_OPT;

typedef struct TagOSDPrm{
	int detailedInfo;
	int	dateEnable;
	int	timeEnable;
	int	logoEnable;
	int logoPos;
	int	textEnable;
	int textPos;
	char text[24];
	int bmpNum;
	osd_char *bmp;
}TOSDPrm;

extern Int32 g_fontsize;

Int32 Str2Bmp_String2BmpBuffer(const Int8 *str, Int32 len, osd_char **ppBmp, Int32 type);
Int32 Str2Bmp_Initial(VideoStd_Type videoStd);
void  Str2Bmp_Deinitial(void);

#endif  //__STR2BMP_H__
