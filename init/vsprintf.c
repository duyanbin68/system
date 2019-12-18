/* passed
* linux/kernel/vsprintf.c
*
* (C) 1991 Linus Torvalds
*/

/************************************************************************/
/*
sprintf\BA\AF\CA\FD\B5\C4ʵ\CF֣\AC\B4󲿷\D6\CAǴ\D3Linuxֱ\BD\D3ȡ\C0\B4\D3õ\C4
\D7\F7\D5ߣ\BAJason_Du 
\D0޸\C4\C8\D5\C6\DA:2013-10-18
email:duyanbin68@163.com
                                                                     */
/************************************************************************/

#include "stdarg.h"

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
* Wirzenius wrote this portably, Torvalds fucked it up :-)
*/

//#include <stdarg.h>		// \B1\EA׼\B2\CE\CA\FDͷ\CEļ\FE\A1\A3\D2Ժ\EA\B5\C4\D0\CEʽ\B6\A8\D2\E5\B1\E4\C1\BF\B2\CE\CA\FD\C1б\ED\A1\A3\D6\F7Ҫ˵\C3\F7\C1\CB-\B8\F6
// \C0\E0\D0\CD(va_list)\BA\CD\C8\FD\B8\F6\BA\EA(va_start, va_arg \BA\CDva_end)\A3\AC\D3\C3\D3\DA
// vsprintf\A1\A2vprintf\A1\A2vfprintf \BA\AF\CA\FD\A1\A3
//#include <string.h>		// \D7ַ\FB\B4\AEͷ\CEļ\FE\A1\A3\D6\F7Ҫ\B6\A8\D2\E5\C1\CBһЩ\D3й\D8\D7ַ\FB\B4\AE\B2\D9\D7\F7\B5\C4Ƕ\C8뺯\CA\FD\A1\A3

/* we use this so that we can do without the ctype library */
/* \CE\D2\C3\C7ʹ\D3\C3\CF\C2\C3\E6\B5Ķ\A8\D2壬\D5\E2\D1\F9\CE\D2\C3ǾͿ\C9\D2Բ\BBʹ\D3\C3ctype \BF\E2\C1\CB */
#define is_digit(c) ((c) >= '0' && (c) <= '9')	// \C5ж\CF\D7ַ\FB\CAǷ\F1\CA\FD\D7\D6\D7ַ\FB\A1\A3

inline int strlen(const char * s)
{
    int length = 0;  
    while (*s++)  
        ++length;  
    return length; 
}

char * strcpy(char * dest, char * src)
{
	char * tmp = dest;
	while(*src){
		*dest++ = *src++;
	}
	return tmp;
}
// \B8ú\AF\CA\FD\BD\AB\D7ַ\FB\CA\FD\D7ִ\AEת\BB\BB\B3\C9\D5\FB\CA\FD\A1\A3\CA\E4\C8\EB\CA\C7\CA\FD\D7ִ\AEָ\D5\EB\B5\C4ָ\D5룬\B7\B5\BB\D8\CAǽ\E1\B9\FB\CA\FDֵ\A1\A3\C1\ED\CD\E2ָ\D5뽫ǰ\D2ơ\A3
static int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit (**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

// \D5\E2\C0ﶨ\D2\E5ת\BB\BB\C0\E0\D0͵ĸ\F7\D6ַ\FB\BAų\A3\CA\FD\A1\A3
#define ZEROPAD 1 /* pad with zero */	/* \CC\EE\B3\E4\C1\E3 */
#define SIGN 2 /* unsigned/signed long */	/* \CE޷\FB\BA\C5/\B7\FB\BAų\A4\D5\FB\CA\FD */
#define PLUS 4 /* show plus */	/* \CF\D4ʾ\BC\D3 */
#define SPACE 8 /* space if plus */	/* \C8\E7\CAǼӣ\AC\D4\F2\D6ÿո\F1 */
#define LEFT 16 /* left justified */	/* \D7\F3\B5\F7\D5\FB */
#define SPECIAL 32 /* 0x */	/* 0x */
#define SMALL 64 /* use 'abcdef' instead of 'ABCDEF' */	/* ʹ\D3\C3Сд\D7\D6ĸ */

// \B3\FD\B2\D9\D7\F7\A1\A3\CA\E4\C8룺n Ϊ\B1\BB\B3\FD\CA\FD\A3\ACbase Ϊ\B3\FD\CA\FD\A3\BB\BD\E1\B9\FB\A3\BAn Ϊ\C9̣\AC\BA\AF\CA\FD\B7\B5\BB\D8ֵΪ\D3\E0\CA\FD\A1\A3
// \B2μ\FB4.5.3 \BD\DA\D3й\D8Ƕ\C8\EB\BB\E3\B1\E0\B5\C4\D0\C5Ϣ\A1\A3

#define do_div(n,base) ({ \
int __res; \
asm( "divl %4": "=a" (n), "=d" (__res): "" (n), "1" (0), "r" (base)); \
__res; })

