/* Public Domain Curses */

/* $Id: curses.h,v 1.295 2008/07/15 17:13:25 wmcbrine Exp $ */

/*----------------------------------------------------------------------*
 *                              PDCurses                                *
 *----------------------------------------------------------------------*/

#ifndef __PDCURSES__
#define __PDCURSES__ 1

/*man-start**************************************************************

PDCurses definitions list:  (Only define those needed)

    PDC_RGB         True if you want to use RGB color definitions
                    (Red = 1, Green = 2, Blue = 4) instead of BGR.
    NCURSES_MOUSE_VERSION   Use the ncurses mouse API instead
                            of PDCurses' traditional mouse API.

PDCurses portable platform definitions list:

    PDC_BUILD       Defines API build version.

**man-end****************************************************************/

#define PDC_BUILD 3401
#define PDCURSES        1
//#define CHTYPE_LONG     1      /* size of chtype; long */

/*----------------------------------------------------------------------*/

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>             /* Required by X/Open usage below */


#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
# define bool _bool
#endif

#include "src/chars.h"
#include "src/keys.h"

/*----------------------------------------------------------------------
 *
 *  PDCurses Manifest Constants
 *
 */

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE 1
#endif
#ifndef NULL
# define NULL (void *)0
#endif
#ifndef ERR
# define ERR (-1)
#endif
#ifndef OK
# define OK 0
#endif

/*----------------------------------------------------------------------
 *
 *  PDCurses Type Declarations
 *
 */

#ifdef CHTYPE_LONG
# if _LP64
typedef unsigned int chtype;
# else
typedef unsigned long chtype;  /* 16-bit attr + 16-bit char */
# endif
#else
typedef unsigned short chtype; /* 8-bit attr + 8-bit char */
#endif

typedef chtype attr_t;

/*----------------------------------------------------------------------
 *
 *  PDCurses Mouse Interface -- SYSVR4, with extensions
 *
 */

typedef struct
{
    int x;           /* absolute column, 0 based, measured in characters */
    int y;           /* absolute row, 0 based, measured in characters */
    short button[3]; /* state of each button */
    int changes;     /* flags indicating what has changed with the mouse */
} MOUSE_STATUS;

#define BUTTON_RELEASED         0x0000
#define BUTTON_PRESSED          0x0001
#define BUTTON_CLICKED          0x0002
#define BUTTON_DOUBLE_CLICKED   0x0003
#define BUTTON_TRIPLE_CLICKED   0x0004
#define BUTTON_MOVED            0x0005  /* PDCurses */
#define WHEEL_SCROLLED          0x0006  /* PDCurses */
#define BUTTON_ACTION_MASK      0x0007  /* PDCurses */

#define PDC_BUTTON_SHIFT        0x0008  /* PDCurses */
#define PDC_BUTTON_CONTROL      0x0010  /* PDCurses */
#define PDC_BUTTON_ALT          0x0020  /* PDCurses */
#define BUTTON_MODIFIER_MASK    0x0038  /* PDCurses */

#define MOUSE_X_POS(_S)         (Mouse_status(_S)->x)
#define MOUSE_Y_POS(_S)         (Mouse_status(_S)->y)

/*
 * Bits associated with the .changes field:
 *   3         2         1         0
 * 210987654321098765432109876543210
 *                                 1 <- button 1 has changed
 *                                10 <- button 2 has changed
 *                               100 <- button 3 has changed
 *                              1000 <- mouse has moved
 *                             10000 <- mouse position report
 *                            100000 <- mouse wheel up
 *                           1000000 <- mouse wheel down
 */

#define PDC_MOUSE_MOVED         0x0008
#define PDC_MOUSE_POSITION      0x0010
#define PDC_MOUSE_WHEEL_UP      0x0020
#define PDC_MOUSE_WHEEL_DOWN    0x0040

