// 实现一些mythroad平台没有的函数

#include "tcc.h"

int errno;  // 实现标准c库的errno标志
#ifdef INCLUDE_MATH
#include <math.h>  // ldexp
#else
double my_ldexp(double v, int e);
#define ldexp my_ldexp
#endif
/*---------------------------------------------------------------------------*/

#define isdigit(a)  ((a) >= '0' && (a) <= '9')
#define isxdigit(a) (isdigit(a) || ((a) >= 'A' && (a) <= 'Z') || ((a) >= 'a' && (a) <= 'z'))
#define isspace(a)  ((a) == ' ' || (a) == '\t' || (a) == '\n' || (a) == '\v' || (a) == '\f' || (a) == '\r')
#define isalpha(a)  ((unsigned int)(((a) | 0x20) - 'a') < 26u)
#define isupper(a)  ((unsigned int)((a) - 'A') < 26u)

#define ERANGE  34
#define EINVAL  22

#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/** 0xFFFFFFFF */
#endif

#ifndef LONG_MAX
#define	LONG_MAX	((long)(ULONG_MAX >> 1))	/** 0x7FFFFFFF */
#endif

#ifndef LONG_MIN
#define	LONG_MIN	((long)(~LONG_MAX))		/** 0x80000000 */
#endif

/**
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long strtol(const char *nptr, char **endptr, register int base)
{
	const char *s = nptr;
	unsigned long acc;
	unsigned long cutoff;
	int neg = 0, any, cutlim;
    int c;

	/**
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;

	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/**
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
#ifdef SDK_MOD  // 两种写法效果相同，只为规避编译器警告
	cutoff = neg ? 0-(unsigned long)LONG_MIN : LONG_MAX;
#else
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
#endif
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;

	for (acc = 0, any = 0; ; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = 0-acc;

	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);

	return (acc);
}

#define ULLONG_MAX 0xffffffffffffffffULL

/**
 * Convert a string to an unsigned long long integer.
 *
 * Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
unsigned long long my_strtoull(const char *nptr, char **endptr, int base)
{
	const char *s;
    char c;
	unsigned long long acc;
	unsigned long long cutoff;
	int neg, any, cutlim;

	/**
	 * See strtoq for comments as to the logic used.
	 */
	s = nptr;
	do {
		c = *s++;
	} while (isspace((unsigned char)c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}

	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X') &&
	    ((s[1] >= '0' && s[1] <= '9') ||
	    (s[1] >= 'A' && s[1] <= 'F') ||
	    (s[1] >= 'a' && s[1] <= 'f'))) {
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0)
		base = c == '0' ? 8 : 10;
	acc = any = 0;
	if (base < 2 || base > 36)
		goto noconv;

	cutoff = ULLONG_MAX / base;
	cutlim = ULLONG_MAX % base;

	for ( ; ; c = *s++) {
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;

		if (c >= base)
			break;

		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0) {
		acc = ULLONG_MAX;
		errno = ERANGE;
	} else if (!any) {
noconv:
		errno = EINVAL;
	} else if (neg)
		acc = 0-acc;

	if (endptr != NULL)
		*endptr = (char *)(any ? s - 1 : nptr);

	return (acc);
}