// \BD\AB\D5\FB\CA\FDת\BB\BBΪָ\B6\A8\BD\F8\D6Ƶ\C4\D7ַ\FB\B4\AE\A1\A3
// \CA\E4\C8룺num-\D5\FB\CA\FD\A3\BBbase-\BD\F8\D6ƣ\BBsize-\D7ַ\FB\B4\AE\B3\A4\B6ȣ\BBprecision-\CA\FD\D7ֳ\A4\B6\C8(\BE\AB\B6\C8)\A3\BBtype-\C0\E0\D0\CDѡ\CF
// \CA\E4\B3\F6\A3\BAstr \D7ַ\FB\B4\AEָ\D5롣

static char *
number (char *str, int num, int base, int size, int precision, int type)
{
	char c, sign,tmp[36];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

// \C8\E7\B9\FB\C0\E0\D0\CDtype ָ\B3\F6\D3\C3Сд\D7\D6ĸ\A3\AC\D4\F2\B6\A8\D2\E5Сд\D7\D6ĸ\BC\AF\A1\A3
// \C8\E7\B9\FB\C0\E0\D0\CDָ\B3\F6Ҫ\D7\F3\B5\F7\D5\FB\A3\A8\BF\BF\D7\F3\B1߽磩\A3\AC\D4\F2\C6\C1\B1\CE\C0\E0\D0\CD\D6е\C4\CC\EE\C1\E3\B1\EA־\A1\A3
// \C8\E7\B9\FB\BD\F8\D6ƻ\F9\CA\FDС\D3\DA2 \BB\F2\B4\F3\D3\DA36\A3\AC\D4\F2\CD˳\F6\B4\A6\C0\ED\A3\ACҲ\BC\B4\B1\BE\B3\CC\D0\F2ֻ\C4ܴ\A6\C0\ED\BB\F9\CA\FD\D4\DA2-32 ֮\BC\E4\B5\C4\CA\FD\A1\A3
	if (type & SMALL)
		digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
// \C8\E7\B9\FB\C0\E0\D0\CDָ\B3\F6Ҫ\CC\EE\C1㣬\D4\F2\D6\C3\D7ַ\FB\B1\E4\C1\BFc='0'\A3\A8Ҳ\BC\B4''\A3\A9\A3\AC\B7\F1\D4\F2c \B5\C8\D3ڿո\F1\D7ַ\FB\A1\A3
// \C8\E7\B9\FB\C0\E0\D0\CDָ\B3\F6\CAǴ\F8\B7\FB\BA\C5\CA\FD\B2\A2\C7\D2\CA\FDֵnum С\D3\DA0\A3\AC\D4\F2\D6÷\FB\BAű\E4\C1\BFsign=\B8\BA\BAţ\AC\B2\A2ʹnum ȡ\BE\F8\B6\D4ֵ\A1\A3
// \B7\F1\D4\F2\C8\E7\B9\FB\C0\E0\D0\CDָ\B3\F6\CAǼӺţ\AC\D4\F2\D6\C3sign=\BCӺţ\AC\B7\F1\D4\F2\C8\F4\C0\E0\D0ʹ\F8\BFո\F1\B1\EA־\D4\F2sign=\BFո񣬷\F1\D4\F2\D6\C30\A1\A3
	c = (type & ZEROPAD) ? '0' : ' ';
	if (type & SIGN && num < 0)
	{
		sign = '-';
		num = -num;
	}
	else
		sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
// \C8\F4\B4\F8\B7\FB\BAţ\AC\D4\F2\BF\ED\B6\C8ֵ\BC\F51\A1\A3\C8\F4\C0\E0\D0\CDָ\B3\F6\CA\C7\CC\D8\CA\E2ת\BB\BB\A3\AC\D4\F2\B6\D4\D3\DAʮ\C1\F9\BD\F8\D6ƿ\ED\B6\C8\D4ټ\F5\C9\D92 λ(\D3\C3\D3\DA0x)\A3\AC
// \B6\D4\D3ڰ˽\F8\D6ƿ\ED\B6ȼ\F51\A3\A8\D3\C3\D3ڰ˽\F8\D6\C6ת\BB\BB\BD\E1\B9\FBǰ\B7\C5һ\B8\F6\C1㣩\A1\A3
	if (sign)
		size--;
	if (type & SPECIAL)
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
// \C8\E7\B9\FB\CA\FDֵnum Ϊ0\A3\AC\D4\F2\C1\D9ʱ\D7ַ\FB\B4\AE='0'\A3\BB\B7\F1\D4\F2\B8\F9\BEݸ\F8\B6\A8\B5Ļ\F9\CA\FD\BD\AB\CA\FDֵnum ת\BB\BB\B3\C9\D7ַ\FB\D0\CEʽ\A1\A3
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0)
			tmp[i++] = digits[do_div (num, base)];
