#ifndef __PDCURSES_CHARS_H__
#define __PDCURSES_CHARS_H__ 1


/*man-start**************************************************************

PDCurses Text Attributes
========================

Originally, PDCurses used a short (16 bits) for its chtype. To include
color, a number of things had to be sacrificed from the strict Unix and
System V support. The main problem was fitting all character attributes
and color into an unsigned char (all 8 bits!).

Today, PDCurses by default uses a long (32 bits) for its chtype, as in
System V. The short chtype is still available, by undefining CHTYPE_LONG
and rebuilding the library.

The following is the structure of a win->_attrs chtype:

short form:

-------------------------------------------------
|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
-------------------------------------------------
  color number |  attrs |   character eg 'a'

The available non-color attributes are bold, reverse and blink. Others
have no effect. The high order char is an index into an array of
physical colors (defined in color.c) -- 32 foreground/background color
pairs (5 bits) plus 3 bits for other attributes.

long form:

----------------------------------------------------------------------------
|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|..| 3| 2| 1| 0|
----------------------------------------------------------------------------
      color number      |     modifiers         |      character eg 'a'

The available non-color attributes are bold, underline, invisible,
right-line, left-line, protect, reverse and blink. 256 color pairs (8
bits), 8 bits for other attributes, and 16 bits for character data.

**man-end****************************************************************/

/*** Video attribute macros ***/

#define A_NORMAL      (chtype)0

#ifdef CHTYPE_LONG
# define A_ALTCHARSET (chtype)0x00010000
# define A_RIGHTLINE  (chtype)0x00020000
# define A_LEFTLINE   (chtype)0x00040000
# define A_INVIS      (chtype)0x00080000
# define A_UNDERLINE  (chtype)0x00100000
# define A_REVERSE    (chtype)0x00200000
# define A_BLINK      (chtype)0x00400000
# define A_BOLD       (chtype)0x00800000

# define A_ATTRIBUTES (chtype)0xffff0000
# define A_CHARTEXT   (chtype)0x0000ffff
# define A_COLOR      (chtype)0xff000000

# define A_ITALIC     A_INVIS
# define A_PROTECT    (A_UNDERLINE | A_LEFTLINE | A_RIGHTLINE)

# define PDC_ATTR_SHIFT  19
# define PDC_COLOR_SHIFT 24
#else
# define A_BOLD       (chtype)0x0100  /* X/Open */
# define A_REVERSE    (chtype)0x0200  /* X/Open */
# define A_BLINK      (chtype)0x0400  /* X/Open */

# define A_ATTRIBUTES (chtype)0xff00  /* X/Open */
# define A_CHARTEXT   (chtype)0x00ff  /* X/Open */
# define A_COLOR      (chtype)0xf800  /* System V */

# define A_ALTCHARSET A_NORMAL        /* X/Open */
# define A_PROTECT    A_NORMAL        /* X/Open */
# define A_UNDERLINE  A_NORMAL        /* X/Open */

# define A_LEFTLINE   A_NORMAL
# define A_RIGHTLINE  A_NORMAL
# define A_ITALIC     A_NORMAL
# define A_INVIS      A_NORMAL

# define PDC_ATTR_SHIFT   8
# define PDC_COLOR_SHIFT 11
#endif

#define A_STANDOUT    (A_REVERSE | A_BOLD) /* X/Open */
#define A_DIM         A_NORMAL

#define CHR_MSK       A_CHARTEXT           /* Obsolete */
#define ATR_MSK       A_ATTRIBUTES         /* Obsolete */
#define ATR_NRM       A_NORMAL             /* Obsolete */

/* For use with attr_t -- X/Open says, "these shall be distinct", so
   this is a non-conforming implementation. */

#define WA_ALTCHARSET A_ALTCHARSET
#define WA_BLINK      A_BLINK
#define WA_BOLD       A_BOLD
#define WA_DIM        A_DIM
#define WA_INVIS      A_INVIS
#define WA_LEFT       A_LEFTLINE
#define WA_PROTECT    A_PROTECT
#define WA_REVERSE    A_REVERSE
#define WA_RIGHT      A_RIGHTLINE
#define WA_STANDOUT   A_STANDOUT
#define WA_UNDERLINE  A_UNDERLINE

