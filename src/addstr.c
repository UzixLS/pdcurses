/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: addstr.c,v 1.44 2008/07/13 16:08:17 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         addstr

  Synopsis:
        int addstr(const char *str);
        int addnstr(const char *str, int n);
        int waddstr(WINDOW *win, const char *str);
        int waddnstr(WINDOW *win, const char *str, int n);
        int mvaddstr(int y, int x, const char *str);
        int mvaddnstr(int y, int x, const char *str, int n);
        int mvwaddstr(WINDOW *win, int y, int x, const char *str);
        int mvwaddnstr(WINDOW *win, int y, int x, const char *str, int n);

        int addwstr(const wchar_t *wstr);
        int addnwstr(const wchar_t *wstr, int n);
        int waddwstr(WINDOW *win, const wchar_t *wstr);
        int waddnwstr(WINDOW *win, const wchar_t *wstr, int n);
        int mvaddwstr(int y, int x, const wchar_t *wstr);
        int mvaddnwstr(int y, int x, const wchar_t *wstr, int n);
        int mvwaddwstr(WINDOW *win, int y, int x, const wchar_t *wstr);
        int mvwaddnwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

  Description:
        These routines write all the characters of the null-terminated
        string str or wide-character string wstr to the given window.
        The functionality is similar to calling waddch() once for each
        character in the string; except that, when PDCurses is built
        with wide-character support enabled, the narrow-character
        functions treat the string as a multibyte string in the current
        locale, and convert it. The routines with n as the last
        argument write at most n characters; if n is negative, then the
        entire string will be added.

  Return Value:
        All functions return OK or ERR.

  Portability                                X/Open    BSD    SYS V
        addstr                                  Y       Y       Y
        waddstr                                 Y       Y       Y
        mvaddstr                                Y       Y       Y
        mvwaddstr                               Y       Y       Y
        addnstr                                 Y       -      4.0
        waddnstr                                Y       -      4.0
        mvaddnstr                               Y       -      4.0
        mvwaddnstr                              Y       -      4.0
        addwstr                                 Y
        waddwstr                                Y
        mvaddwstr                               Y
        mvwaddwstr                              Y
        addnwstr                                Y
        waddnwstr                               Y
        mvaddnwstr                              Y
        mvwaddnwstr                             Y

**man-end****************************************************************/

int waddnstr(SESSION *S, WINDOW *win, const char *str, int n)
{
    int i = 0;

    PDC_LOG(("waddnstr() - called: string=\"%s\" n %d \n", str, n));

    if (!S || !win || !str)
        return ERR;

    while (str[i] && (i < n || n < 0))
    {
        chtype wch = (unsigned char)(str[i++]);
        if (waddch(S, win, wch) == ERR)
            return ERR;
    }

    return OK;
}

int addstr(SESSION *S, const char *str)
{
    PDC_LOG(("addstr() - called: string=\"%s\"\n", str));

    return waddnstr(S, S->stdscr, str, -1);
}

int addnstr(SESSION *S, const char *str, int n)
{
    PDC_LOG(("addnstr() - called: string=\"%s\" n %d \n", str, n));

    return waddnstr(S, S->stdscr, str, n);
}

int waddstr(SESSION *S, WINDOW *win, const char *str)
{
    PDC_LOG(("waddstr() - called: string=\"%s\"\n", str));

    return waddnstr(S, win, str, -1);
}

int mvaddstr(SESSION *S, int y, int x, const char *str)
{
    PDC_LOG(("mvaddstr() - called: y %d x %d string=\"%s\"\n", y, x, str));

    if (move(S, y, x) == ERR)
        return ERR;

    return waddnstr(S, S->stdscr, str, -1);
}

int mvaddnstr(SESSION *S, int y, int x, const char *str, int n)
{
    PDC_LOG(("mvaddnstr() - called: y %d x %d string=\"%s\" n %d \n",
             y, x, str, n));

    if (move(S, y, x) == ERR)
        return ERR;

    return waddnstr(S, S->stdscr, str, n);
}

int mvwaddstr(SESSION *S, WINDOW *win, int y, int x, const char *str)
{
    PDC_LOG(("mvwaddstr() - called: string=\"%s\"\n", str));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return waddnstr(S, win, str, -1);
}

int mvwaddnstr(SESSION *S, WINDOW *win, int y, int x, const char *str, int n)
{
    PDC_LOG(("mvwaddnstr() - called: y %d x %d string=\"%s\" n %d \n",
             y, x, str, n));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return waddnstr(S, win, str, n);
}

