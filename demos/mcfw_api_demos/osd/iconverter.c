#include <iconverter.h>

#define RET_ILSEQ      (-1)
#define RET_TOOFEW(n)  (-2-2*(n))
#define RET_ILUNI      (-1)
#define RET_TOOSMALL   (-2)

#define abort() {return -1;}
#define NULL    0

typedef struct {
    unsigned short indx; /* index into big table */
    unsigned short used; /* bitmask of used entries */
} Summary16;

#define ICONVERTER_OPTION_GET_OUT_ILSEQ_CHAR(options) ((options >> 8) & 0xFF)
//all converters
#include "ascii.inc"

/* General multi-byte encodings */
#include "ucs2.inc"
#include "utf8.inc"

/* 8-bit encodings */

/* CJK character sets */
#include "gb2312.inc"
#include "euc_cn.inc"
#include "gbk.inc"

const static struct iconverter_charset_map_entry_s {
    xxx_mb2wc_t mb2wc;
    xxx_flush_t flush;
    xxx_wc2mb_t wc2mb;
    xxx_reset_t reset;
} iconverter_charset_map[] =
{
    {ascii_mbtowc,        NULL,           ascii_wctomb,        NULL            }, /* 0 */
    {euc_cn_mbtowc,       NULL,           euc_cn_wctomb,       NULL            }, /* 11 */
    {gbk_mbtowc,          NULL,           gbk_wctomb,          NULL            }, /* 12 */
    {ucs2_mbtowc,         NULL,           ucs2_wctomb,         NULL            }, /* 34 */
    {utf8_mbtowc,         NULL,           utf8_wctomb,         NULL            }, /* 36 */
    {NULL,                NULL,           NULL,                NULL            }
};

#include <string.h>
#include <assert.h>
#include <stdio.h>

int iconv_init_x(int in_charset, int out_charset, int options, conv_t ctx)
{
	assert(ctx != NULL);
    if (in_charset < 0 || in_charset >= ICONVERTER_CHARSETSIZE)
        return ICONVERTER_INIT_IN_CHARSET_NA;
    if (out_charset < 0 || out_charset >= ICONVERTER_CHARSETSIZE)
        return ICONVERTER_INIT_OUT_CHARSET_NA;

    memset(ctx, 0, sizeof(*ctx));
    ctx->mb2wc = iconverter_charset_map[in_charset].mb2wc;
    ctx->flush = iconverter_charset_map[in_charset].flush;
    ctx->wc2mb = iconverter_charset_map[out_charset].wc2mb;
    ctx->reset = iconverter_charset_map[out_charset].reset;
    ctx->options = options;
    if (!ICONVERTER_OPTION_GET_OUT_ILSEQ_CHAR(options))
    {
        ctx->options = ctx->options | ICONVERTER_OPTION_OUT_ILSEQ_CHAR('?');
    }

	fprintf(stderr, "iconv_init done\n");
    return ICONVERTER_INIT_OK;
}

int iconv_convert_x(conv_t ctx,
                  unsigned char const *in_buf, int in_size,
                  unsigned char *out_buf, int out_size,
                  int *p_in_size_consumed,
                  int *p_out_size_consumed)
{
    ucs4_t wc;
    int in_idx, out_idx;
    int result, prev_result;
    state_t prev_istate;

    assert(ctx != NULL);
    assert(in_buf != NULL);
    assert(out_buf != NULL);

    for ( in_idx = 0, out_idx = 0; in_idx < in_size && out_idx < out_size; )
    {
        prev_istate = ctx->istate;

        result = ctx->mb2wc(ctx, &wc, in_buf + in_idx, in_size - in_idx);
        assert(result <= in_size - in_idx);
        if (result < 0)
        {
            if (result == RET_ILSEQ)
            {
                if (ctx->options & ICONVERTER_OPTION_IGNORE_IN_ILSEQ)
                {
                    ctx->istate = 0;
                    in_idx++;
                    continue;
                } else {
                    result = ICONVERTER_CONVERT_IN_ILSEQ;
                    goto exit;
                }
            }
            else
            {
                result = ICONVERTER_CONVERT_IN_TOO_SMALL;
                goto exit;
            }
        }
        prev_result = result;
        in_idx += prev_result;

        result = ctx->wc2mb(ctx, out_buf + out_idx, wc, out_size - out_idx);
        assert(result <= out_size - out_idx);
        if (result < 0)
        {
            if (result == RET_ILUNI)
            {
                if (ctx->options & ICONVERTER_OPTION_IGNORE_OUT_ILSEQ)
                {
                    out_buf[out_idx++] = ICONVERTER_OPTION_GET_OUT_ILSEQ_CHAR(ctx->options);
                    ctx->ostate = 0;
                    continue;
                }
                else
                {
                    result = ICONVERTER_CONVERT_OUT_ILSEQ;
                    in_idx -= prev_result;
                    ctx->istate = prev_istate;
                    goto exit;
                }
            }
            else
            {
                result = ICONVERTER_CONVERT_OUT_TOO_SMALL;
                in_idx -= prev_result;
                ctx->istate = prev_istate;
                goto exit;
            }
        }
        out_idx += result;
    }
    result = ICONVERTER_CONVERT_OK;

exit:
    if (p_in_size_consumed)
    {
        *p_in_size_consumed = in_idx;
    }
    if (p_out_size_consumed)
    {
        *p_out_size_consumed = out_idx;
    }
    return result;
}

int iconv_fini_x(conv_t ctx)
{
    memset(ctx, 0, sizeof(*ctx));

    return 0;
}