#define A_BUTTON_CHANGED(_S)    (Mouse_status(_S)->changes & 7)
#define MOUSE_MOVED(_S)         (Mouse_status(_S)->changes & PDC_MOUSE_MOVED)
#define MOUSE_POS_REPORT(_S)    (Mouse_status(_S)->changes & PDC_MOUSE_POSITION)
#define BUTTON_CHANGED(_S,x)    (Mouse_status(_S)->changes & (1 << ((x) - 1)))
#define BUTTON_STATUS(_S,x)     (Mouse_status(_S)->button[(x) - 1])
#define MOUSE_WHEEL_UP(_S)      (Mouse_status(_S)->changes & PDC_MOUSE_WHEEL_UP)
#define MOUSE_WHEEL_DOWN(_S)    (Mouse_status(_S)->changes & PDC_MOUSE_WHEEL_DOWN)

/* mouse bit-masks */

#define BUTTON1_RELEASED        0x00000001L
#define BUTTON1_PRESSED         0x00000002L
#define BUTTON1_CLICKED         0x00000004L
#define BUTTON1_DOUBLE_CLICKED  0x00000008L
#define BUTTON1_TRIPLE_CLICKED  0x00000010L
#define BUTTON1_MOVED           0x00000010L /* PDCurses */

#define BUTTON2_RELEASED        0x00000020L
#define BUTTON2_PRESSED         0x00000040L
#define BUTTON2_CLICKED         0x00000080L
#define BUTTON2_DOUBLE_CLICKED  0x00000100L
#define BUTTON2_TRIPLE_CLICKED  0x00000200L
#define BUTTON2_MOVED           0x00000200L /* PDCurses */

#define BUTTON3_RELEASED        0x00000400L
#define BUTTON3_PRESSED         0x00000800L
#define BUTTON3_CLICKED         0x00001000L
#define BUTTON3_DOUBLE_CLICKED  0x00002000L
#define BUTTON3_TRIPLE_CLICKED  0x00004000L
#define BUTTON3_MOVED           0x00004000L /* PDCurses */

/* For the ncurses-compatible functions only, BUTTON4_PRESSED and
   BUTTON5_PRESSED are returned for mouse scroll wheel up and down;
   otherwise PDCurses doesn't support buttons 4 and 5 */

#define BUTTON4_RELEASED        0x00008000L
#define BUTTON4_PRESSED         0x00010000L
#define BUTTON4_CLICKED         0x00020000L
#define BUTTON4_DOUBLE_CLICKED  0x00040000L
#define BUTTON4_TRIPLE_CLICKED  0x00080000L

#define BUTTON5_RELEASED        0x00100000L
#define BUTTON5_PRESSED         0x00200000L
#define BUTTON5_CLICKED         0x00400000L
#define BUTTON5_DOUBLE_CLICKED  0x00800000L
#define BUTTON5_TRIPLE_CLICKED  0x01000000L

#define MOUSE_WHEEL_SCROLL      0x02000000L /* PDCurses */
#define BUTTON_MODIFIER_SHIFT   0x04000000L /* PDCurses */
#define BUTTON_MODIFIER_CONTROL 0x08000000L /* PDCurses */
#define BUTTON_MODIFIER_ALT     0x10000000L /* PDCurses */

#define ALL_MOUSE_EVENTS        0x1fffffffL
#define REPORT_MOUSE_POSITION   0x20000000L

/* ncurses mouse interface */

typedef unsigned long mmask_t;

typedef struct
{
        short id;       /* unused, always 0 */
        int x, y, z;    /* x, y same as MOUSE_STATUS; z unused */
        mmask_t bstate; /* equivalent to changes + button[], but
                           in the same format as used for mousemask() */
} MEVENT;

#ifdef NCURSES_MOUSE_VERSION
# define BUTTON_SHIFT   BUTTON_MODIFIER_SHIFT
# define BUTTON_CONTROL BUTTON_MODIFIER_CONTROL
# define BUTTON_CTRL    BUTTON_MODIFIER_CONTROL
# define BUTTON_ALT     BUTTON_MODIFIER_ALT
#else
# define BUTTON_SHIFT   PDC_BUTTON_SHIFT
# define BUTTON_CONTROL PDC_BUTTON_CONTROL
# define BUTTON_ALT     PDC_BUTTON_ALT
#endif



/*----------------------------------------------------------------------
 *
 *  PDCurses Types
 *
 */

typedef struct _win WINDOW;
typedef struct _screen SCREEN;
typedef struct _session SESSION;

extern int COLORS;
extern int COLOR_PAIRS;


