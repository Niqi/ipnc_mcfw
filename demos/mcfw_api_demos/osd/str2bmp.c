#include <osa_debug.h>
#include <arpa/inet.h>
#include <freetype/ft2build.h>
#include <str2bmp.h>
#include <iconverter.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define COL_MIN		(0)
#define COL_MAX		(100)

typedef struct TagStr2Bmp {
	FT_Library  ftlib;
    FT_Face     ftf_cn[OSD_MENU_TYPE_COUNT];
	struct iconv_ctx ctx;
    Int32         streamId;
	Int32			visible;
	Int32			curMenuIdx;
	Int32			curItemIdx;
	Int32			curLeft;
	Int32			curRight;
}TStr2Bmp;
static TStr2Bmp	gObjTStr2Bmp;
Int32 g_fontsize;
static Int32 OSD_FONT_SIZE_720P = 40;


static Int32 Str2Bmp_Setbmp(osd_char *bmp, FT_GlyphSlot glyph)
{
	Int32 bufsiz;

	if(NULL == glyph || NULL == bmp){
		return OSA_EFAIL;
	}

	bmp->rows        = glyph->bitmap.rows;
	bmp->width       = glyph->bitmap.width;
	bmp->pitch       = glyph->bitmap.pitch;
	bmp->pixel_mode  = glyph->bitmap.pixel_mode;
	bmp->bitmap_left = glyph->bitmap_left;
	bmp->bitmap_top  = glyph->bitmap_top;
	bmp->advance_x   = glyph->advance.x;
	bmp->advance_y   = glyph->advance.y;

	bufsiz = bmp->rows * bmp->pitch;
	memcpy(bmp->buffer, glyph->bitmap.buffer, bufsiz);

	return OSA_SOK;
}

Int32 Str2Bmp_String2BmpBuffer(const Int8 *str, Int32 len, osd_char **ppBmp, Int32 type)
{
	UInt8 *sbuf;
	Int32 ret;

	Int32  i,j;
	osd_char *bmp;

	Int32 font_size = 0;
	if(NULL == str || type >= OSD_MENU_TYPE_COUNT || NULL == *ppBmp)
		return OSA_EFAIL;

	font_size = (*ppBmp)->fontsize ? (*ppBmp)->fontsize : OSD_FONT_SIZE_D1;

	sbuf = (UInt8 *)str;

	if(0 == len || 128 < len){
		OSA_ERROR("string len is invalid, len: %d!\n", len);
		return OSA_EFAIL;
	}

	i = 0;
	j = 0;
	bmp = *ppBmp;
	ushort utmp;
	uint charcode;
	FT_Face ftf;
	FT_GlyphSlot slot;

	while(i < len){
		utmp = 0;
		charcode = 0;
		ftf  = NULL;
		slot = NULL;

		if((sbuf[i] < 128)){
			ftf = gObjTStr2Bmp.ftf_cn[type];
			utmp = sbuf[i];
			FT_Set_Pixel_Sizes(ftf, 0, font_size);
			charcode = FT_Get_Char_Index(ftf, utmp);
			ret=FT_Load_Glyph(ftf, charcode, FT_LOAD_DEFAULT);
			if(ret != 0){
				OSA_printf("FT_Load_Glyph failed!!!\n");
			}
			if(ftf->glyph->format != FT_GLYPH_FORMAT_BITMAP){
				FT_Render_Glyph(ftf->glyph, /*FT_RENDER_MODE_MONO*/FT_RENDER_MODE_NORMAL);
			}
			slot = ftf->glyph;
			i += 1;
			j += 1;
		}
		else{
			ftf = gObjTStr2Bmp.ftf_cn[type];
			iconv_convert_x(&(gObjTStr2Bmp.ctx), (UInt8 *)sbuf+i, 2, (UInt8 *)&utmp, 2, NULL, NULL);
			utmp = htons(utmp);
			FT_Set_Pixel_Sizes(ftf, 0, font_size);
			charcode = FT_Get_Char_Index(ftf, utmp);
			ret = FT_Load_Glyph(ftf, charcode, FT_LOAD_DEFAULT|FT_LOAD_NO_AUTOHINT);
			if(ftf->glyph->format != FT_GLYPH_FORMAT_BITMAP){
				FT_Render_Glyph(ftf->glyph,  /*FT_RENDER_MODE_MONO*/FT_RENDER_MODE_NORMAL);
			}
			slot = ftf->glyph;
			i += 2;
			j += 1;
		}

		if(NULL != slot){
			if (0 != Str2Bmp_Setbmp(bmp, slot)){
				OSA_ERROR("Str2Bmp_Setbmp!");
				goto __flag_exit_err;
			}
			bmp->text_len = len;
			bmp->fontsize = font_size;
			bmp->textPosX = (*ppBmp)->textPosX;
			bmp->textPosY = (*ppBmp)->textPosY;
			bmp->ackNum   = (*ppBmp)->ackNum;

			bmp++;
		}
	}
	(*ppBmp)->bmp_num = j;
	return OSA_SOK;

__flag_exit_err:
	free(*ppBmp);
	*ppBmp = NULL;
	return OSA_EFAIL;
}