#define WA_HORIZONTAL A_NORMAL
#define WA_LOW        A_NORMAL
#define WA_TOP        A_NORMAL
#define WA_VERTICAL   A_NORMAL

/*** Alternate character set macros ***/

/* 'w' = 32-bit chtype; acs_map[] index | A_ALTCHARSET
   'n' = 16-bit chtype; it gets the fallback set because no bit is
         available for A_ALTCHARSET */

#ifdef CHTYPE_LONG
# define ACS_PICK(w, n) ((chtype)w | A_ALTCHARSET)
#else
# define ACS_PICK(w, n) ((chtype)n)
#endif

/* VT100-compatible symbols -- box chars */

#define ACS_ULCORNER  ACS_PICK('l', '+')
#define ACS_LLCORNER  ACS_PICK('m', '+')
#define ACS_URCORNER  ACS_PICK('k', '+')
#define ACS_LRCORNER  ACS_PICK('j', '+')
#define ACS_RTEE      ACS_PICK('u', '+')
#define ACS_LTEE      ACS_PICK('t', '+')
#define ACS_BTEE      ACS_PICK('v', '+')
#define ACS_TTEE      ACS_PICK('w', '+')
#define ACS_HLINE     ACS_PICK('q', '-')
#define ACS_VLINE     ACS_PICK('x', '|')
#define ACS_PLUS      ACS_PICK('n', '+')

/* VT100-compatible symbols -- other */

#define ACS_S1        ACS_PICK('o', '-')
#define ACS_S9        ACS_PICK('s', '_')
#define ACS_DIAMOND   ACS_PICK('`', '+')
#define ACS_CKBOARD   ACS_PICK('a', ':')
#define ACS_DEGREE    ACS_PICK('f', '\'')
#define ACS_PLMINUS   ACS_PICK('g', '#')
#define ACS_BULLET    ACS_PICK('~', 'o')

/* Teletype 5410v1 symbols -- these are defined in SysV curses, but
   are not well-supported by most terminals. Stick to VT100 characters
   for optimum portability. */

#define ACS_LARROW    ACS_PICK(',', '<')
#define ACS_RARROW    ACS_PICK('+', '>')
#define ACS_DARROW    ACS_PICK('.', 'v')
#define ACS_UARROW    ACS_PICK('-', '^')
#define ACS_BOARD     ACS_PICK('h', '#')
#define ACS_LANTERN   ACS_PICK('i', '*')
#define ACS_BLOCK     ACS_PICK('0', '#')

/* That goes double for these -- undocumented SysV symbols. Don't use
   them. */

#define ACS_S3        ACS_PICK('p', '-')
#define ACS_S7        ACS_PICK('r', '-')
#define ACS_LEQUAL    ACS_PICK('y', '<')
#define ACS_GEQUAL    ACS_PICK('z', '>')
#define ACS_PI        ACS_PICK('{', 'n')
#define ACS_NEQUAL    ACS_PICK('|', '+')
#define ACS_STERLING  ACS_PICK('}', 'L')

/* Box char aliases */

#define ACS_BSSB      ACS_ULCORNER
#define ACS_SSBB      ACS_LLCORNER
#define ACS_BBSS      ACS_URCORNER
#define ACS_SBBS      ACS_LRCORNER
#define ACS_SBSS      ACS_RTEE
#define ACS_SSSB      ACS_LTEE
#define ACS_SSBS      ACS_BTEE
#define ACS_BSSS      ACS_TTEE
#define ACS_BSBS      ACS_HLINE
#define ACS_SBSB      ACS_VLINE
#define ACS_SSSS      ACS_PLUS


/*** Color macros ***/

#define COLOR_BLACK   0

#ifdef PDC_RGB        /* RGB */
# define COLOR_RED    1
# define COLOR_GREEN  2
# define COLOR_BLUE   4
#else                 /* BGR */
# define COLOR_BLUE   1
# define COLOR_GREEN  2
# define COLOR_RED    4
#endif

#define COLOR_CYAN    (COLOR_BLUE | COLOR_GREEN)
#define COLOR_MAGENTA (COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW  (COLOR_RED | COLOR_GREEN)

#define COLOR_WHITE   7


#endif /* __PDCURSES_CHARS_H__ */