/*----------------------------------------------------------------------
 *
 *  PDCurses Function Declarations
 *
 */

/* Standard */

int     addch(SESSION *S, const chtype);
int     addchnstr(SESSION *S, const chtype *, int);
int     addchstr(SESSION *S, const chtype *);
int     addnstr(SESSION *S, const char *, int);
int     addstr(SESSION *S, const char *);
int     attroff(SESSION *S, chtype);
int     attron(SESSION *S, chtype);
int     attrset(SESSION *S, chtype);
int     attr_get(SESSION *S, attr_t *, short *, void *);
int     attr_off(SESSION *S, attr_t, void *);
int     attr_on(SESSION *S, attr_t, void *);
int     attr_set(SESSION *S, attr_t, short, void *);
int     baudrate(SESSION *S);
int     beep(SESSION *S);
int     bkgd(SESSION *S, chtype);
void    bkgdset(SESSION *S, chtype);
int     border(SESSION *S, chtype, chtype, chtype, chtype, chtype, chtype, chtype, chtype);
int     box(SESSION *S, WINDOW *, chtype, chtype);
bool    can_change_color(SESSION *S);
int     cbreak(SESSION *S);
int     chgat(SESSION *S, int, attr_t, short, const void *);
int     clearok(SESSION *S, WINDOW *, bool);
int     clear(SESSION *S);
int     clrtobot(SESSION *S);
int     clrtoeol(SESSION *S);
int     color_content(SESSION *S,short, short *, short *, short *);
int     color_set(SESSION *S, short, void *);
int     copywin(SESSION *S, const WINDOW *, WINDOW *, int, int, int, int, int, int, int);
int     curs_set(SESSION *S, int);
int     def_prog_mode(SESSION *S);
int     def_shell_mode(SESSION *S);
int     delay_output(SESSION *S, int);
int     delch(SESSION *S);
int     deleteln(SESSION *S);
void    delscreen(SESSION *S);
int     delwin(SESSION *S, WINDOW *);
WINDOW *derwin(SESSION *S, WINDOW *, int, int, int, int);
int     doupdate(SESSION *S);
WINDOW *dupwin(SESSION *S, WINDOW *);
int     echochar(SESSION *S, const chtype);
int     echo(SESSION *S);
int     endwin(SESSION *S);
char    erasechar(SESSION *S);
int     erase(SESSION *S);
void    filter(SESSION *S);
int     flash(SESSION *S);
int     flushinp(SESSION *S);
chtype  getbkgd(SESSION *S, WINDOW *);
int     getnstr(SESSION *S, char *, int);
int     getstr(SESSION *S, char *);
WINDOW *getwin(SESSION *S, FILE *);
int     halfdelay(SESSION *S, int);
bool    has_colors(SESSION *S);
bool    has_ic(SESSION *S);
bool    has_il(SESSION *S);
int     hline(SESSION *S, chtype, int);
void    idcok(SESSION *S, WINDOW *, bool);
int     idlok(SESSION *S, WINDOW *, bool);
void    immedok(SESSION *S, WINDOW *, bool);
int     inchnstr(SESSION *S, chtype *, int);
int     inchstr(SESSION *S, chtype *);
chtype  inch(SESSION *S);
int     init_color(SESSION *S, short, short, short, short);
int     init_pair(SESSION *S, short, short, short);
WINDOW *initscr(SESSION *S, void *userargs);
int     innstr(SESSION *S, char *, int);
int     insch(SESSION *S, chtype);
int     insdelln(SESSION *S, int);
int     insertln(SESSION *S);
int     insnstr(SESSION *S, const char *, int);
int     insstr(SESSION *S, const char *);
int     instr(SESSION *S, char *);
int     intrflush(SESSION *S, WINDOW *, bool);
bool    isendwin(SESSION *S);
bool    is_linetouched(SESSION *S, WINDOW *, int);
bool    is_wintouched(SESSION *S, WINDOW *);
const char   *keyname(int);
int     keypad(SESSION *S, WINDOW *, bool);
char    killchar(SESSION *S);
int     leaveok(SESSION *S, WINDOW *, bool);
char   *longname(SESSION *S);
int     meta(SESSION *S, WINDOW *, bool);
int     move(SESSION *S, int, int);
int     mvaddch(SESSION *S, int, int, const chtype);
int     mvaddchnstr(SESSION *S, int, int, const chtype *, int);
int     mvaddchstr(SESSION *S, int, int, const chtype *);
int     mvaddnstr(SESSION *S, int, int, const char *, int);
int     mvaddstr(SESSION *S, int, int, const char *);
int     mvchgat(SESSION *S, int, int, int, attr_t, short, const void *);
int     mvcur(SESSION *S, int, int, int, int);
int     mvdelch(SESSION *S, int, int);
int     mvderwin(SESSION *S, WINDOW *, int, int);
int     mvgetch(SESSION *S, int, int);
int     mvgetnstr(SESSION *S, int, int, char *, int);
int     mvgetstr(SESSION *S, int, int, char *);
int     mvhline(SESSION *S, int, int, chtype, int);
chtype  mvinch(SESSION *S, int, int);
int     mvinchnstr(SESSION *S, int, int, chtype *, int);
int     mvinchstr(SESSION *S, int, int, chtype *);
int     mvinnstr(SESSION *S, int, int, char *, int);
int     mvinsch(SESSION *S, int, int, chtype);
int     mvinsnstr(SESSION *S, int, int, const char *, int);
int     mvinsstr(SESSION *S, int, int, const char *);
int     mvinstr(SESSION *S, int, int, char *);
int     mvprintw(SESSION *S, int, int, const char *, ...);
int     mvscanw(SESSION *S, int, int, const char *, ...);
int     mvvline(SESSION *S, int, int, chtype, int);
int     mvwaddchnstr(SESSION *S, WINDOW *, int, int, const chtype *, int);
int     mvwaddchstr(SESSION *S, WINDOW *, int, int, const chtype *);
int     mvwaddch(SESSION *S, WINDOW *, int, int, const chtype);
int     mvwaddnstr(SESSION *S, WINDOW *, int, int, const char *, int);
int     mvwaddstr(SESSION *S, WINDOW *, int, int, const char *);
int     mvwchgat(SESSION *S, WINDOW *, int, int, int, attr_t, short, const void *);
int     mvwdelch(SESSION *S, WINDOW *, int, int);
int     mvwgetch(SESSION *S, WINDOW *, int, int);
int     mvwgetnstr(SESSION *S, WINDOW *, int, int, char *, int);
int     mvwgetstr(SESSION *S, WINDOW *, int, int, char *);
int     mvwhline(SESSION *S, WINDOW *, int, int, chtype, int);
int     mvwinchnstr(SESSION *S, WINDOW *, int, int, chtype *, int);
int     mvwinchstr(SESSION *S, WINDOW *, int, int, chtype *);
chtype  mvwinch(SESSION *S, WINDOW *, int, int);
int     mvwinnstr(SESSION *S, WINDOW *, int, int, char *, int);
int     mvwinsch(SESSION *, WINDOW *, int, int, chtype);
int     mvwinsnstr(SESSION *S, WINDOW *, int, int, const char *, int);
int     mvwinsstr(SESSION *S, WINDOW *, int, int, const char *);
int     mvwinstr(SESSION *S, WINDOW *, int, int, char *);
int     mvwin(SESSION *S, WINDOW *, int, int);
int     mvwprintw(SESSION *S, WINDOW *, int, int, const char *, ...);
int     mvwscanw(SESSION *S, WINDOW *, int, int, const char *, ...);
int     mvwvline(SESSION *S, WINDOW *, int, int, chtype, int);
int     napms(SESSION *S, int);
WINDOW *newpad(SESSION *S, int, int);
SCREEN *newterm(SESSION *S, const char *, FILE *, FILE *, void *userargs);
WINDOW *newwin(SESSION *S, int, int, int, int);
int     nl(SESSION *S);
int     nocbreak(SESSION *S);
int     nodelay(SESSION *S, WINDOW *, bool);
int     noecho(SESSION *S);
int     nonl(SESSION *S);
void    noqiflush(SESSION *S);
int     noraw(SESSION *S);
int     notimeout(SESSION *S, WINDOW *, bool);
int     overlay(SESSION *S, const WINDOW *, WINDOW *);
int     overwrite(SESSION *S, const WINDOW *, WINDOW *);
int     pair_content(SESSION *S, short, short *, short *);
int     pechochar(SESSION *S, WINDOW *, chtype);
int     pnoutrefresh(SESSION *S, WINDOW *, int, int, int, int, int, int);
int     prefresh(SESSION *S, WINDOW *, int, int, int, int, int, int);
int     printw(SESSION *S, const char *, ...);
int     putwin(SESSION *S, WINDOW *, FILE *);
void    qiflush(SESSION *S);
int     raw(SESSION *S);
int     redrawwin(SESSION *S, WINDOW *);
int     refresh(SESSION *S);
int     reset_prog_mode(SESSION *S);
int     reset_shell_mode(SESSION *S);
int     resetty(SESSION *S);
int     ripoffline(SESSION *S, int, int (*)(WINDOW *, int));
int     savetty(SESSION *S);
int     scanw(SESSION *S, const char *, ...);
int     scr_dump(SESSION *S, const char *);
int     scr_init(SESSION *S, const char *);
int     scr_restore(SESSION *S, const char *);
int     scr_set(SESSION *, const char *);
int     scrl(SESSION *S, int);
int     scroll(SESSION *S, WINDOW *);
int     scrollok(SESSION *S, WINDOW *, bool);
SCREEN *set_term(SESSION *S, SCREEN *);
int     setscrreg(SESSION *S, int, int);
int     slk_attroff(SESSION *S, const chtype);
int     slk_attr_off(SESSION *S, const attr_t, void *);
int     slk_attron(SESSION *S, const chtype);
int     slk_attr_on(SESSION *S, const attr_t, void *);
int     slk_attrset(SESSION *S, const chtype);
int     slk_attr_set(SESSION *S, const attr_t, short, void *);
int     slk_clear(SESSION *S);
int     slk_color(SESSION *S, short);
int     slk_init(SESSION *S, int);
char   *slk_label(SESSION *S, int);
int     slk_noutrefresh(SESSION *S);
int     slk_refresh(SESSION *S);
int     slk_restore(SESSION *S);
int     slk_set(SESSION *S, int, const char *, int);
int     slk_touch(SESSION *S);
int     standend(SESSION *S);
int     standout(SESSION *S);
int     start_color(SESSION *S);
WINDOW *subpad(SESSION *S, WINDOW *, int, int, int, int);
WINDOW *subwin(SESSION *S, WINDOW *, int, int, int, int);
int     syncok(SESSION *S, WINDOW *, bool);
chtype  termattrs(SESSION *S);
attr_t  term_attrs(SESSION *S);
char   *termname(SESSION *S);
void    timeout(SESSION *S, int);
int     touchline(SESSION *S, WINDOW *, int, int);
int     touchwin(SESSION *S, WINDOW *);
int     typeahead(SESSION *S, int);
int     untouchwin(SESSION *S, WINDOW *);
void    use_env(SESSION *S, bool);
int     vidattr(SESSION *S, chtype);
int     vid_attr(SESSION *S, attr_t, short, void *);
int     vidputs(SESSION *S, chtype, int (*)(int));
int     vid_puts(SESSION *S, attr_t, short, void *, int (*)(int));
int     vline(SESSION *S, chtype, int);
int     vw_printw(SESSION *S, WINDOW *, const char *, va_list);
int     vwprintw(SESSION *S, WINDOW *, const char *, va_list);
int     vw_scanw(SESSION *S, WINDOW *, const char *, va_list);
int     vwscanw(SESSION *S, WINDOW *, const char *, va_list);
int     waddchnstr(SESSION *S, WINDOW *, const chtype *, int);
int     waddchstr(SESSION *S, WINDOW *, const chtype *);
int     waddch(SESSION *S, WINDOW *, const chtype);
int     waddnstr(SESSION *S, WINDOW *, const char *, int);
int     waddstr(SESSION *S, WINDOW *, const char *);
int     wattroff(SESSION *S, WINDOW *, chtype);
int     wattron(SESSION *S, WINDOW *, chtype);
int     wattrset(SESSION *S, WINDOW *, chtype);
int     wattr_get(SESSION *S, WINDOW *, attr_t *, short *, void *);
int     wattr_off(SESSION *S, WINDOW *, attr_t, void *);
int     wattr_on(SESSION *S, WINDOW *, attr_t, void *);
int     wattr_set(SESSION *S, WINDOW *, attr_t, short, void *);
void    wbkgdset(SESSION *S, WINDOW *, chtype);
int     wbkgd(SESSION *S, WINDOW *, chtype);
int     wborder(SESSION *, WINDOW *, chtype, chtype, chtype, chtype,
                chtype, chtype, chtype, chtype);