void Str2Bmp_Deinitial()
{
	gObjTStr2Bmp.visible    = 0;
	gObjTStr2Bmp.curMenuIdx = 0;
	gObjTStr2Bmp.curItemIdx = 0;
	FT_Done_Face(gObjTStr2Bmp.ftf_cn[OSD_MENU_1080P]);
	FT_Done_Face(gObjTStr2Bmp.ftf_cn[OSD_MENU_D1]);
	FT_Done_FreeType(gObjTStr2Bmp.ftlib);
	iconv_fini_x(&(gObjTStr2Bmp.ctx));
	OSA_printf("Str2Bmp_Initial done.\n");
}

Int32 Str2Bmp_Initial(VideoStd_Type videoStd)
{
	switch (videoStd)
	{
		case VideoStd_1080I_25:
		case VideoStd_1080I_30:
		case VideoStd_1080P_24:
		case VideoStd_1080P_25:
		case VideoStd_1080P_30:
			OSD_FONT_SIZE_720P = 40;
			g_fontsize = 40;
			break;
	    case VideoStd_3M_15:
			OSD_FONT_SIZE_720P = 45;
			g_fontsize = 45;
			break;

		case VideoStd_720P_30:
		case VideoStd_720P_50:
		case VideoStd_720P_60:
			OSD_FONT_SIZE_720P = 30;
			g_fontsize = 30;
			break;

		default:
			OSD_FONT_SIZE_720P = 24;
			g_fontsize = 24;
			break;
	}

	memset(&gObjTStr2Bmp, 0, sizeof(TStr2Bmp));

	if(FT_Init_FreeType(&(gObjTStr2Bmp.ftlib))){
		OSA_ERROR("FT_Init_FreeType!\n");
		return OSA_EFAIL;
	}

	/* 1080p */
	if(FT_New_Face(gObjTStr2Bmp.ftlib, "/opt/ipnc/simhei.ttf", 0, &(gObjTStr2Bmp.ftf_cn[OSD_MENU_1080P]))){
		OSA_ERROR("FT_New_Face!\n");
		return OSA_EFAIL;
	}
	FT_Set_Pixel_Sizes(gObjTStr2Bmp.ftf_cn[OSD_MENU_1080P], 0, OSD_FONT_SIZE_720P);
	FT_Select_Charmap(gObjTStr2Bmp.ftf_cn[OSD_MENU_1080P], FT_ENCODING_UNICODE);

	/* D1 */
	if(FT_New_Face(gObjTStr2Bmp.ftlib, "/opt/ipnc/simhei.ttf", 0, &(gObjTStr2Bmp.ftf_cn[OSD_MENU_D1]))){
		OSA_ERROR("FT_New_Face!\n");
		return OSA_EFAIL;
	}
	FT_Set_Pixel_Sizes(gObjTStr2Bmp.ftf_cn[OSD_MENU_D1], 0, OSD_FONT_SIZE_D1);
	FT_Select_Charmap(gObjTStr2Bmp.ftf_cn[OSD_MENU_D1], FT_ENCODING_UNICODE);
	iconv_init_x(ICONVERTER_CHARSET_GBK, ICONVERTER_CHARSET_UCS_2, 0,   &(gObjTStr2Bmp.ctx));

	gObjTStr2Bmp.visible    = 0;
	gObjTStr2Bmp.curMenuIdx = 0;
	gObjTStr2Bmp.curItemIdx = 0;
	gObjTStr2Bmp.curLeft    = COL_MIN;
	gObjTStr2Bmp.curRight   = COL_MAX;

	return OSA_SOK;
}
