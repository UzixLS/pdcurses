/* Public Domain Curses */

/* $Id: curspriv.h,v 1.158 2008/07/13 16:08:16 wmcbrine Exp $ */

/* Private definitions and declarations for use within PDCurses.
   These should generally not be referenced by applications. */

#ifndef __CURSES_INTERNALS__
#define __CURSES_INTERNALS__ 1

#define CURSES_LIBRARY
#include <curses.h>
#include <panel.h>


#if defined(__TURBOC__) || defined(__EMX__) || defined(__DJGPP__) || \
    defined(__CYGWIN32__) || defined(__MINGW32__) || \
    defined(__WATCOMC__) || defined(__PACIFIC__)
# ifndef HAVE_VSSCANF
#  define HAVE_VSSCANF       /* have vsscanf() */
# endif
#endif

#if defined(__CYGWIN32__) || defined(__MINGW32__) || \
    defined(__LCC__) || defined(__WATCOMC__)
# ifndef HAVE_VSNPRINTF
#  define HAVE_VSNPRINTF     /* have vsnprintf() */
# endif
#endif

#if defined(_MSC_VER) && defined(_WIN32) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE 1   /* kill nonsense warnings */
#endif



#define _INBUFSIZ   512 /* size of terminal input buffer */
#define NUNGETCH    256 /* max # chars to ungetch() */


/*----------------------------------------------------------------------
 *
 *  PDCurses Structure Definitions
 *
 */

typedef struct           /* structure for ripped off lines */
{
    int line;
    int (*init)(WINDOW *, int);
} RIPPEDOFFLINE;

struct _win       /* definition of a window */
{
    int   _cury;          /* current pseudo-cursor */
    int   _curx;
    int   _maxy;          /* max window coordinates */
    int   _maxx;
    int   _begy;          /* origin on screen */
    int   _begx;
    int   _flags;         /* window properties */
    chtype _attrs;        /* standard attributes and colors */
    chtype _bkgd;         /* background, normally blank */
    bool  _clear;         /* causes clear at next refresh */
    bool  _leaveit;       /* leaves cursor where it is */
    bool  _scroll;        /* allows window scrolling */
    bool  _nodelay;       /* input character wait flag */
    bool  _immed;         /* immediate update flag */
    bool  _sync;          /* synchronise window ancestors */
    bool  _use_keypad;    /* flags keypad key mode active */
    chtype **_y;          /* pointer to line pointer array */
    short *_firstch;      /* first changed character in line */
    short *_lastch;       /* last changed character in line */
    int   _tmarg;         /* top of scrolling region */
    int   _bmarg;         /* bottom of scrolling region */
    int   _delayms;       /* milliseconds of delay for getch() */
    int   _parx, _pary;   /* coords relative to parent (0,0) */
    struct _win *_parent; /* subwin's pointer to parent win */
};

struct _screen
{
    bool  alive;          /* if initscr() called, and not endwin() */
    bool  autocr;         /* if cr -> lf */
    bool  cbreak;         /* if terminal unbuffered */
    bool  echo;           /* if terminal echo */
    bool  raw_inp;        /* raw input mode (v. cooked input) */
    bool  raw_out;        /* raw output mode (7 v. 8 bits) */
    bool  audible;        /* FALSE if the bell is visual */
    bool  mono;           /* TRUE if current screen is mono */
    bool  resized;        /* TRUE if TERM has been resized */
    bool  orig_attr;      /* TRUE if we have the original colors */
    short orig_fore;      /* original screen foreground color */
    short orig_back;      /* original screen foreground color */
    int   cursrow;        /* position of physical cursor */
    int   curscol;        /* position of physical cursor */
    int   visibility;     /* visibility of cursor */
    int   orig_cursor;    /* original cursor size */
    int   lines;          /* new value for LINES */
    int   cols;           /* new value for COLS */
    unsigned long _trap_mbe;       /* trap these mouse button events */
    unsigned long _map_mbe_to_key; /* map mouse buttons to slk */
    int   mouse_wait;              /* time to wait (in ms) for a
                                      button release after a press, in
                                      order to count it as a click */
    int   slklines;                /* lines in use by slk_init() */
    WINDOW *slk_winptr;            /* window for slk */
    int   linesrippedoff;          /* lines ripped off via ripoffline() */
    int   linesrippedoffontop;     /* lines ripped off on
                                      top via ripoffline() */
    int   delaytenths;             /* 1/10ths second to wait block
                                      getch() for */
    bool  _preserve;               /* TRUE if screen background
                                      to be preserved */
    int   _restore;                /* specifies if screen background
                                      to be restored, and how */
    bool  save_key_modifiers;      /* TRUE if each key modifiers saved
                                      with each key press */
    bool  return_key_modifiers;    /* TRUE if modifier keys are
                                      returned as "real" keys */
    bool  key_code;                /* TRUE if last key is a special key;
                                      used internally by get_wch() */
    short line_color;     /* color of line attributes - default -1 */
};