int     wchgat(SESSION *S, WINDOW *, int, attr_t, short, const void *);
int     wclear(SESSION *S, WINDOW *);
int     wclrtobot(SESSION *S, WINDOW *);
int     wclrtoeol(SESSION *S, WINDOW *);
int     wcolor_set(SESSION *S, WINDOW *, short, void *);
void    wcursyncup(SESSION *S, WINDOW *);
int     wdelch(SESSION *S, WINDOW *);
int     wdeleteln(SESSION *S, WINDOW *);
int     wechochar(SESSION *S, WINDOW *, const chtype);
int     werase(SESSION *S, WINDOW *);
int     wgetch(SESSION *S, WINDOW *);
int     wgetnstr(SESSION *S, WINDOW *, char *, int);
int     wgetstr(SESSION *S, WINDOW *, char *);
int     whline(SESSION *S, WINDOW *, chtype, int);
int     winchnstr(SESSION *S, WINDOW *, chtype *, int);
int     winchstr(SESSION *S, WINDOW *, chtype *);
chtype  winch(SESSION *S, WINDOW *);
int     winnstr(SESSION *S, WINDOW *, char *, int);
int     winsch(SESSION *S, WINDOW *, chtype);
int     winsdelln(SESSION *S, WINDOW *, int);
int     winsertln(SESSION *S, WINDOW *);
int     winsnstr(SESSION *S, WINDOW *, const char *, int);
int     winsstr(SESSION *S, WINDOW *, const char *);
int     winstr(SESSION *S, WINDOW *, char *);
int     wmove(SESSION *S, WINDOW *, int, int);
int     wnoutrefresh(SESSION *S, WINDOW *);
int     wprintw(SESSION *S, WINDOW *, const char *, ...);
int     wredrawln(SESSION *S, WINDOW *, int, int);
int     wrefresh(SESSION *S, WINDOW *);
int     wscanw(SESSION *S, WINDOW *, const char *, ...);
int     wscrl(SESSION *S, WINDOW *, int);
int     wsetscrreg(SESSION *S, WINDOW *, int, int);
int     wstandend(SESSION *S, WINDOW *);
int     wstandout(SESSION *S, WINDOW *);
void    wsyncdown(SESSION *S, WINDOW *);
void    wsyncup(SESSION *S, WINDOW *);
void    wtimeout(SESSION *S, WINDOW *, int);
int     wtouchln(SESSION *S, WINDOW *, int, int, int);
int     wvline(SESSION *S, WINDOW *, chtype, int);


