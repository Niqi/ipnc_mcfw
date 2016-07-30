#ifndef __I_CHARSET_CONVERTER_H__
#define __I_CHARSET_CONVERTER_H__

typedef unsigned int	 ucs4_t;
typedef unsigned int     state_t;
typedef	struct iconv_ctx *conv_t;

typedef int (*xxx_mb2wc_t) (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n);
typedef int (*xxx_flush_t) (conv_t conv, ucs4_t *pwc);
typedef int (*xxx_wc2mb_t) (conv_t conv, unsigned char *r, ucs4_t wc, int n);
typedef int (*xxx_reset_t) (conv_t conv, unsigned char *r, int n);

struct iconv_ctx {
	state_t istate;
	state_t ostate;
	xxx_mb2wc_t	 mb2wc;
	xxx_flush_t  flush;
	xxx_wc2mb_t	 wc2mb;
	xxx_reset_t	 reset;
	int options;
};

#define ICONVERTER_CHARSET_ASCII       0
#define ICONVERTER_CHARSET_GB2312      1
#define ICONVERTER_CHARSET_GBK         2
#define ICONVERTER_CHARSET_UCS_2       3
#define ICONVERTER_CHARSET_UTF_8       4
#define ICONVERTER_CHARSETSIZE         5

/*< ignore incorrect input sequences */
#define ICONVERTER_OPTION_IGNORE_IN_ILSEQ	1
/*< replace sequence which can't be converted to OUT charset with OUTIL_CHAR */
#define ICONVERTER_OPTION_IGNORE_OUT_ILSEQ	2
#define ICONVERTER_OPTION_OUT_ILSEQ_CHAR(ch) (ch << 8)

#ifdef __cplusplus
extern "C"
{
#endif

#define ICONVERTER_INIT_OK			   0
#define ICONVERTER_INIT_IN_CHARSET_NA  -1
#define ICONVERTER_INIT_OUT_CHARSET_NA -2

extern int iconv_init_x(int in_charset, int out_charset, int options, conv_t ctx);

#define ICONVERTER_CONVERT_OK            0
#define ICONVERTER_CONVERT_IN_TOO_SMALL  -1
#define ICONVERTER_CONVERT_OUT_TOO_SMALL -2
#define ICONVERTER_CONVERT_IN_ILSEQ      -3
#define ICONVERTER_CONVERT_OUT_ILSEQ     -4

extern int iconv_convert_x(conv_t ctx,
                  unsigned char const *in_buf, int in_size,
                  unsigned char *out_buf, int out_size,
                  int *p_in_size_consumed,
                  int *p_out_size_consumed);

extern int iconv_fini_x(conv_t ctx);

#ifdef __cplusplus
} ;
#endif

#endif  //__I_CHARSET_CONVERTER_H__
