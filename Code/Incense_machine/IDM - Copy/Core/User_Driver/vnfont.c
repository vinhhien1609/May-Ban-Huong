#include <stdint.h>
#include "glcd_font.h"
//#include <stddef.h>

const uint8_t g_vnfont_8x15[] =
{ /*7, la do lon cua ki tu can hien thi*/
    7, 0x0,0x80,0xe0,0x60,0xe0,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* A */         
    6, 0xe0,0xe0,0x20,0x20,0xe0,0xc0,0x1f,0x1f,0x11,0x11,0x1f,0xe,      /* B */
    6, 0xc0,0xe0,0x20,0x20,0x20,0x40,0xf,0x1f,0x10,0x10,0x10,0x8,      /* C */
    7, 0xe0,0xe0,0x20,0x20,0x60,0xc0,0x80,0x1f,0x1f,0x10,0x10,0x18,0xf,0x7,      /* D */
    5, 0xe0,0xe0,0x20,0x20,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* E */
    5, 0xe0,0xe0,0x20,0x20,0x20,0x1f,0x1f,0x1,0x1,0x1,      /* F */
    6, 0xc0,0xe0,0x20,0x20,0x20,0x40,0xf,0x1f,0x10,0x12,0x1e,0x1e,      /* G */
    6, 0xe0,0xe0,0x0,0x0,0xe0,0xe0,0x1f,0x1f,0x1,0x1,0x1f,0x1f,      /* H */
    2, 0xe0, 0xe0, 0x1f, 0x1f,     /* I */
    4, 0x0,0x0,0xe0,0xe0,0x10,0x10,0x1f,0xf,      /* J */
    6, 0xe0,0xe0,0x0,0xc0,0xe0,0x20,0x1f,0x1f,0x3,0xf,0x1c,0x10,      /* K */
    5, 0xe0,0xe0,0x0,0x0,0x0,0x1f,0x1f,0x10,0x10,0x10,      /* L */
    8, 0xe0,0xe0,0xc0,0x0,0x0,0xc0,0xe0,0xe0,0x1f,0x1f,0x1,0x7,0x7,0x1,0x1f,0x1f,      /* M */
    7, 0xe0,0xe0,0xc0,0x0,0x0,0xe0,0xe0,0x1f,0x1f,0x0,0x3,0xc,0x1f,0x1f,      /* N */
    7, 0xc0,0xe0,0x20,0x20,0x20,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* O */
    6, 0xe0,0xe0,0x20,0x20,0xe0,0xc0,0x1f,0x1f,0x2,0x2,0x3,0x1,      /* P */
    8, 0xc0,0xe0,0x20,0x20,0x20,0xe0,0xc0,0x0,0xf,0x1f,0x10,0x10,0x10,0x1f,0x2f,0x20,      /* Q */
    6, 0xe0,0xe0,0x20,0x20,0xe0,0xc0,0x1f,0x1f,0x1,0x3,0x1f,0x1c,      /* R */
    5, 0xc0,0xe0,0x20,0x20,0x40,0x9,0x13,0x13,0x1f,0xe,      /* S */
    6, 0x20,0x20,0xe0,0xe0,0x20,0x20,0x0,0x0,0x1f,0x1f,0x0,0x0,      /* T */
    6, 0xe0,0xe0,0x0,0x0,0xe0,0xe0,0xf,0x1f,0x10,0x10,0x1f,0xf,      /* U */
    7, 0xe0,0xe0,0x0,0x0,0x0,0xe0,0xe0,0x0,0x7,0x1f,0x18,0x1f,0x7,0x0,      /* V */
    12, 0x60,0xe0,0x80,0x0,0x80,0xe0,0xe0,0x80,0x0,0x80,0xe0,0x60,0x0,0x3,0x1f,0x1c,0x7,0x0,0x0,0x7,0x1c,0x1f,0x3,0x0,      /* W */
    7, 0x20,0xe0,0xc0,0x0,0xc0,0xe0,0x20,0x10,0x1c,0xf,0x3,0xf,0x1c,0x10,      /* X */
    6, 0x60,0xe0,0x80,0x80,0xe0,0x60,0x0,0x1,0x1f,0x1f,0x1,0x0,      /* Y */
    6, 0x20,0x20,0x20,0xa0,0xe0,0x60,0x18,0x1c,0x17,0x13,0x10,0x10,      /* Z */
    2, 0x0,0x0,0x3c,0x1c,      /* , */
    7, 0x0,0x80,0xec,0x68,0xec,0x80,0x0,0x1c,0x1f,0x07,0x44,0x7,0x1f,0x1c,      /* Ặ - \x1EB6 */
    7, 0x0,0x80,0xe8,0x64,0xe8,0x80,0x0,0x1c,0x1f,0x07,0x44,0x7,0x1f,0x1c,      /* Ậ - \x1EAC */
    7, 0x0,0x80,0xe0,0x60,0xe0,0x80,0x0,0x1c,0x1f,0x07,0x44,0x7,0x1f,0x1c,      /* Ạ - \x1EA0 */
    7, 0x0,0x80,0xe8,0x64,0xe8,0x85,0x2,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ẩ - \x1EA8 */
    7, 0x0,0x80,0xe8,0x70,0xed,0x82,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ẳ - \x1EB2 */
    7, 0x0,0x80,0xe0,0x6a,0xe4,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ả - \x1EA2 */
    7, 0x0,0x80,0xea,0x71,0xea,0x81,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ẵ - \x1EB4 */
    7, 0x0,0x80,0xea,0x65,0xea,0x81,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ẫ - \x1EAA */
    7, 0x0,0x80,0xe9,0x72,0xe8,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ằ - \x1EB0 */
    7, 0x0,0x80,0xe8,0x72,0xe9,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ắ - \x1EAE */
    7, 0x1,0x82,0xe8,0x64,0xe8,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ầ - \x1EA6 */
    7, 0x0,0x80,0xe8,0x64,0xe8,0x82,0x1,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ấ - \x1EA4 */
    7, 0x0,0x80,0xe8,0x64,0xe8,0x84,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ã - \xC3 */
    7, 0x0,0x80,0xe4,0x68,0xe4,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Ă - \x102 */
    7, 0x0,0x80,0xe8,0x64,0xe8,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Â - \xC2 */
    7, 0x0,0x80,0xe4,0x68,0xe0,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* À - \xC0 */
    7, 0x0,0x80,0xe0,0x68,0xe4,0x80,0x0,0x1c,0x1f,0x07,0x04,0x7,0x1f,0x1c,      /* Á - \xC1 */
    5, 0xe0,0xe0,0x20,0x20,0x20,0x1f,0x1f,0x51,0x11,0x11,       /* Ẹ - \x1EB8 */
    5, 0xe0,0xe8,0x24,0x2d,0x22,0x1f,0x1f,0x11,0x11,0x11,       /* Ể - \x1EC2 */
    5, 0xe0,0xe0,0x2a,0x24,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* Ẻ - \x1EBA */
    5, 0xe0,0xf2,0x29,0x32,0x21,0x1f,0x1f,0x11,0x11,0x11,       /* Ễ - \x1EC4 */
    5, 0xe1,0xea,0x24,0x28,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* Ề - \x1EC0 */
    5, 0xe0,0xe8,0x24,0x2a,0x21,0x1f,0x1f,0x11,0x11,0x11,       /* Ế - \x1EBE */
    5, 0xe0,0xe8,0x24,0x28,0x24,0x1f,0x1f,0x11,0x11,0x11,       /* Ẽ - \x1EBC */
    5, 0xe0,0xe8,0x24,0x28,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* Ê - \xCA */
    5, 0xe0,0xe4,0x28,0x20,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* È - \xC8 */
    5, 0xe0,0xe0,0x28,0x24,0x20,0x1f,0x1f,0x11,0x11,0x11,       /* É - \xC9 */
    5, 0xe0,0xec,0x24,0x2c,0x20,0x1f,0x1f,0x51,0x51,0x11,       /* Ệ - \x1EC6 */
    2, 0xe8, 0xe4, 0x1f, 0x1f,      /* Í \xCD */
    2, 0xe4, 0xe8, 0x1f, 0x1f,      /* Ì \xCC */
    4, 0x08, 0xe4, 0xe8, 0x4, 0x0, 0x1f, 0x1f, 0x0,   /* Ĩ \x128 */
    2, 0xea, 0xe4, 0x1f, 0x1f,    /* Ỉ \x1EC8 */
    2, 0xe0, 0xe0, 0x5f, 0x5f,    /* Ị \x1ECA */
    7, 0xc0,0xe0,0x28,0x24,0x28,0xe0,0xc0,0xf,0x1f,0x10,0x50,0x10,0x1f,0xf,      /* Ộ \x1ED8 */
    7, 0xc0,0xe0,0x20,0x25,0x22,0xe8,0xd0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ở \x1EDE */
    7, 0xc0,0xe4,0x22,0x24,0x22,0xe8,0xd0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ỡ \x1EE0 */
    7, 0xc0,0xe0,0x22,0x24,0x20,0xe8,0xd0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ờ \x1EDC */
    7, 0xc0,0xe0,0x20,0x24,0x22,0xe8,0xd0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ớ \x1EDA */
    7, 0xc0,0xe0,0x20,0x20,0x20,0xe0,0xc0,0xf,0x1f,0x10,0x50,0x10,0x1f,0xf,      /* Ọ \x1ECC */
    7, 0xc0,0xe0,0x28,0x24,0x28,0xe5,0xc2,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ổ \x1ED4 */
    7, 0xc0,0xe0,0x20,0x20,0x20,0xe8,0xd0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ơ \x1A0 */
    7, 0xc0,0xe0,0x20,0x2a,0x24,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ỏ \x1ECE */
    7, 0xc0,0xe0,0x32,0x29,0x32,0xe1,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* ỗ \x1ED6 */
    7, 0xc1,0xe2,0x28,0x24,0x28,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* ồ \x1ED2 */
    7, 0xc0,0xe0,0x28,0x24,0x28,0xe2,0xc1,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* ố \x1ED0 */
    7, 0xc0,0xe0,0x28,0x24,0x28,0xe4,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Õ \xD5 */
    7, 0xc0,0xe0,0x28,0x24,0x28,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ô \xD4 */
    7, 0xc0,0xe0,0x24,0x28,0x20,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ò \xD2 */
    7, 0xc0,0xe0,0x20,0x28,0x24,0xe0,0xc0,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,      /* Ó \xD3 */
    7, 0xc0,0xe0,0x20,0x20,0x20,0xe8,0xd0,0xf,0x1f,0x10,0x50,0x10,0x1f,0xf,      /* Ợ \x1EE2 */
    9, 0xe0,0xe0,0x0,0xa,0x4,0xe0,0xe0,0x70,0x30,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,0x0,0x0,      /* Ử \x1EEC */
    9, 0xe0,0xe0,0x8,0x4,0x8,0xe4,0xe0,0x70,0x30,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,0x0,0x0,      /* Ữ \x1EEE */
    9, 0xe0,0xe0,0x4,0x8,0x0,0xe0,0xe0,0x70,0x30,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,0x0,0x0,      /* Ừ \x1EEA */
    9, 0xe0,0xe0,0x0,0x8,0x4,0xe0,0xe0,0x70,0x30,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,0x0,0x0,      /* Ứ \x1EE8 */
    6, 0xe0,0xe0,0x0,0x0,0xe0,0xe0,0xf,0x1f,0x50,0x50,0x1f,0xf,      /* Ụ \x1EE4 */
    9, 0xe0,0xe0,0x0,0x0,0x0,0xe0,0xe0,0x70,0x30,0xf,0x1f,0x10,0x10,0x10,0x1f,0xf,0x0,0x0,      /* Ư \x1AF */
    6, 0xe0,0xe0,0x0,0xa,0xe4,0xe0,0xf,0x1f,0x10,0x10,0x1f,0xf,      /* Ủ \x1EE6 */
    6, 0xe0,0xe8,0x4,0x8,0xe4,0xe0,0xf,0x1f,0x10,0x10,0x1f,0xf,      /* Ũ \x168 */
    6, 0xe0,0xe0,0x4,0x8,0xe0,0xe0,0xf,0x1f,0x10,0x10,0x1f,0xf,      /* Ù \xD9 */
    6, 0xe0,0xe0,0x0,0x8,0xe4,0xe0,0xf,0x1f,0x10,0x10,0x1f,0xf,      /* Ú \xDA */
    9, 0xe0,0xe0,0x0,0x0,0x0,0xe0,0xe0,0x70,0x30,0xf,0x1f,0x10,0x50,0x10,0x1f,0xf,0x0,0x0,      /* Ự \x1EF0 */
    8, 0x0,0xe0,0xe0,0x20,0x20,0x60,0xc0,0x80,0x1,0x1f,0x1f,0x11,0x10,0x18,0xf,0x7,      /* Đ \x110 */
    8, 0x0,0xc0,0xe0,0x20,0xe0,0xc0,0x0,0x0, 0xc,0x1e,0x13,0x13,0x17,0xc,0x1e,0x10,     /* & */
    2, 0x80,0x80,0x19,0x19,   /* : */
    6, 0x0,0x0,0x0,0xc0,0x30,0x8,0x10,0xc,0x3,0x0,0x0,0x0,  /* / */
    7, 0x10,0x18,0x0c,0x84,0xc4,0x7c,0x38,0x00,0x00,0x00,0x31,0x31,0x0,0x0, /* ? */
};


