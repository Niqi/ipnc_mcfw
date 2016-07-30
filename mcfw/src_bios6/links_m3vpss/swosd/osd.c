#include "osd.h"

static inline void osd_put_pixel_420_blk(Int32 x, Int32 y, Int8 *yuv, Int32 w, Int32 h, Int32 m)
{
    yuv[(w*(y-1) + x)]  = m ? 16 : 255;
}

inline Int32 __osd_get_font_siz(Int32 w)
{
	if(w > 1000){
		return OSD_FONT_SIZE_1080P;
	}
	else{
		return OSD_FONT_SIZE_D1;
	}
}

static inline Int32 osd_bmp_mask(Int32 x, Int32 y, TSWOSD_Char *bmp, UInt8 *yuv, Int32 w, Int32 h)
{
	static const UInt8 y_max = 0x40;

	Int32 l, t;
	Int32 xmax, ymax;
	Int32 cnt, cnt_max;
	Int32 i, j;
	l = x + bmp->bitmap_left;
	if( l < 0 )
		l = 0;
	t = y + __osd_get_font_siz(w) - bmp->bitmap_top;
	if( t < 0 )
		t = 0;

	xmax = l + bmp->width;
	if(xmax > w)
		xmax = w;

	ymax = t + bmp->rows;
	if(ymax > h)
		ymax = h;

	cnt = 0;
	//cnt_max = (bmp->width) * (bmp->rows) / 5;
	cnt_max = (bmp->width) * (bmp->rows) * 3 / 4;

	for(i = t; i < ymax; i++)
	{
		for(j = l; j < xmax; j++)
		{
			if(yuv[(w * i + j)] > y_max)
			{
				cnt++;

				if(cnt > cnt_max)
				{
					return 1;
				}
			}

		}
	}

	return 0;
}

static inline void osd_draw_bmp_blk(TSWOSD_Char *pBmp, Int32 x, Int32 y, Int8 *yuv, Int32 w, Int32 h)
{
	Int32 i, j;
	Int32 xmax, ymax;
	Int32 xorig, yorig;
	Int32 p, q;
	Int32 m;

	if(NULL == yuv || NULL == pBmp)
		return;
	m = osd_bmp_mask(x, y, pBmp, (UInt8 *)yuv, w, h);
	xorig = x + pBmp->bitmap_left;
	if(xorig < 0)
		xorig = 0;
	yorig = y + pBmp->fontsize - pBmp->bitmap_top;
	if(yorig < 0)
		yorig = 0;
	xmax = xorig + pBmp->width;
	if(xmax >= w)
		xmax = w;
	ymax = yorig + pBmp->rows;
	if(ymax >= h)
		ymax = h;
	if(FT_PIXEL_MODE_MONO == pBmp->pixel_mode)
	{
		for(j = yorig, q = 0; j < ymax; j++, q++)
		{
			for(i = xorig, p = 0; i < xmax; i++, p++)
			{
				if(pBmp->buffer[q * pBmp->pitch + p / 8] & (0x80 >> (p % 8)))
				{
					osd_put_pixel_420_blk(i, j, yuv, w, h, m);
				}
			}
		}
	}
	else
	{
		for(j = yorig, q = 0; j < ymax; j++, q++)
		{
			for(i = xorig, p = 0; i < xmax; i++, p++)
			{

				if(pBmp->buffer[q * pBmp->pitch + p])
				{
					osd_put_pixel_420_blk(i, j, yuv, w, h, m);
				}
			}
		}
    }
}


void OSD_textShow(Int32 l, Int32 t, TSWOSD_Char *bmp, UInt8 *yuv, Int32 w, Int32 h)
{
	Int32 x, y;
	TSWOSD_Char *p;
	Int32 cnt = 0;

	if (NULL == bmp || NULL == yuv) return ;
	x = l;
	y = t;
	p = bmp;

	while (cnt < (bmp->len))
	{
		osd_draw_bmp_blk(p, x, y, (Int8 *)yuv, w, h);
		x += (p->advance_x)>>6;

		p++;
		cnt++;
	}
}

void OSD_time_show(UInt32 chId,UInt32 dateTimePrm[],TSWOSD_Char *bmp_time, Int32 l, Int32 t,UInt8 *yuv, Int32 w, Int32 h,Int32 fontsize)
{
	TSWOSD_Char *p[25];
	Int32 tmp,textlen;

	if (NULL == yuv)
	{
		return;
	}


	if(chId == 0)
	{
		textlen = fontsize*17;
		if ((l + textlen ) > w ) l = (w - textlen);
		if ((t + 40) > h ) t = (h - 45);
	}
	else if(chId == 1)
	{
		textlen = 16*17;
		if ((l + textlen ) > w ) l = (w - textlen);
		if ((t + 20) > h ) t = (h - 23);
		t = t -20;
	}
	// year
	tmp = (dateTimePrm[0] ) / 1000 + 2;
	p[0] = &(bmp_time[tmp]);
	tmp = ((dateTimePrm[0]) % 1000) / 100 + 2;
	p[1] = &(bmp_time[tmp]);
	tmp = (((dateTimePrm[0] ) % 1000) % 100) / 10 + 2;
	p[2] = &(bmp_time[tmp]);
	tmp = (((dateTimePrm[0] ) % 1000) % 100) % 10 + 2;
	p[3] = &(bmp_time[tmp]);
	p[4] = &(bmp_time[14]);

	// month
	tmp = (dateTimePrm[1] ) / 10 + 2;
	p[5] = &(bmp_time[tmp]);
	tmp = (dateTimePrm[1] ) % 10 + 2;
	p[6] = &(bmp_time[tmp]);
	p[7] = &(bmp_time[15]);

    // day
	tmp = dateTimePrm[2] / 10 + 2;
	p[8] = &(bmp_time[tmp]);
	tmp = dateTimePrm[2] % 10 + 2;
	p[9] = &(bmp_time[tmp]);
	p[10] = &(bmp_time[16]);

	// blank
	p[11] = &(bmp_time[0]);

	// hour
	tmp = dateTimePrm[3] / 10 + 2;
	p[12] = &(bmp_time[tmp]);
	tmp = dateTimePrm[3] % 10 + 2;
	p[13] = &(bmp_time[tmp]);
	p[14] = &(bmp_time[17]);

	// min
	tmp = dateTimePrm[4] / 10 + 2;
	p[15] = &(bmp_time[tmp]);
	tmp = dateTimePrm[4] % 10 + 2;
	p[16] = &(bmp_time[tmp]);
	p[17] = &(bmp_time[18]);

	// sec
	tmp = dateTimePrm[5] / 10 + 2;
	p[18] = &(bmp_time[tmp]);
	tmp = dateTimePrm[5] % 10 + 2;
	p[19] = &(bmp_time[tmp]);
	p[20] = &(bmp_time[19]);

	// week
	p[21] = &(bmp_time[0]);
	p[22] = &(bmp_time[20]);
	p[23] = &(bmp_time[21]);

	tmp = dateTimePrm[6] + 22;
	p[24] = &(bmp_time[tmp]);
	//Vps_printf("dateTimePrm[6] = %d\n",dateTimePrm[6]);

	Int32 x, y;
	Int32 i = 0;
	x = l;
	y = t;
	for (i = 0; i < 21; i++)
	{
		osd_draw_bmp_blk(p[i], x, y, (Int8 *)yuv, w, h);

		x += (p[i]->advance_x) >> 6;
	}

	return ;
}