// \C8\F4\CA\FDֵ\D7ַ\FB\B8\F6\CA\FD\B4\F3\D3ھ\AB\B6\C8ֵ\A3\AC\D4򾫶\C8ֵ\C0\A9չΪ\CA\FD\D7ָ\F6\CA\FDֵ\A1\A3
// \BF\ED\B6\C8ֵsize \BC\F5ȥ\D3\C3\D3ڴ\E6\B7\C5\CA\FDֵ\D7ַ\FB\B5ĸ\F6\CA\FD\A1\A3
	if (i > precision)
		precision = i;
	size -= precision;
// \B4\D3\D5\E2\C0\EF\D5\E6\D5\FD\BF\AAʼ\D0γ\C9\CB\F9\D0\E8Ҫ\B5\C4ת\BB\BB\BD\E1\B9\FB\A3\AC\B2\A2\D4\DDʱ\B7\C5\D4\DA\D7ַ\FB\B4\AEstr \D6С\A3
// \C8\F4\C0\E0\D0\CD\D6\D0û\D3\D0\CC\EE\C1\E3(ZEROPAD)\BA\CD\D7\F3\BF\BF\C6루\D7\F3\B5\F7\D5\FB\A3\A9\B1\EA־\A3\AC\D4\F2\D4\DAstr \D6\D0\CA\D7\CF\C8
// \CC\EE\B7\C5ʣ\D3\E0\BF\ED\B6\C8ֵָ\B3\F6\B5Ŀո\F1\CA\FD\A1\A3\C8\F4\D0\E8\B4\F8\B7\FB\BA\C5λ\A3\AC\D4\F2\B4\E6\C8\EB\B7\FB\BAš\A3
	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
// \C8\F4\C0\E0\D0\CDָ\B3\F6\CA\C7\CC\D8\CA\E2ת\BB\BB\A3\AC\D4\F2\B6\D4\D3ڰ˽\F8\D6\C6ת\BB\BB\BD\E1\B9\FBͷһλ\B7\C5\D6\C3һ\B8\F6'0'\A3\BB\B6\F8\B6\D4\D3\DAʮ\C1\F9\BD\F8\D6\C6\D4\F2\B4\E6\B7\C5'0x'\A1\A3
	if (type & SPECIAL)
		if (base == 8)
			*str++ = '0';
		else if (base == 16)
		{
			*str++ = '0';
			*str++ = digits[33];	// 'X'\BB\F2'x'
		}
