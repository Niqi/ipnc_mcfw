#ifndef __M3_OSD_H__
#define __M3_OSD_H__

#include "swosdLink_priv.h"

#define OSD_FONT_SIZE_1080P		40
#define OSD_FONT_SIZE_D1		16

typedef enum  FT_Pixel_Mode_{
	FT_PIXEL_MODE_NONE = 0,
	FT_PIXEL_MODE_MONO,
	FT_PIXEL_MODE_GRAY,
	FT_PIXEL_MODE_GRAY2,
	FT_PIXEL_MODE_GRAY4,
	FT_PIXEL_MODE_LCD,
	FT_PIXEL_MODE_LCD_V,

	FT_PIXEL_MODE_MAX	  /* do not remove */

} FT_Pixel_Mode;


void OSD_time_show(UInt32 chId,UInt32 dateTimePrm[],TSWOSD_Char *bmp_time,Int32 l, Int32 t, UInt8 *yuv, Int32 w, Int32 h,Int32 fontsize);
void OSD_textShow(Int32 l, Int32 t, TSWOSD_Char *bmp, UInt8 *yuv, Int32 w, Int32 h);
#endif  //__M3_OSD_H__