/* Quasi-standard */

chtype  getattrs(WINDOW *);
int     getbegx(WINDOW *);
int     getbegy(WINDOW *);
int     getmaxx(WINDOW *);
int     getmaxy(WINDOW *);
int     getparx(WINDOW *);
int     getpary(WINDOW *);
int     getcurx(WINDOW *);
int     getcury(WINDOW *);
char   *unctrl(chtype);

int     crmode(SESSION *);
int     nocrmode(SESSION *);
int     draino(SESSION *S, int);
int     resetterm(SESSION *S);
int     fixterm(SESSION *S);
int     saveterm(SESSION *S);
int     setsyx(SESSION *S, int, int);

int     mouse_set(SESSION *S, unsigned long);
int     mouse_on(SESSION *S, unsigned long);
int     mouse_off(SESSION *S, unsigned long);
int     request_mouse_pos(SESSION *S);
int     map_button(SESSION *S, unsigned long);
void    wmouse_position(SESSION *S, WINDOW *, int *, int *);
unsigned long getmouse(SESSION *S);
unsigned long getbmap(SESSION *S);

/* ncurses */

int     assume_default_colors(SESSION *S, int, int);
const char *curses_version(void);
bool    has_key(int);
int     use_default_colors(SESSION *S);
int     wresize(SESSION *S, WINDOW *, int, int);