// \C8\F4\C0\E0\D0\CD\D6\D0û\D3\D0\D7\F3\B5\F7\D5\FB\A3\A8\D7\F3\BF\BF\C6룩\B1\EA־\A3\AC\D4\F2\D4\DAʣ\D3\E0\BF\ED\B6\C8\D6д\E6\B7\C5c \D7ַ\FB\A3\A8'0'\BB\F2\BFո񣩣\AC\BC\FB51 \D0С\A3
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
// \B4\CBʱi \B4\E6\D3\D0\CA\FDֵnum \B5\C4\CA\FD\D7ָ\F6\CA\FD\A1\A3\C8\F4\CA\FD\D7ָ\F6\CA\FDС\D3ھ\AB\B6\C8ֵ\A3\AC\D4\F2str \D6з\C5\C8루\BE\AB\B6\C8ֵ-i\A3\A9\B8\F6'0'\A1\A3
	while (i < precision--)
		*str++ = '0';
// \BD\ABת\CA\FDֵ\BB\BB\BAõ\C4\CA\FD\D7\D6\D7ַ\FB\CC\EE\C8\EBstr \D6С\A3\B9\B2i \B8\F6\A1\A3
	while (i-- > 0)
		*str++ = tmp[i];
// \C8\F4\BF\ED\B6\C8ֵ\C8Դ\F3\D3\DA\C1㣬\D4\F2\B1\EDʾ\C0\E0\D0ͱ\EA־\D6\D0\D3\D0\D7\F3\BF\BF\C6\EB\B1\EA־\B1\EA־\A1\A3\D4\F2\D4\DAʣ\D3\E0\BF\ED\B6\C8\D6з\C5\C8\EB\BFո\F1\A1\A3
	while (size-- > 0)
		*str++ = ' ';
	return str;			// \B7\B5\BB\D8ת\BB\BB\BAõ\C4\D7ַ\FB\B4\AE\A1\A3
}

