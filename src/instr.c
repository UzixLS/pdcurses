/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: instr.c,v 1.44 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         instr

  Synopsis:
        int instr(char *str);
        int innstr(char *str, int n);
        int winstr(WINDOW *win, char *str);
        int winnstr(WINDOW *win, char *str, int n);
        int mvinstr(int y, int x, char *str);
        int mvinnstr(int y, int x, char *str, int n);
        int mvwinstr(WINDOW *win, int y, int x, char *str);
        int mvwinnstr(WINDOW *win, int y, int x, char *str, int n);

        int inwstr(wchar_t *wstr);
        int innwstr(wchar_t *wstr, int n);
        int winwstr(WINDOW *win, wchar_t *wstr);
        int winnwstr(WINDOW *win, wchar_t *wstr, int n);
        int mvinwstr(int y, int x, wchar_t *wstr);
        int mvinnwstr(int y, int x, wchar_t *wstr, int n);
        int mvwinwstr(WINDOW *win, int y, int x, wchar_t *wstr);
        int mvwinnwstr(WINDOW *win, int y, int x, wchar_t *wstr, int n);

  Description:
        These functions take characters (or wide characters) from the
        current or specified position in the window, and return them as
        a string in str (or wstr). Attributes are ignored. The functions
        with n as the last argument return a string at most n characters
        long.

  Return Value:
        Upon successful completion, innstr(), mvinnstr(), mvwinnstr()
        and winnstr() return the number of characters actually read into
        the string; instr(), mvinstr(), mvwinstr() and winstr() return
        OK. Otherwise, all these functions return ERR.

  Portability                                X/Open    BSD    SYS V
        instr                                   Y       -      4.0
        winstr                                  Y       -      4.0
        mvinstr                                 Y       -      4.0
        mvwinstr                                Y       -      4.0
        innstr                                  Y       -      4.0
        winnstr                                 Y       -      4.0
        mvinnstr                                Y       -      4.0
        mvwinnstr                               Y       -      4.0
        inwstr                                  Y
        winwstr                                 Y
        mvinwstr                                Y
        mvwinwstr                               Y
        innwstr                                 Y
        winnwstr                                Y
        mvinnwstr                               Y
        mvwinnwstr                              Y

**man-end****************************************************************/

int winnstr(SESSION *S, WINDOW *win, char *str, int n)
{
    chtype *src;
    int i;

    PDC_LOG(("winnstr() - called: n %d \n", n));

    if (!win || !str)
        return ERR;

    if (n < 0 || (win->_curx + n) > win->_maxx)
        n = win->_maxx - win->_curx;

    src = win->_y[win->_cury] + win->_curx;

    for (i = 0; i < n; i++)
        str[i] = src[i] & A_CHARTEXT;

    str[i] = '\0';

    return i;
}

int instr(SESSION *S, char *str)
{
    PDC_LOG(("instr() - called: string=\"%s\"\n", str));
    if (!S)
        return ERR;
    return (ERR == winnstr(S, S->stdscr, str, S->stdscr->_maxx)) ? ERR : OK;
}

int winstr(SESSION *S, WINDOW *win, char *str)
{
    PDC_LOG(("winstr() - called: \n"));

    return (ERR == winnstr(S, win, str, win->_maxx)) ? ERR : OK;
}

int mvinstr(SESSION *S, int y, int x, char *str)
{
    PDC_LOG(("mvinstr() - called: y %d x %d \n", y, x));

    if (!S)
        return ERR;

    if (move(S, y, x) == ERR)
        return ERR;

    return (ERR == winnstr(S, S->stdscr, str, S->stdscr->_maxx)) ? ERR : OK;
}

int mvwinstr(SESSION *S, WINDOW *win, int y, int x, char *str)
{
    PDC_LOG(("mvwinstr() - called: y %d x %d \n", y, x));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return (ERR == winnstr(S, win, str, win->_maxx)) ? ERR : OK;
}

int innstr(SESSION *S, char *str, int n)
{
    PDC_LOG(("innstr() - called: n %d \n", n));
    if (!S)
        return ERR;
    return winnstr(S, S->stdscr, str, n);
}

int mvinnstr(SESSION *S, int y, int x, char *str, int n)
{
    PDC_LOG(("mvinnstr() - called: y %d x %d n %d \n", y, x, n));

    if (!S)
        return ERR;

    if (move(S, y, x) == ERR)
        return ERR;

    return winnstr(S, S->stdscr, str, n);
}

int mvwinnstr(SESSION *S, WINDOW *win, int y, int x, char *str, int n)
{
    PDC_LOG(("mvwinnstr() - called: y %d x %d n %d \n", y, x, n));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return winnstr(S, win, str, n);
}

