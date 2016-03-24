/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: insstr.c,v 1.46 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         insstr

  Synopsis:
        int insstr(const char *str);
        int insnstr(const char *str, int n);
        int winsstr(WINDOW *win, const char *str);
        int winsnstr(WINDOW *win, const char *str, int n);
        int mvinsstr(int y, int x, const char *str);
        int mvinsnstr(int y, int x, const char *str, int n);
        int mvwinsstr(WINDOW *win, int y, int x, const char *str);
        int mvwinsnstr(WINDOW *win, int y, int x, const char *str, int n);

        int ins_wstr(const wchar_t *wstr);
        int ins_nwstr(const wchar_t *wstr, int n);
        int wins_wstr(WINDOW *win, const wchar_t *wstr);
        int wins_nwstr(WINDOW *win, const wchar_t *wstr, int n);
        int mvins_wstr(int y, int x, const wchar_t *wstr);
        int mvins_nwstr(int y, int x, const wchar_t *wstr, int n);
        int mvwins_wstr(WINDOW *win, int y, int x, const wchar_t *wstr);
        int mvwins_nwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

  Description:
        The insstr() functions insert a character string into a window
        at the current cursor position, by repeatedly calling winsch().
        When PDCurses is built with wide-character support enabled, the
        narrow-character functions treat the string as a multibyte
        string in the current locale, and convert it first. All
        characters to the right of the cursor are moved to the right,
        with the possibility of the rightmost characters on the line
        being lost.  The cursor position does not change (after moving
        to y, x, if specified).  The routines with n as the last
        argument insert at most n characters; if n is negative, then the
        entire string is inserted.

  Return Value:
        All functions return OK on success and ERR on error.

  Portability                                X/Open    BSD    SYS V
        insstr                                  Y       -      4.0
        winsstr                                 Y       -      4.0
        mvinsstr                                Y       -      4.0
        mvwinsstr                               Y       -      4.0
        insnstr                                 Y       -      4.0
        winsnstr                                Y       -      4.0
        mvinsnstr                               Y       -      4.0
        mvwinsnstr                              Y       -      4.0
        ins_wstr                                Y
        wins_wstr                               Y
        mvins_wstr                              Y
        mvwins_wstr                             Y
        ins_nwstr                               Y
        wins_nwstr                              Y
        mvins_nwstr                             Y
        mvwins_nwstr                            Y

**man-end****************************************************************/

#include <string.h>

int winsnstr(SESSION *S, WINDOW *win, const char *str, int n)
{
    int len;

    PDC_LOG(("winsnstr() - called: string=\"%s\" n %d \n", str, n));

    if (!S || !win || !str)
        return ERR;

    len = strlen(str);

    if (n < 0 || n < len)
        n = len;

    while (n)
        if (winsch(S, win, (unsigned char)(str[--n])) == ERR)
            return ERR;

    return OK;
}

int insstr(SESSION *S, const char *str)
{
    PDC_LOG(("insstr() - called: string=\"%s\"\n", str));

    return winsnstr(S, S->stdscr, str, -1);
}

int winsstr(SESSION *S, WINDOW *win, const char *str)
{
    PDC_LOG(("winsstr() - called: string=\"%s\"\n", str));

    return winsnstr(S, win, str, -1);
}

int mvinsstr(SESSION *S, int y, int x, const char *str)
{
    PDC_LOG(("mvinsstr() - called: y %d x %d string=\"%s\"\n", y, x, str));

    if (move(S, y, x) == ERR)
        return ERR;

    return winsnstr(S, S->stdscr, str, -1);
}

int mvwinsstr(SESSION *S, WINDOW *win, int y, int x, const char *str)
{
    PDC_LOG(("mvwinsstr() - called: string=\"%s\"\n", str));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return winsnstr(S, win, str, -1);
}

int insnstr(SESSION *S, const char *str, int n)
{
    PDC_LOG(("insnstr() - called: string=\"%s\" n %d \n", str, n));

    return winsnstr(S, S->stdscr, str, n);
}

int mvinsnstr(SESSION *S, int y, int x, const char *str, int n)
{
    PDC_LOG(("mvinsnstr() - called: y %d x %d string=\"%s\" n %d \n",
             y, x, str, n));

    if (move(S, y, x) == ERR)
        return ERR;

    return winsnstr(S, S->stdscr, str, n);
}

int mvwinsnstr(SESSION *S, WINDOW *win, int y, int x, const char *str, int n)
{
    PDC_LOG(("mvwinsnstr() - called: y %d x %d string=\"%s\" n %d \n",
             y, x, str, n));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return winsnstr(S, win, str, n);
}