int     mouseinterval(SESSION *S, int);
mmask_t mousemask(SESSION *S, mmask_t, mmask_t *);
bool    mouse_trafo(SESSION *S, int *, int *, bool);
int     nc_getmouse(SESSION *S, MEVENT *);
int     ungetmouse(SESSION *S, MEVENT *);
bool    wenclose(const WINDOW *, int, int);
bool    wmouse_trafo(const WINDOW *, int *, int *, bool);

/* PDCurses */

int     addrawch(SESSION *, chtype);
int     insrawch(SESSION *, chtype);
bool    is_termresized(SESSION *);
int     mvaddrawch(SESSION *, int, int, chtype);
int     mvdeleteln(SESSION *, int, int);
int     mvinsertln(SESSION *, int, int);
int     mvinsrawch(SESSION *, int, int, chtype);
int     mvwaddrawch(SESSION *, WINDOW *, int, int, chtype);
int     mvwdeleteln(SESSION *, WINDOW *, int, int);
int     mvwinsertln(SESSION *, WINDOW *, int, int);
int     mvwinsrawch(SESSION *, WINDOW *, int, int, chtype);
int     raw_output(SESSION *, bool);
int     resize_term(SESSION *, int, int);
WINDOW *resize_window(SESSION *, WINDOW *, int, int);
int     waddrawch(SESSION *, WINDOW *, chtype);
int     winsrawch(SESSION *, WINDOW *, chtype);
char    wordchar(SESSION *);