uint16_t vnfontFindPosition(uint16_t c)
{
    switch (c)
    {
    case 'A':    return 0;
    case 'B':    return 15;
    case 'C':    return 28;
    case 'D':    return 41;
    case 'E':    return 56;
    case 'F':    return 67;
    case 'G':    return 78;
    case 'H':    return 91;
    case 'I':    return 104;
    case 'J':    return 109;
    case 'K':    return 118;
    case 'L':    return 131;
    case 'M':    return 142;
    case 'N':    return 159;
    case 'O':    return 174;
    case 'P':    return 189;
    case 'Q':    return 202;
    case 'R':    return 219;
    case 'S':    return 232;
    case 'T':    return 243;
    case 'U':    return 256;
    case 'V':    return 269;
    case 'W':    return 284;
    case 'X':    return 309;
    case 'Y':    return 324;
    case 'Z':    return 337;
    case ','   : return 350;   /* , */
    case 0x1EB6: return 355;   /* Ặ */
		case 0x80	 : return 355;   /* Ặ */
    case 0x1EAC: return 370;   /* Ậ */
		case 0x81	 : return 370;   /* Ậ */
    case 0x1EA0: return 385;   /* Ạ */
		case 0x82: return 385;   /* Ạ */
    case 0x1EA8: return 400;   /* Ẩ */
		case 0x83	 : return 400;   /* Ẩ */
    case 0x1EB2: return 415;   /* Ẳ */
		case 0x84	 : return 415;   /* Ẳ */
    case 0x1EA2: return 430;   /* Ả */
		case 0x85	 : return 430;   /* Ả */
    case 0x1EB4: return 445;   /* Ẵ */
		case 0x86	 : return 445;   /* Ẵ */
    case 0x1EAA: return 460;   /* Ẫ */
		case 0x87	 : return 460;   /* Ẫ */
    case 0x1EB0: return 475;   /* Ằ */
		case 0x88: return 475;   /* Ằ */
    case 0x1EAE: return 490;   /* Ắ */
		case 0x89: return 490;   /* Ắ */
    case 0x1EA6: return 505;   /* Ầ */
		case 0x8A	 : return 505;   /* Ầ */
    case 0x1EA4: return 520;   /* Ấ */
		case 0x8B  : return 520;   /* Ấ */
    case 0xC3  : return 535;   /* Ã */
    case 0x102 : return 550;   /* Ă */
		case 0x8C  : return 550;   /* Ă */
    case 0xC2  : return 565;   /* Â */
    case 0xC0  : return 580;   /* À */
    case 0xC1  : return 595;   /* Á */
    case 0x1EB8: return 610;   /* Ẹ */
		case 0x8D  : return 610;   /* Ẹ */
    case 0x1EC2: return 621;   /* Ể */
		case 0x8E  : return 621;   /* Ể */
    case 0x1EBA: return 632;   /* Ẻ */
		case 0x8F  : return 632;   /* Ẻ */
    case 0x1EC4: return 643;   /* Ễ */
		case 0x90  : return 643;   /* Ễ */
    case 0x1EC0: return 654;   /* Ề */
		case 0x91  : return 654;   /* Ề */
    case 0x1EBE: return 665;   /* Ế */
		case 0x92  : return 665;   /* Ế */
    case 0x1EBC: return 676;   /* Ẽ */
		case 0x93  : return 676;   /* Ẽ */
    case 0xCA  : return 687;   /* Ê */
    case 0xC8	 : return 698;   /* È */
    case 0xC9  :   return 709;   /* É */
    case 0x1EC6: return 720;   /* Ệ */
		case 0x94  : return 720;   /* Ệ */
    case 0xCD  : return 731;   /* Í */
    case 0xCC  : return 736;   /* Ì */
    case 0x128 : return 741;   /* Ĩ */
		case 0x95  : return 741;   /* Ĩ */
    case 0x1EC8: return 750;   /* Ỉ */
		case 0x96: return 750;   /* Ỉ */
    case 0x1ECA: return 755;   /* Ị */
		case 0x97: return 755;   /* Ị */
    case 0x1ED8: return 760;   /* Ộ */
		case 0x98: return 760;   /* Ộ */
    case 0x1EDE: return 775;   /* Ở */
		case 0x99: return 775;   /* Ở */
    case 0x1EE0: return 790;   /* Ỡ */
		case 0x9A: return 790;   /* Ỡ */
    case 0x1EDC: return 805;   /* Ờ */
		case 0x9B: return 805;   /* Ờ */
    case 0x1EDA: return 820;   /* Ớ */
		case 0x9C: return 820;   /* Ớ */
    case 0x1ECC: return 835;   /* Ọ */
		case 0x9D: return 835;   /* Ọ */
    case 0x1ED4: return 850;   /* Ổ */
		case 0x9E: return 850;   /* Ổ */
    case 0x1A0:  return 865;   /* Ơ */
		case 0x9F:  return 865;   /* Ơ */
    case 0x1ECE: return 880;   /* Ỏ */
		case 0xA0: return 880;   /* Ỏ */
    case 0x1ED6: return 895;   /* Ỗ */
		case 0xA1: return 895;   /* Ỗ */
    case 0x1ED2: return 910;   /* Ồ */
		case 0xA2: return 910;   /* Ồ */
    case 0x1ED0: return 925;   /* Ố */
		case 0xA3: return 925;   /* Ố */
    case 0xD5:   return 940;   /* Õ */
    case 0xD4:   return 955;   /* Ô */
    case 0xD2:   return 970;   /* Ò */
    case 0xD3:   return 985;   /* Ó */
    case 0x1EE2: return 1000;   /* Ợ */
		case 0xA4: return 1000;   /* Ợ */
    case 0x1EEC: return 1015;   /* Ử */
		case 0xA5: return 1015;   /* Ử */
    case 0x1EEE: return 1034;   /* Ữ */
		case 0xA6: return 1034;   /* Ữ */
    case 0x1EEA: return 1053;   /* Ừ */
		case 0xA7: return 1053;   /* Ừ */
    case 0x1EE8: return 1072;   /* Ứ */
		case 0xA8: return 1072;   /* Ứ */
    case 0x1EE4: return 1091;   /* Ụ */
		case 0xA9: return 1091;   /* Ụ */
    case 0x1AF:  return 1104;   /* Ư */
		case 0xAA:  return 1104;   /* Ư */
    case 0x1EE6: return 1123;   /* Ủ */
		case 0xAB: return 1123;   /* Ủ */
    case 0x168:  return 1136;   /* Ũ */
		case 0xAC:  return 1136;   /* Ũ */
    case 0xD9:   return 1149;   /* Ù */
    case 0xDA:   return 1162;   /* Ú */
    case 0x1EF0: return 1175;   /* Ự */
		case 0xAD: return 1175;   /* Ự */
    case 0x110:  return 1194;   /* Đ */
		case 0xAE:  return 1194;   /* Đ */
    case '&':    return 1211;   /* & */
    case ':':    return 1228;   /* : */
    case '/':    return 1233;   /* / */
    case '?':    return 1246;   /* ? */
    }
    return 0;
}