struct _session
{
    SCREEN      *SP;           /* curses variables */
    WINDOW      *stdscr;       /* the default screen window */
    WINDOW      *curscr;       /* the current screen image */
    int          LINES;        /* terminal height */
    int          COLS;         /* terminal width */
    int          TABSIZE;
    char         ttytype[64];  /* terminal name/description */
    RIPPEDOFFLINE linesripped[5];
    char         linesrippedoff;

    bool         color_started;
    MOUSE_STATUS Mouse_status;
    MOUSE_STATUS mouse_status;
    bool         mouse_ungot;
    unsigned long key_modifiers;
    int          getch_c_pindex;    /* putter index */
    int          getch_c_gindex;    /* getter index */
    int          getch_c_ungind;    /* ungetch() push index */
    int          getch_c_ungch[NUNGETCH];   /* array of ungotten chars */
    int          getch_buffer[_INBUFSIZ];   /* character buffer */
    int          pad_save_pminrow;
    int          pad_save_pmincol;
    int          pad_save_sminrow;
    int          pad_save_smincol;
    int          pad_save_smaxrow;
    int          pad_save_smaxcol;
    PANEL       *panel_bottom;
    PANEL       *panel_top;
    PANEL        panel_stdscr_pseudo;
    struct SLK  *slk;
    int          slk_label_length;
    int          slk_labels;
    int          slk_label_fmt;
    int          slk_label_line;
    bool         slk_hidden;
    char         slk_label_temp[33];
    struct pd   *pd; /* platform depended data */
};

extern chtype acs_map[];    /* alternate character set map */



/* Window properties */

#define _SUBWIN    0x01  /* window is a subwindow */
#define _PAD       0x10  /* X/Open Pad. */
#define _SUBPAD    0x20  /* X/Open subpad. */

/* Miscellaneous */

#define _NO_CHANGE -1    /* flags line edge unchanged */

#define _ECHAR     0x08  /* Erase char       (^H) */
#define _DWCHAR    0x17  /* Delete Word char (^W) */
#define _DLCHAR    0x15  /* Delete Line char (^U) */



/*----------------------------------------------------------------------*/

/* Platform implementation functions */

void    PDC_beep(SESSION *);
bool    PDC_can_change_color(SESSION *);
int     PDC_color_content(SESSION *, short, short *, short *, short *);
bool    PDC_check_key(SESSION *);
int     PDC_curs_set(SESSION *, int);
void    PDC_flushinp(SESSION *);
int     PDC_get_cursor_mode(SESSION *);
int     PDC_get_key(SESSION *);
void    PDC_get_termsize(SESSION *, int *, int *);
void    PDC_gotoyx(SESSION *, int, int);
int     PDC_init_color(SESSION *, short, short, short, short);
void    PDC_init_pair(SESSION *, short, short, short);
int     PDC_modifiers_set(SESSION *);
int     PDC_mouse_set(SESSION *);
void    PDC_napms(SESSION *, int);
int     PDC_pair_content(SESSION *, short, short *, short *);
void    PDC_reset_prog_mode(SESSION *);
void    PDC_reset_shell_mode(SESSION *);
int     PDC_resize_screen(SESSION *, int, int);
void    PDC_restore_screen_mode(SESSION *, int);
void    PDC_save_screen_mode(SESSION *, int);
void    PDC_scr_close(SESSION *);
void    PDC_scr_free(SESSION *);
int     PDC_scr_open(SESSION *, void *userargs);
void    PDC_set_keyboard_binary(SESSION *, bool);
void    PDC_transform_line(SESSION *, int, int, int, const chtype *);
const char *PDC_sysname(SESSION *);
void   *PDC_malloc(unsigned);
void   *PDC_calloc(unsigned, unsigned);
void    PDC_free(void *);


/* Internal cross-module functions */

void    PDC_init_atrtab(SESSION *);
WINDOW *PDC_makelines(SESSION *, WINDOW *);
WINDOW *PDC_makenew(SESSION *, int, int, int, int);
int     PDC_mouse_in_slk(SESSION *, int, int);
void    PDC_slk_free(SESSION *);
void    PDC_slk_initialize(SESSION *);
void    PDC_sync(SESSION *, WINDOW *);

# define PDC_LOG(x)
# define RCSID(x)


/* Internal macros for attributes */

#ifdef CHTYPE_LONG
# define PDC_COLOR_PAIRS 256
#else
# define PDC_COLOR_PAIRS  32
#endif

#ifndef max
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define DIVROUND(num, divisor) ((num) + ((divisor) >> 1)) / (divisor)

#define PDC_CLICK_PERIOD 150  /* time to wait for a click, if
                                 not set by mouseinterval() */


#endif /* __CURSES_INTERNALS__*/