// 转换浮点数字符串为浮点数
double my_strtod(const char* s, char** endptr)
{
    register const char* p     = s;
    register long double value = 0.0;
    int                  sign  = 0;
    long double          factor;
    unsigned int         expo;

    while (isspace(*p))  // 跳过前面的空格
        p++;

    if (*p == '-' || *p == '+')
        sign = *p++;  // 把符号赋给字符sign，指针后移。

    // 处理数字字符

    while ((unsigned int)(*p - '0') < 10u)  // 转换整数部分
        value = value * 10 + (*p++ - '0');

    // 如果是正常的表示方式（如：1234.5678）
    if (*p == '.')
    {
        factor = 1.;
        p++;

        while ((unsigned int)(*p - '0') < 10u)
        {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    // 如果是IEEE754标准的格式（如：1.23456E+3）
    if ((*p | 32) == 'e')
    {
        expo   = 0;
        factor = 10.0;

        switch (*++p)
        {
        case '-':
            factor = 0.1;
        case '+':
            p++;
            break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            break;

        default :
            value = 0.0;
            p     = s;
            goto done;
        }

        while ((unsigned int)(*p - '0') < 10u)
            expo = 10 * expo + (*p++ - '0');

        while (1)
        {
            if (expo & 1)
                value *= factor;

            if ((expo >>= 1) == 0)
                break;

            factor *= factor;
        }
    }

done:
    if (endptr != NULL)
        *endptr = (char*)p;

    return (sign == '-' ? -value : value);
}

/*---------------------------------------------------------------------------*/

/*
 * 装载浮点数，v是尾数，e为指数。
 * 如：x=ldexp(1.0,6);则表示要转载的浮点数是1.0*2^6
 */
double my_ldexp(double v, int e)
{
    double two = 2.0;

    if (e < 0) {
       e = -e; /*这句话和后面的if语句是用来对两位溢出码的机器*/
       if (e < 0)
           return 0.0;

       while (e > 0)
       {
         if (e & 1)
             v /= two;
         two *= two;
          e >>= 1;
       }
    } else if (e > 0) {
       while (e > 0)
       {
         if (e & 1)
             v *= two;
         two *= two;
         e >>= 1;
       }
    }

    return v;
}

/*---------------------------------------------------------------------------*/

// 不区分大小写比较字符串
int my_stricmp(const char *dst, const char *src)
{
    int ch1, ch2;

    do {
        if (((ch1 = (unsigned char)(*(dst++))) >= 'A') &&(ch1 <= 'Z'))
            ch1 += 0x20;

        if (((ch2 = (unsigned char)(*(src++))) >= 'A') &&(ch2 <= 'Z'))
            ch2 += 0x20;

    } while (ch1 && (ch1 == ch2));

    return (ch1 - ch2);
}

// 变通实现，不严密
void *my_realloc(void *p, int size)
{
    void *np = NULL;

    if (size > 0) {
        np = malloc(size);
        if (NULL != p && NULL != np)
            memcpy(np, p, size);
    }

    if (NULL != p)
        free(p);

    return np;
}

/*---------------------------------------------------------------------------*/

#define ZEROPAD 1               // Pad with zero
#define SIGN    2               // Unsigned/signed long
#define PLUS    4               // Show plus
#define SPACE   8               // Space if plus
#define LEFT    16              // Left justified
#define SPECIAL 32              // 0x
#define LARGE   64              // Use 'ABCDEF' instead of 'abcdef'

#ifndef abs
#define abs(a)  ((a) < 0 ?  -(a) :(a))
#endif
#define is_digit(c) ((c) >= '0' && (c) <= '9')
#define is_space(c) ((unsigned)((c) - 9) < 5u || ' ' == (c))
/////////////////////////////////////////////////////////////////////////////

static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char upper_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static unsigned long strnlen(const char *s, int count)
{
    const char *sc;

    for (sc = s; *sc != '\0' && count--; ++sc)
        ;

    return sc - s;
}

static int skip_atoi(const char **s)
{
    int i = 0;

    while (is_digit(**s))
    {
        i = i*10 + *((*s)++) - '0';
    }

    return i;
}

static char * number(char *str, long num, int base, int size, int precision, int type)
{
    char c, sign, tmp[66];
    const char *dig = digits;
    int i;

    if (base < 2 || base > 36)
        return 0;

    if (type & LARGE) 
        dig = upper_digits;

    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;

    if (type & SIGN)
    {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }

    if (type & SPECIAL)
    {
        if (16 == base)
            size -= 2;
        else if (8 == base)
            size--;
    }

    i = 0;

    if (0 == num) {
        tmp[i++] = '0';
    } else {
        while (num != 0)
        {
            tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
            num = ((unsigned long) num) / (unsigned) base;
        }
    }

    if (i > precision) 
        precision = i;
    size -= precision;

    if (!(type & (ZEROPAD | LEFT)))
    {
        while(size-- > 0)
            *str++ = ' ';
    }

    if (sign)
        *str++ = sign;

    if (type & SPECIAL)
    {
        if (8 == base) {
            *str++ = '0';
        } else if (16 == base) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }

    if(!(type & LEFT))
    {
        while(size-- > 0) *str++ = c;
    }

    while(i < precision--)
        *str++ = '0';

    while(i-- > 0)
        *str++ = tmp[i];

    while(size-- > 0)
        *str++ = ' ';

    return str;
}

int my_vsprintf(char *buf, const char *fmt, va_list args)
{
    char *str;
    int field_width;      /* Width of output field */

    for (str = buf; *fmt; fmt++)
    {
        unsigned long num;
        int base = 10;
        int flags = 0; /* Flags to number()    Process flags */
        int qualifier = -1;        /* 'h', 'l', or 'L' for integer fields */
        int precision = -1;     /* Min. # of digits for integers; max number of chars for from string */
        int bFmt = 1;

        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        bFmt = 1;
        while (bFmt)
        {
            fmt++; /* This also skips first '%' */
            switch (*fmt)
            {
            case '-': flags |= LEFT; break;
            case '+': flags |= PLUS; break;
            case ' ': flags |= SPACE; break;
            case '#': flags |= SPECIAL; break;
            case '0': flags |= ZEROPAD; break;
            default:  bFmt = 0;
            }
        }

        /* Get field width */
        field_width = -1;
        if (is_digit(*fmt)) {
            field_width = skip_atoi(&fmt);
        } else if ('*' == *fmt) {
            fmt++;
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* Get the precision */
        precision = -1;
        if ('.' == *fmt)
        {
            ++fmt;
            if (is_digit(*fmt)) {
                precision = skip_atoi(&fmt);
            } else if ('*' == *fmt) {
                ++fmt;
                precision = va_arg(args, int);
            }

            if (precision < 0)
                precision = 0;
        }

        /* Get the conversion qualifier */
        qualifier = -1;
        if ('h' == *fmt || 'l' == *fmt || 'L' == *fmt)
        {
            qualifier = *fmt;
            fmt++;
        }

        /* Default base */
        base = 10;
        switch (*fmt)
        {
        case 'c':
            {
                if (!(flags & LEFT))
                {
                    while (--field_width > 0)
                        *str++ = ' ';
                }

                *str++ = (unsigned char) va_arg(args, int);

                while (--field_width > 0)
                    *str++ = ' ';
                continue;
            }

        case 's':
            {
                int i;
                int len;
                char *s = va_arg(args, char *);
                if (!s) 
                    s = "<NULL>";
                len = strnlen(s, precision);
                if (!(flags & LEFT))
                {
                    while (len < field_width--)
                        *str++ = ' ';
                }

                for (i = 0; i < len; ++i)
                    *str++ = *s++;

                while (len < field_width--) 
                    *str++ = ' ';
                continue;
            }

        case 'p':
            {
                if (-1 == field_width)
                {
                    field_width = 2 * sizeof(void *);
                    flags |= ZEROPAD;
                }
                str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
                continue;
            }

        case 'n':
            {
                if ('l' == qualifier) {
                    long *ip = va_arg(args, long *);
                    *ip = (str - buf);
                } else {
                    int *ip = va_arg(args, int *);
                    *ip = (str - buf);
                }
                continue;
            }

            /* Integer number formats - set up the flags and "break" */
        case 'o':
            {
                base = 8;
                break;
            }

        case 'X':
            {
                flags |= LARGE; /* no break */
            }
        case 'x':
            {
                base = 16;
                break;
            }

        case 'd':
        case 'i':
            {
                flags |= SIGN; /* no break */
            }
        case 'u':
            {
                break;
            }

        default:
            {
                if (*fmt != '%')
                    *str++ = '%';

                if (*fmt) {
                    *str++ = *fmt;
                } else {
                    --fmt;
                }
                continue;
            }
        }  /* end of switch (*fmt) */

        if (qualifier == 'l') {
            num = va_arg(args, unsigned long);
        } else if (qualifier == 'h') {
            if (flags & SIGN)
                num = va_arg(args, short);
            else
                num = va_arg(args, unsigned short);
        } else if (flags & SIGN) {
            num = va_arg(args, int);
        } else {
            num = va_arg(args, unsigned long);
        }

        str = number(str, num, base, field_width, precision, flags);
    } /* end of for (str = buf; *fmt; fmt++) */

    *str = '\0';
    return str - buf;
}