unsigned char char_table[] = {
		/* 'X' 0x0058 charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		0xC3, 	/*  [**    **]  */
		0xC3, 	/*  [**    **]  */
		0x66, 	/*  [ **  ** ]  */
		0x3C, 	/*  [  ****  ]  */
		0x18, 	/*  [   **   ]  */
		0x3C, 	/*  [  ****  ]  */
		0x66, 	/*  [ **  ** ]  */
		0xC3, 	/*  [**    **]  */
		0xC3, 	/*  [**    **]  */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		
		/* 'i' 0x0069 charwidth: 3 */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x60, 	/*  [ **]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		0x60, 	/*  [ **]  */
		0x60, 	/*  [ **]  */
		0x60, 	/*  [ **]  */
		0x60, 	/*  [ **]  */
		0x60, 	/*  [ **]  */
		0x60, 	/*  [ **]  */
		0x00, 	/*  [   ]  */
		0x00, 	/*  [   ]  */
		
		/* 'n' 0x006E charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x7C, 	/*  [ ***** ]  */
		0x76, 	/*  [ *** **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'K' 0x004B charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		0x66, 	/*  [ **  ** ]  */
		0x6C, 	/*  [ ** **  ]  */
		0x78, 	/*  [ ****   ]  */
		0x70, 	/*  [ ***    ]  */
		0x70, 	/*  [ ***    ]  */
		0x78, 	/*  [ ****   ]  */
		0x6C, 	/*  [ ** **  ]  */
		0x66, 	/*  [ **  ** ]  */
		0x63, 	/*  [ **   **]  */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		
		/* 'í' 0x00ED charwidth: 5 */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		0x18, 	/*  [   **]  */
		0x30, 	/*  [  ** ]  */
		0x00, 	/*  [     ]  */
		0x30, 	/*  [  ** ]  */
		0x30, 	/*  [  ** ]  */
		0x30, 	/*  [  ** ]  */
		0x30, 	/*  [  ** ]  */
		0x30, 	/*  [  ** ]  */
		0x30, 	/*  [  ** ]  */
		0x00, 	/*  [     ]  */
		0x00, 	/*  [     ]  */
		
		/* 'h' 0x0068 charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x60, 	/*  [ **    ]  */
		0x60, 	/*  [ **    ]  */
		0x60, 	/*  [ **    ]  */
		0x7C, 	/*  [ ***** ]  */
		0x76, 	/*  [ *** **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'C' 0x0043 charwidth: 8 */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		0x3E, 	/*  [  ***** ]  */
		0x63, 	/*  [ **   **]  */
		0x60, 	/*  [ **     ]  */
		0x60, 	/*  [ **     ]  */
		0x60, 	/*  [ **     ]  */
		0x60, 	/*  [ **     ]  */
		0x60, 	/*  [ **     ]  */
		0x63, 	/*  [ **   **]  */
		0x3E, 	/*  [  ***** ]  */
		0x00, 	/*  [        ]  */
		0x00, 	/*  [        ]  */
		
		/* 'à' 0x00E0 charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x30, 	/*  [  **   ]  */
		0x18, 	/*  [   **  ]  */
		0x00, 	/*  [       ]  */
		0x3C, 	/*  [  **** ]  */
		0x06, 	/*  [     **]  */
		0x3E, 	/*  [  *****]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x3E, 	/*  [  *****]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'o' 0x006F charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x3C, 	/*  [  **** ]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x3C, 	/*  [  **** ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'Q' 0x0051 charwidth: 9 */
		0x00, 0x00, 	/*  [         ]  */
		0x00, 0x00, 	/*  [         ]  */
		0x3E, 0x00, 	/*  [  *****  ]  */
		0x63, 0x00, 	/*  [ **   ** ]  */
		0x63, 0x00, 	/*  [ **   ** ]  */
		0x63, 0x00, 	/*  [ **   ** ]  */
		0x63, 0x00, 	/*  [ **   ** ]  */
		0x63, 0x00, 	/*  [ **   ** ]  */
		0x6F, 0x00, 	/*  [ ** **** ]  */
		0x67, 0x00, 	/*  [ **  *** ]  */
		0x3E, 0x00, 	/*  [  *****  ]  */
		0x03, 0x00, 	/*  [      ** ]  */
		0x00, 0x00, 	/*  [         ]  */
		
		/* 'u' 0x0075 charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x6E, 	/*  [ ** ***]  */
		0x3E, 	/*  [  *****]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'ý' 0x00FD charwidth: 6 */
		0x00, 	/*  [      ]  */
		0x00, 	/*  [      ]  */
		0x18, 	/*  [   ** ]  */
		0x30, 	/*  [  **  ]  */
		0x00, 	/*  [      ]  */
		0x6C, 	/*  [ ** **]  */
		0x6C, 	/*  [ ** **]  */
		0x6C, 	/*  [ ** **]  */
		0x6C, 	/*  [ ** **]  */
		0x38, 	/*  [  *** ]  */
		0x30, 	/*  [  **  ]  */
		0x30, 	/*  [  **  ]  */
		0xE0, 	/*  [***   ]  */
		
		/* 'á' 0x00E1 charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x0C, 	/*  [    ** ]  */
		0x18, 	/*  [   **  ]  */
		0x00, 	/*  [       ]  */
		0x3C, 	/*  [  **** ]  */
		0x06, 	/*  [     **]  */
		0x3E, 	/*  [  *****]  */
		0x66, 	/*  [ **  **]  */
		0x66, 	/*  [ **  **]  */
		0x3E, 	/*  [  *****]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* 'c' 0x0063 charwidth: 7 */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		0x3C, 	/*  [  **** ]  */
		0x66, 	/*  [ **  **]  */
		0x60, 	/*  [ **    ]  */
		0x60, 	/*  [ **    ]  */
		0x66, 	/*  [ **  **]  */
		0x3C, 	/*  [  **** ]  */
		0x00, 	/*  [       ]  */
		0x00, 	/*  [       ]  */
		
		/* ' ' 0x0020 charwidth: 4 */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */
		0x00, 	/*  [    ]  */		
		
};