// \CF\C2\C3溯\CA\FD\CA\C7\CB͸\F1ʽ\BB\AF\CA\E4\B3\F6\B5\BD\D7ַ\FB\B4\AE\D6С\A3
// Ϊ\C1\CB\C4\DC\D4\DA\C4ں\CB\D6\D0ʹ\D3ø\F1ʽ\BB\AF\B5\C4\CA\E4\B3\F6\A3\ACLinus \D4\DA\C4ں\CBʵ\CF\D6\C1˸\C3C \B1\EA׼\BA\AF\CA\FD\A1\A3
// \C6\E4\D6в\CE\CA\FDfmt \CAǸ\F1ʽ\D7ַ\FB\B4\AE\A3\BBargs \CAǸ\F6\CA\FD\B1仯\B5\C4ֵ\A3\BBbuf \CA\C7\CA\E4\B3\F6\D7ַ\FB\B4\AE\BB\BA\B3\E5\C7\F8\A1\A3
// \C7\EB\B2μ\FB\B1\BE\B4\FA\C2\EB\C1б\ED\BA\F3\B5\C4\D3йظ\F1ʽת\BB\BB\D7ַ\FB\B5Ľ\E9\C9ܡ\A3
int
vsprintf_old (char *buf, const char *fmt, va_list args)
{
	int len;
	int i;
	char *str;			// \D3\C3\D3ڴ\E6\B7\C5ת\BB\BB\B9\FD\B3\CC\D6е\C4\D7ַ\FB\B4\AE\A1\A3
	char *s;
	int *ip;

	int flags;			/* flags to number() */
/* number()\BA\AF\CA\FDʹ\D3õı\EA־ */
	int field_width;		/* width of output field */
/* \CA\E4\B3\F6\D7ֶο\ED\B6\C8*/
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
/* min. \D5\FB\CA\FD\CA\FD\D7ָ\F6\CA\FD\A3\BBmax. \D7ַ\FB\B4\AE\D6\D0\D7ַ\FB\B8\F6\CA\FD */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */
/* 'h', 'l',\BB\F2'L'\D3\C3\D3\DA\D5\FB\CA\FD\D7ֶ\CE */
// \CA\D7\CFȽ\AB\D7ַ\FBָ\D5\EBָ\CF\F2buf\A3\ACȻ\BA\F3ɨ\C3\E8\B8\F1ʽ\D7ַ\FB\B4\AE\A3\AC\B6Ը\F7\B8\F6\B8\F1ʽת\BB\BBָʾ\BD\F8\D0\D0\CF\E0Ӧ\B5Ĵ\A6\C0\ED\A1\A3
	for (str = buf; *fmt; ++fmt)
	{
		if (*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}

// \CF\C2\C3\E6ȡ\B5ø\F1ʽָʾ\D7ַ\FB\B4\AE\D6еı\EA־\D3򣬲\A2\BD\AB\B1\EA־\B3\A3\C1\BF\B7\C5\C8\EBflags \B1\E4\C1\BF\D6С\A3
/* process flags */
		flags = 0;
repeat:
		++fmt;			/* this also skips first '%' */
		switch (*fmt)
		{
		case '-':
			flags |= LEFT;
			goto repeat;		// \D7\F3\BF\BF\C6\EB\B5\F7\D5\FB\A1\A3
		case '+':
			flags |= PLUS;
			goto repeat;		// \B7żӺš\A3
		case ' ':
			flags |= SPACE;
			goto repeat;		// \B7ſո\F1\A1\A3
		case '#':
			flags |= SPECIAL;
			goto repeat;		// \CA\C7\CC\D8\CA\E2ת\BB\BB\A1\A3
		case '0':
			flags |= ZEROPAD;
			goto repeat;		// Ҫ\CC\EE\C1\E3(\BC\B4'0')\A1\A3
		}

// ȡ\B5\B1ǰ\B2\CE\CA\FD\D7ֶο\ED\B6\C8\D3\F2ֵ\A3\AC\B7\C5\C8\EBfield_width \B1\E4\C1\BF\D6С\A3\C8\E7\B9\FB\BF\ED\B6\C8\D3\F2\D6\D0\CA\C7\CA\FDֵ\D4\F2ֱ\BD\D3ȡ\C6\E4Ϊ\BF\ED\B6\C8ֵ\A1\A3
// \C8\E7\B9\FB\BF\ED\B6\C8\D3\F2\D6\D0\CA\C7\D7ַ\FB'*'\A3\AC\B1\EDʾ\CF\C2һ\B8\F6\B2\CE\CA\FDָ\B6\A8\BF\ED\B6ȡ\A3\D2\F2\B4˵\F7\D3\C3va_arg ȡ\BF\ED\B6\C8ֵ\A1\A3\C8\F4\B4\CBʱ\BF\ED\B6\C8ֵ
// С\D3\DA0\A3\AC\D4\F2\B8ø\BA\CA\FD\B1\EDʾ\C6\E4\B4\F8\D3б\EA־\D3\F2'-'\B1\EA־\A3\A8\D7\F3\BF\BF\C6룩\A3\AC\D2\F2\B4˻\B9\D0\E8\D4ڱ\EA־\B1\E4\C1\BF\D6\D0\CC\ED\C8\EB\B8ñ\EA־\A3\AC\B2\A2
// \BD\AB\D7ֶο\ED\B6\C8ֵȡΪ\C6\E4\BE\F8\B6\D4ֵ\A1\A3
/* get field width */
		field_width = -1;
		if (is_digit (*fmt))
			field_width = skip_atoi (&fmt);
		else if (*fmt == '*')
		{
/* it's the next argument */
			field_width = va_arg (args, int);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
		}

// \CF\C2\C3\E6\D5\E2\B6δ\FA\C2룬ȡ\B8\F1ʽת\BB\BB\B4\AE\B5ľ\AB\B6\C8\D3򣬲\A2\B7\C5\C8\EBprecision \B1\E4\C1\BF\D6С\A3\BE\AB\B6\C8\D3\F2\BF\AAʼ\B5ı\EA־\CA\C7'.'\A1\A3
// \C6䴦\C0\ED\B9\FD\B3\CC\D3\EB\C9\CF\C3\E6\BF\ED\B6\C8\D3\F2\B5\C4\C0\E0\CBơ\A3\C8\E7\B9\FB\BE\AB\B6\C8\D3\F2\D6\D0\CA\C7\CA\FDֵ\D4\F2ֱ\BD\D3ȡ\C6\E4Ϊ\BE\AB\B6\C8ֵ\A1\A3\C8\E7\B9\FB\BE\AB\B6\C8\D3\F2\D6\D0\CA\C7
// \D7ַ\FB'*'\A3\AC\B1\EDʾ\CF\C2һ\B8\F6\B2\CE\CA\FDָ\B6\A8\BE\AB\B6ȡ\A3\D2\F2\B4˵\F7\D3\C3va_arg ȡ\BE\AB\B6\C8ֵ\A1\A3\C8\F4\B4\CBʱ\BF\ED\B6\C8ֵС\D3\DA0\A3\AC\D4\F2
// \BD\AB\D7ֶξ\AB\B6\C8ֵȡΪ\C6\E4\BE\F8\B6\D4ֵ\A1\A3
/* get the precision */
		precision = -1;
		if (*fmt == '.')
		{
			++fmt;
			if (is_digit (*fmt))
				precision = skip_atoi (&fmt);
			else if (*fmt == '*')
			{
/* it's the next argument */
				precision = va_arg (args, int);
			}
			if (precision < 0)
			precision = 0;
		}

// \CF\C2\C3\E6\D5\E2\B6δ\FA\C2\EB\B7\D6\CE\F6\B3\A4\B6\C8\D0\DE\CAη\FB\A3\AC\B2\A2\BD\AB\C6\E4\B4\E6\C8\EBqualifer \B1\E4\C1\BF\A1\A3\A3\A8h,l,L \B5ĺ\AC\D2\E5\B2μ\FB\C1б\ED\BA\F3\B5\C4˵\C3\F7\A3\A9\A1\A3
/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
		{
			qualifier = *fmt;
			++fmt;
		}

// \CF\C2\C3\E6\B7\D6\CE\F6ת\BB\BBָʾ\B7\FB\A1\A3
		switch (*fmt)
		{
// \C8\E7\B9\FBת\BB\BBָʾ\B7\FB\CA\C7'c'\A3\AC\D4\F2\B1\EDʾ\B6\D4Ӧ\B2\CE\CA\FDӦ\CA\C7\D7ַ\FB\A1\A3\B4\CBʱ\C8\E7\B9\FB\B1\EA־\D3\F2\B1\ED\C3\F7\B2\BB\CA\C7\D7\F3\BF\BF\C6룬\D4\F2\B8\C3\D7ֶ\CEǰ\C3\E6
// \B7\C5\C8\EB\BF\ED\B6\C8\D3\F2ֵ-1 \B8\F6\BFո\F1\D7ַ\FB\A3\ACȻ\BA\F3\D4ٷ\C5\C8\EB\B2\CE\CA\FD\D7ַ\FB\A1\A3\C8\E7\B9\FB\BF\ED\B6\C8\D3򻹴\F3\D3\DA0\A3\AC\D4\F2\B1\EDʾΪ\D7\F3\BF\BF\C6룬\D4\F2\D4\DA
// \B2\CE\CA\FD\D7ַ\FB\BA\F3\C3\E6\CC\ED\BCӿ\ED\B6\C8ֵ-1 \B8\F6\BFո\F1\D7ַ\FB\A1\A3
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg (args, int);
			while (--field_width > 0)
				*str++ = ' ';
			break;

// \C8\E7\B9\FBת\BB\BBָʾ\B7\FB\CA\C7's'\A3\AC\D4\F2\B1\EDʾ\B6\D4Ӧ\B2\CE\CA\FD\CA\C7\D7ַ\FB\B4\AE\A1\A3\CA\D7\CF\C8ȡ\B2\CE\CA\FD\D7ַ\FB\B4\AE\B5ĳ\A4\B6ȣ\AC\C8\F4\C6䳬\B9\FD\C1˾\AB\B6\C8\D3\F2ֵ\A3\AC
// \D4\F2\C0\A9չ\BE\AB\B6\C8\D3\F2=\D7ַ\FB\B4\AE\B3\A4\B6ȡ\A3\B4\CBʱ\C8\E7\B9\FB\B1\EA־\D3\F2\B1\ED\C3\F7\B2\BB\CA\C7\D7\F3\BF\BF\C6룬\D4\F2\B8\C3\D7ֶ\CEǰ\B7\C5\C8\EB(\BF\ED\B6\C8ֵ-\D7ַ\FB\B4\AE\B3\A4\B6\C8)
// \B8\F6\BFո\F1\D7ַ\FB\A1\A3Ȼ\BA\F3\D4ٷ\C5\C8\EB\B2\CE\CA\FD\D7ַ\FB\B4\AE\A1\A3\C8\E7\B9\FB\BF\ED\B6\C8\D3򻹴\F3\D3\DA0\A3\AC\D4\F2\B1\EDʾΪ\D7\F3\BF\BF\C6룬\D4\F2\D4ڲ\CE\CA\FD\D7ַ\FB\B4\AE\BA\F3\C3\E6
// \CC\ED\BC\D3(\BF\ED\B6\C8ֵ-\D7ַ\FB\B4\AE\B3\A4\B6\C8)\B8\F6\BFո\F1\D7ַ\FB\A1\A3
		case 's':
			s = va_arg (args, char *);
			len = strlen (s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			break;

// \C8\E7\B9\FB\B8\F1ʽת\BB\BB\B7\FB\CA\C7'o'\A3\AC\B1\EDʾ\D0轫\B6\D4Ӧ\B5Ĳ\CE\CA\FDת\BB\BB\B3ɰ˽\F8\D6\C6\CA\FD\B5\C4\D7ַ\FB\B4\AE\A1\A3\B5\F7\D3\C3number()\BA\AF\CA\FD\B4\A6\C0\ED\A1\A3
		case 'o':
			str = number (str, va_arg (args, unsigned long), 8,
									field_width, precision, flags);
			break;

// \C8\E7\B9\FB\B8\F1ʽת\BB\BB\B7\FB\CA\C7'p'\A3\AC\B1\EDʾ\B6\D4Ӧ\B2\CE\CA\FD\B5\C4һ\B8\F6ָ\D5\EB\C0\E0\D0͡\A3\B4\CBʱ\C8\F4\B8ò\CE\CA\FDû\D3\D0\C9\E8\D6ÿ\ED\B6\C8\D3\F2\A3\AC\D4\F2Ĭ\C8Ͽ\ED\B6\C8
// Ϊ8\A3\AC\B2\A2\C7\D2\D0\E8Ҫ\CC\ED\C1㡣Ȼ\BA\F3\B5\F7\D3\C3number()\BA\AF\CA\FD\BD\F8\D0д\A6\C0\ED\A1\A3
		case 'p':
			if (field_width == -1)
			{
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number (str, (unsigned long) va_arg (args, void *), 16,
							field_width, precision, flags);
			break;

// \C8\F4\B8\F1ʽת\BB\BBָʾ\CA\C7'x'\BB\F2'X'\A3\AC\D4\F2\B1\EDʾ\B6\D4Ӧ\B2\CE\CA\FD\D0\E8Ҫ\B4\F2ӡ\B3\C9ʮ\C1\F9\BD\F8\D6\C6\CA\FD\CA\E4\B3\F6\A1\A3'x'\B1\EDʾ\D3\C3Сд\D7\D6ĸ\B1\EDʾ\A1\A3
		case 'x':
			flags |= SMALL;
		case 'X':
			str = number (str, va_arg (args, unsigned long), 16,
									field_width, precision, flags);
			break;

// \C8\E7\B9\FB\B8\F1ʽת\BB\BB\D7ַ\FB\CA\C7'd','i'\BB\F2'u'\A3\AC\D4\F2\B1\EDʾ\B6\D4Ӧ\B2\CE\CA\FD\CA\C7\D5\FB\CA\FD\A3\AC'd', 'i'\B4\FA\B1\ED\B7\FB\BA\C5\D5\FB\CA\FD\A3\AC\D2\F2\B4\CB\D0\E8Ҫ\BC\D3\C9\CF
// \B4\F8\B7\FB\BAű\EA־\A1\A3'u'\B4\FA\B1\ED\CE޷\FB\BA\C5\D5\FB\CA\FD\A1\A3
		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			str = number (str, va_arg (args, unsigned long), 10,
									field_width, precision, flags);
			break;

// \C8\F4\B8\F1ʽת\BB\BBָʾ\B7\FB\CA\C7'n'\A3\AC\D4\F2\B1\EDʾҪ\B0ѵ\BDĿǰΪֹת\BB\BB\CA\E4\B3\F6\B5\C4\D7ַ\FB\CA\FD\B1\A3\B4浽\B6\D4Ӧ\B2\CE\CA\FDָ\D5\EBָ\B6\A8\B5\C4λ\D6\C3\D6С\A3
// \CA\D7\CF\C8\C0\FB\D3\C3va_arg()ȡ\B5øò\CE\CA\FDָ\D5룬Ȼ\BA\F3\BD\AB\D2Ѿ\ADת\BB\BB\BAõ\C4\D7ַ\FB\CA\FD\B4\E6\C8\EB\B8\C3ָ\D5\EB\CB\F9ָ\B5\C4λ\D6á\A3
		case 'n':
			ip = va_arg (args, int *);
			*ip = (str - buf);
			break;

// \C8\F4\B8\F1ʽת\BB\BB\B7\FB\B2\BB\CA\C7'%'\A3\AC\D4\F2\B1\EDʾ\B8\F1ʽ\D7ַ\FB\B4\AE\D3д\ED\A3\ACֱ\BDӽ\ABһ\B8\F6'%'д\C8\EB\CA\E4\B3\F6\B4\AE\D6С\A3
// \C8\E7\B9\FB\B8\F1ʽת\BB\BB\B7\FB\B5\C4λ\D6ô\A6\BB\B9\D3\D0\D7ַ\FB\A3\AC\D4\F2Ҳֱ\BDӽ\AB\B8\C3\D7ַ\FBд\C8\EB\CA\E4\B3\F6\B4\AE\D6У\AC\B2\A2\B7\B5\BBص\BD107 \D0м\CC\D0\F8\B4\A6\C0\ED
// \B8\F1ʽ\D7ַ\FB\B4\AE\A1\A3\B7\F1\D4\F2\B1\EDʾ\D2Ѿ\AD\B4\A6\C0\ED\B5\BD\B8\F1ʽ\D7ַ\FB\B4\AE\B5Ľ\E1β\B4\A6\A3\AC\D4\F2\CD˳\F6ѭ\BB\B7\A1\A3
		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			break;
		}
	}
	*str = '\0';			// \D7\EE\BA\F3\D4\DAת\BB\BB\BAõ\C4\D7ַ\FB\B4\AE\BD\E1β\B4\A6\CC\ED\C9\CFnull\A1\A3
	return str - buf;		// \B7\B5\BB\D8ת\BB\BB\BAõ\C4\D7ַ\FB\B4\AE\B3\A4\B6\C8ֵ\A1\A3
}
void my_itoa(int n,char s[])  
{  
    int i = 0,j;
    char tmp[8] = {0};
    do{  
        tmp[i++]=n % 10 +'0'; 
    }while((n /= 10) > 0);       
	i--;
    for(j = i; j >= 0; j--)
	s[j] = tmp[i - j];
//s[5] = tmp [1] ,s[0] = tmp[5 - 0] = 0
}  

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char * p;
	char tmp[256];
	va_list p_next_arg = args;
	
	for(p = buf;*fmt;fmt++){
		if(*fmt != '%'){
			*p++ = *fmt;
			continue;
		}
		fmt++;
		switch (*fmt){
		case 'd':
			my_itoa(tmp,*(int*)p_next_arg);
			strcpy(p,tmp);
			p_next_arg += 4;
			p += strlen(tmp);
			break;
		case 's':
			break;
		default:
			break;
		}
	}
	return (p - buf);
}

int sprintf(char * buffer,const char *fmt, ...)
{
	va_list args;
	int i;
	
	va_start(args, fmt);
	i = vsprintf(buffer, fmt, args);
	va_end(args);
	return i;
}