int     PDC_ungetch(SESSION *, int);
int     PDC_set_blink(SESSION *, bool);
int     PDC_set_line_color(SESSION *, short);
void    PDC_set_title(SESSION *, const char *);

int     PDC_clearclipboard(SESSION *S);
int     PDC_freeclipboard(SESSION *S, char *);
int     PDC_getclipboard(SESSION *S, char **, long *);
int     PDC_setclipboard(SESSION *S, const char *, long);

unsigned long PDC_get_input_fd(SESSION *S);
unsigned long PDC_get_key_modifiers(SESSION *S);
int     PDC_return_key_modifiers(SESSION *, bool);
int     PDC_save_key_modifiers(SESSION *, bool);



/*** Functions defined as macros ***/

/* getch() and ungetch() conflict with some DOS libraries */

#define getch(_S)          wgetch((_S), stdscr(_S))
#define ungetch(_S, ch)    PDC_ungetch((_S),(ch))

#define COLOR_PAIR(n)      (((chtype)(n) << PDC_COLOR_SHIFT) & A_COLOR)
#define PAIR_NUMBER(n)     (((n) & A_COLOR) >> PDC_COLOR_SHIFT)

/* These will _only_ work as macros */

#define getbegyx(w, y, x)  (y = getbegy(w), x = getbegx(w))
#define getmaxyx(w, y, x)  (y = getmaxy(w), x = getmaxx(w))
#define getparyx(w, y, x)  (y = getpary(w), x = getparx(w))
#define getyx(w, y, x)     (y = getcury(w), x = getcurx(w))

#define getsyx(y, x)       { if (curscr->_leaveit) (y)=(x)=-1; \
                             else getyx(curscr,(y),(x)); }

#ifdef NCURSES_MOUSE_VERSION
# define getmouse(x) nc_getmouse(x)
#endif

extern int LINES(SESSION *);
extern int COLS(SESSION *);
extern WINDOW* stdscr(SESSION *);
extern MOUSE_STATUS *Mouse_status(SESSION *);
extern unsigned size_of_SESSION(void);

/* return codes from PDC_getclipboard() and PDC_setclipboard() calls */

#define PDC_CLIP_SUCCESS         0
#define PDC_CLIP_ACCESS_ERROR    1
#define PDC_CLIP_EMPTY           2
#define PDC_CLIP_MEMORY_ERROR    3

/* PDCurses key modifier masks */

#define PDC_KEY_MODIFIER_SHIFT   1
#define PDC_KEY_MODIFIER_CONTROL 2
#define PDC_KEY_MODIFIER_ALT     4
#define PDC_KEY_MODIFIER_NUMLOCK 8

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
# undef bool
}
#endif

#endif  /* __PDCURSES__ */