unsigned int code_offset_table[] = {
		   0, /* 'X' 0x0058 */
		  13, /* 'i' 0x0069 */
		  26, /* 'n' 0x006E */
		  39, /* 'K' 0x004B */
		  52, /* 'í' 0x00ED */
		  65, /* 'h' 0x0068 */
		  78, /* 'C' 0x0043 */
		  91, /* 'à' 0x00E0 */
		 104, /* 'o' 0x006F */
		 117, /* 'Q' 0x0051 */
		 143, /* 'u' 0x0075 */
		 156, /* 'ý' 0x00FD */
		 169, /* 'á' 0x00E1 */
		 182, /* 'c' 0x0063 */
		 195, /* ' ' 0x0020 */
};


unsigned char code_mapping_table[] = {
		  0, /* 00 -   0 - '.' */
		  0, /* 01 -   1 - '.' */
		  0, /* 02 -   2 - '.' */
		  0, /* 03 -   3 - '.' */
		  0, /* 04 -   4 - '.' */
		  0, /* 05 -   5 - '.' */
		  0, /* 06 -   6 - '.' */
		  0, /* 07 -   7 - '.' */
		  0, /* 08 -   8 - '.' */
		  0, /* 09 -   9 - '.' */
		  0, /* 0A -  10 - '.' */
		  0, /* 0B -  11 - '.' */
		  0, /* 0C -  12 - '.' */
		  0, /* 0D -  13 - '.' */
		  0, /* 0E -  14 - '.' */
		  0, /* 0F -  15 - '.' */
		  0, /* 10 -  16 - '.' */
		  0, /* 11 -  17 - '.' */
		  0, /* 12 -  18 - '.' */
		  0, /* 13 -  19 - '.' */
		  0, /* 14 -  20 - '.' */
		  0, /* 15 -  21 - '.' */
		  0, /* 16 -  22 - '.' */
		  0, /* 17 -  23 - '.' */
		  0, /* 18 -  24 - '.' */
		  0, /* 19 -  25 - '.' */
		  0, /* 1A -  26 - '.' */
		  0, /* 1B -  27 - '.' */
		  0, /* 1C -  28 - '.' */
		  0, /* 1D -  29 - '.' */
		  0, /* 1E -  30 - '.' */
		  0, /* 1F -  31 - '.' */
		 14, /* 20 -  32 - ' ' */
		  0, /* 21 -  33 - '!' */
		  0, /* 22 -  34 - '"' */
		  0, /* 23 -  35 - '#' */
		  0, /* 24 -  36 - '$' */
		  0, /* 25 -  37 - '%' */
		  0, /* 26 -  38 - '&' */
		  0, /* 27 -  39 - ''' */
		  0, /* 28 -  40 - '(' */
		  0, /* 29 -  41 - ')' */
		  0, /* 2A -  42 - '*' */
		  0, /* 2B -  43 - '+' */
		  0, /* 2C -  44 - ',' */
		  0, /* 2D -  45 - '-' */
		  0, /* 2E -  46 - '.' */
		  0, /* 2F -  47 - '/' */
		  0, /* 30 -  48 - '0' */
		  0, /* 31 -  49 - '1' */
		  0, /* 32 -  50 - '2' */
		  0, /* 33 -  51 - '3' */
		  0, /* 34 -  52 - '4' */
		  0, /* 35 -  53 - '5' */
		  0, /* 36 -  54 - '6' */
		  0, /* 37 -  55 - '7' */
		  0, /* 38 -  56 - '8' */
		  0, /* 39 -  57 - '9' */
		  0, /* 3A -  58 - ':' */
		  0, /* 3B -  59 - ';' */
		  0, /* 3C -  60 - '<' */
		  0, /* 3D -  61 - '=' */
		  0, /* 3E -  62 - '>' */
		  0, /* 3F -  63 - '?' */
		  0, /* 40 -  64 - '@' */
		  0, /* 41 -  65 - 'A' */
		  0, /* 42 -  66 - 'B' */
		  6, /* 43 -  67 - 'C' */
		  0, /* 44 -  68 - 'D' */
		  0, /* 45 -  69 - 'E' */
		  0, /* 46 -  70 - 'F' */
		  0, /* 47 -  71 - 'G' */
		  0, /* 48 -  72 - 'H' */
		  0, /* 49 -  73 - 'I' */
		  0, /* 4A -  74 - 'J' */
		  3, /* 4B -  75 - 'K' */
		  0, /* 4C -  76 - 'L' */
		  0, /* 4D -  77 - 'M' */
		  0, /* 4E -  78 - 'N' */
		  0, /* 4F -  79 - 'O' */
		  0, /* 50 -  80 - 'P' */
		  9, /* 51 -  81 - 'Q' */
		  0, /* 52 -  82 - 'R' */
		  0, /* 53 -  83 - 'S' */
		  0, /* 54 -  84 - 'T' */
		  0, /* 55 -  85 - 'U' */
		  0, /* 56 -  86 - 'V' */
		  0, /* 57 -  87 - 'W' */
		  0, /* 58 -  88 - 'X' */
		  0, /* 59 -  89 - 'Y' */
		  0, /* 5A -  90 - 'Z' */
		  0, /* 5B -  91 - '[' */
		  0, /* 5C -  92 - '\' */
		  0, /* 5D -  93 - ']' */
		  0, /* 5E -  94 - '^' */
		  0, /* 5F -  95 - '_' */
		  0, /* 60 -  96 - '`' */
		  0, /* 61 -  97 - 'a' */
		  0, /* 62 -  98 - 'b' */
		 13, /* 63 -  99 - 'c' */
		  0, /* 64 - 100 - 'd' */
		  0, /* 65 - 101 - 'e' */
		  0, /* 66 - 102 - 'f' */
		  0, /* 67 - 103 - 'g' */
		  5, /* 68 - 104 - 'h' */
		  1, /* 69 - 105 - 'i' */
		  0, /* 6A - 106 - 'j' */
		  0, /* 6B - 107 - 'k' */
		  0, /* 6C - 108 - 'l' */
		  0, /* 6D - 109 - 'm' */
		  2, /* 6E - 110 - 'n' */
		  8, /* 6F - 111 - 'o' */
		  0, /* 70 - 112 - 'p' */
		  0, /* 71 - 113 - 'q' */
		  0, /* 72 - 114 - 'r' */
		  0, /* 73 - 115 - 's' */
		  0, /* 74 - 116 - 't' */
		 10, /* 75 - 117 - 'u' */
		  0, /* 76 - 118 - 'v' */
		  0, /* 77 - 119 - 'w' */
		  0, /* 78 - 120 - 'x' */
		  0, /* 79 - 121 - 'y' */
		  0, /* 7A - 122 - 'z' */
		  0, /* 7B - 123 - '{' */
		  0, /* 7C - 124 - '|' */
		  0, /* 7D - 125 - '}' */
		  0, /* 7E - 126 - '~' */
		  0, /* 7F - 127 - '' */
		  0, /* 80 - 128 - '€' */
		  0, /* 81 - 129 - '' */
		  0, /* 82 - 130 - '‚' */
		  0, /* 83 - 131 - 'ƒ' */
		  0, /* 84 - 132 - '„' */
		  0, /* 85 - 133 - '…' */
		  0, /* 86 - 134 - '†' */
		  0, /* 87 - 135 - '‡' */
		  0, /* 88 - 136 - 'ˆ' */
		  0, /* 89 - 137 - '‰' */
		  0, /* 8A - 138 - 'Š' */
		  0, /* 8B - 139 - '‹' */
		  0, /* 8C - 140 - 'Œ' */
		  0, /* 8D - 141 - '' */
		  0, /* 8E - 142 - 'Ž' */
		  0, /* 8F - 143 - '' */
		  0, /* 90 - 144 - '' */
		  0, /* 91 - 145 - '‘' */
		  0, /* 92 - 146 - '’' */
		  0, /* 93 - 147 - '“' */
		  0, /* 94 - 148 - '”' */
		  0, /* 95 - 149 - '•' */
		  0, /* 96 - 150 - '–' */
		  0, /* 97 - 151 - '—' */
		  0, /* 98 - 152 - '˜' */
		  0, /* 99 - 153 - '™' */
		  0, /* 9A - 154 - 'š' */
		  0, /* 9B - 155 - '›' */
		  0, /* 9C - 156 - 'œ' */
		  0, /* 9D - 157 - '' */
		  0, /* 9E - 158 - 'ž' */
		  0, /* 9F - 159 - 'Ÿ' */
		  0, /* A0 - 160 - ' ' */
		  0, /* A1 - 161 - '¡' */
		  0, /* A2 - 162 - '¢' */
		  0, /* A3 - 163 - '£' */
		  0, /* A4 - 164 - '¤' */
		  0, /* A5 - 165 - '¥' */
		  0, /* A6 - 166 - '¦' */
		  0, /* A7 - 167 - '§' */
		  0, /* A8 - 168 - '¨' */
		  0, /* A9 - 169 - '©' */
		  0, /* AA - 170 - 'ª' */
		  0, /* AB - 171 - '«' */
		  0, /* AC - 172 - '¬' */
		  0, /* AD - 173 - '­' */
		  0, /* AE - 174 - '®' */
		  0, /* AF - 175 - '¯' */
		  0, /* B0 - 176 - '°' */
		  0, /* B1 - 177 - '±' */
		  0, /* B2 - 178 - '²' */
		  0, /* B3 - 179 - '³' */
		  0, /* B4 - 180 - '´' */
		  0, /* B5 - 181 - 'µ' */
		  0, /* B6 - 182 - '¶' */
		  0, /* B7 - 183 - '·' */
		  0, /* B8 - 184 - '¸' */
		  0, /* B9 - 185 - '¹' */
		  0, /* BA - 186 - 'º' */
		  0, /* BB - 187 - '»' */
		  0, /* BC - 188 - '¼' */
		  0, /* BD - 189 - '½' */
		  0, /* BE - 190 - '¾' */
		  0, /* BF - 191 - '¿' */
		  0, /* C0 - 192 - 'À' */
		  0, /* C1 - 193 - 'Á' */
		  0, /* C2 - 194 - 'Â' */
		  0, /* C3 - 195 - 'Ã' */
		  0, /* C4 - 196 - 'Ä' */
		  0, /* C5 - 197 - 'Å' */
		  0, /* C6 - 198 - 'Æ' */
		  0, /* C7 - 199 - 'Ç' */
		  0, /* C8 - 200 - 'È' */
		  0, /* C9 - 201 - 'É' */
		  0, /* CA - 202 - 'Ê' */
		  0, /* CB - 203 - 'Ë' */
		  0, /* CC - 204 - 'Ì' */
		  0, /* CD - 205 - 'Í' */
		  0, /* CE - 206 - 'Î' */
		  0, /* CF - 207 - 'Ï' */
		  0, /* D0 - 208 - 'Ð' */
		  0, /* D1 - 209 - 'Ñ' */
		  0, /* D2 - 210 - 'Ò' */
		  0, /* D3 - 211 - 'Ó' */
		  0, /* D4 - 212 - 'Ô' */
		  0, /* D5 - 213 - 'Õ' */
		  0, /* D6 - 214 - 'Ö' */
		  0, /* D7 - 215 - '×' */
		  0, /* D8 - 216 - 'Ø' */
		  0, /* D9 - 217 - 'Ù' */
		  0, /* DA - 218 - 'Ú' */
		  0, /* DB - 219 - 'Û' */
		  0, /* DC - 220 - 'Ü' */
		  0, /* DD - 221 - 'Ý' */
		  0, /* DE - 222 - 'Þ' */
		  0, /* DF - 223 - 'ß' */
		  7, /* E0 - 224 - 'à' */
		 12, /* E1 - 225 - 'á' */
		  0, /* E2 - 226 - 'â' */
		  0, /* E3 - 227 - 'ã' */
		  0, /* E4 - 228 - 'ä' */
		  0, /* E5 - 229 - 'å' */
		  0, /* E6 - 230 - 'æ' */
		  0, /* E7 - 231 - 'ç' */
		  0, /* E8 - 232 - 'è' */
		  0, /* E9 - 233 - 'é' */
		  0, /* EA - 234 - 'ê' */
		  0, /* EB - 235 - 'ë' */
		  0, /* EC - 236 - 'ì' */
		  4, /* ED - 237 - 'í' */
		  0, /* EE - 238 - 'î' */
		  0, /* EF - 239 - 'ï' */
		  0, /* F0 - 240 - 'ð' */
		  0, /* F1 - 241 - 'ñ' */
		  0, /* F2 - 242 - 'ò' */
		  0, /* F3 - 243 - 'ó' */
		  0, /* F4 - 244 - 'ô' */
		  0, /* F5 - 245 - 'õ' */
		  0, /* F6 - 246 - 'ö' */
		  0, /* F7 - 247 - '÷' */
		  0, /* F8 - 248 - 'ø' */
		  0, /* F9 - 249 - 'ù' */
		  0, /* FA - 250 - 'ú' */
		  0, /* FB - 251 - 'û' */
		  0, /* FC - 252 - 'ü' */
		 11, /* FD - 253 - 'ý' */
		  0, /* FE - 254 - 'þ' */
		  0, /* FF - 255 - 'ÿ' */
			
			
};


unsigned char code_width_table[] = {
		 8, /* 'X' 0x0058*/
		 3, /* 'i' 0x0069*/
		 7, /* 'n' 0x006E*/
		 8, /* 'K' 0x004B*/
		 5, /* 'í' 0x00ED*/
		 7, /* 'h' 0x0068*/
		 8, /* 'C' 0x0043*/
		 7, /* 'à' 0x00E0*/
		 7, /* 'o' 0x006F*/
		 9, /* 'Q' 0x0051*/
		 7, /* 'u' 0x0075*/
		 6, /* 'ý' 0x00FD*/
		 7, /* 'á' 0x00E1*/
		 7, /* 'c' 0x0063*/
		 4, /* ' ' 0x0020*/
};
unsigned char heigh = 13;

