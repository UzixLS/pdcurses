/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: getstr.c,v 1.51 2008/07/14 04:24:51 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         getstr

  Synopsis:
        int getstr(char *str);
        int wgetstr(WINDOW *win, char *str);
        int mvgetstr(int y, int x, char *str);
        int mvwgetstr(WINDOW *win, int y, int x, char *str);
        int getnstr(char *str, int n);
        int wgetnstr(WINDOW *win, char *str, int n);
        int mvgetnstr(int y, int x, char *str, int n);
        int mvwgetnstr(WINDOW *win, int y, int x, char *str, int n);

        int get_wstr(wint_t *wstr);
        int wget_wstr(WINDOW *win, wint_t *wstr);
        int mvget_wstr(int y, int x, wint_t *wstr);
        int mvwget_wstr(WINDOW *win, int, int, wint_t *wstr);
        int getn_wstr(wint_t *wstr, int n);
        int wgetn_wstr(WINDOW *win, wint_t *wstr, int n);
        int mvgetn_wstr(int y, int x, wint_t *wstr, int n);
        int mvwgetn_wstr(WINDOW *win, int y, int x, wint_t *wstr, int n);

  Description:
        These routines call wgetch() repeatedly to build a string,
        interpreting erase and kill characters along the way, until a
        newline or carriage return is received. When PDCurses is built
        with wide-character support enabled, the narrow-character
        functions convert the wgetch()'d values into a multibyte string
        in the current locale before returning it. The resulting string
        is placed in the area pointed to by *str. The routines with n as
        the last argument read at most n characters.

        Note that there's no way to know how long the buffer passed to
        wgetstr() is, so use wgetnstr() to avoid buffer overflows.

  Return Value:
        This functions return ERR on failure or any other value on
        success.

  Portability                                X/Open    BSD    SYS V
        getstr                                  Y       Y       Y
        wgetstr                                 Y       Y       Y
        mvgetstr                                Y       Y       Y
        mvwgetstr                               Y       Y       Y
        getnstr                                 Y       -      4.0
        wgetnstr                                Y       -      4.0
        mvgetnstr                               Y       -       -
        mvwgetnstr                              Y       -       -
        get_wstr                                Y
        wget_wstr                               Y
        mvget_wstr                              Y
        mvwget_wstr                             Y
        getn_wstr                               Y
        wgetn_wstr                              Y
        mvgetn_wstr                             Y
        mvwgetn_wstr                            Y

**man-end****************************************************************/

#define MAXLINE 255

int wgetnstr(SESSION *S, WINDOW *win, char *str, int n)
{
    int ch, i, num, x, chars;
    char *p;
    bool stop, oldecho, oldcbreak, oldnodelay;

    PDC_LOG(("wgetnstr() - called\n"));

    if (!S || !win || !str)
        return ERR;

    chars = 0;
    p = str;
    stop = FALSE;

    x = win->_curx;

    oldcbreak = S->SP->cbreak; /* remember states */
    oldecho = S->SP->echo;
    oldnodelay = win->_nodelay;

    S->SP->echo = FALSE;       /* we do echo ourselves */
    cbreak(S);               /* ensure each key is returned immediately */
    win->_nodelay = FALSE;  /* don't return -1 */

    wrefresh(S, win);

    while (!stop)
    {
        ch = wgetch(S, win);

        switch (ch)
        {

        case '\t':
            ch = ' ';
            num = S->TABSIZE - (win->_curx - x) % S->TABSIZE;
            for (i = 0; i < num; i++)
            {
                if (chars < n)
                {
                    if (oldecho)
                        waddch(S, win, ch);
                    *p++ = ch;
                    ++chars;
                }
                else
                    beep(S);
            }
            break;

        case _ECHAR:        /* CTRL-H -- Delete character */
            if (p > str)
            {
                if (oldecho)
                    waddstr(S, win, "\b \b");
                ch = (unsigned char)(*--p);
                if ((ch < ' ') && (oldecho))
                    waddstr(S, win, "\b \b");
                chars--;
            }
            break;

        case _DLCHAR:       /* CTRL-U -- Delete line */
            while (p > str)
            {
                if (oldecho)
                    waddstr(S, win, "\b \b");
                ch = (unsigned char)(*--p);
                if ((ch < ' ') && (oldecho))
                    waddstr(S, win, "\b \b");
            }
            chars = 0;
            break;

        case _DWCHAR:       /* CTRL-W -- Delete word */

            while ((p > str) && (*(p - 1) == ' '))
            {
                if (oldecho)
                    waddstr(S, win, "\b \b");

                --p;        /* remove space */
                chars--;
            }
            while ((p > str) && (*(p - 1) != ' '))
            {
                if (oldecho)
                    waddstr(S, win, "\b \b");

                ch = (unsigned char)(*--p);
                if ((ch < ' ') && (oldecho))
                    waddstr(S, win, "\b \b");
                chars--;
            }
            break;

        case '\n':
        case '\r':
            stop = TRUE;
            if (oldecho)
                waddch(S, win, '\n');
            break;

        default:
            if (chars < n)
            {
                if (!S->SP->key_code && ch < 0x100)
                {
                    *p++ = ch;
                    if (oldecho)
                        waddch(S, win, ch);
                    chars++;
                }
            }
            else
                beep(S);

            break;

        }

        wrefresh(S, win);
    }

    *p = '\0';

    S->SP->echo = oldecho;     /* restore old settings */
    S->SP->cbreak = oldcbreak;
    win->_nodelay = oldnodelay;

    return OK;
}

int getstr(SESSION *S, char *str)
{
    PDC_LOG(("getstr() - called\n"));

    return wgetnstr(S, S->stdscr, str, MAXLINE);
}

int wgetstr(SESSION *S, WINDOW *win, char *str)
{
    PDC_LOG(("wgetstr() - called\n"));

    return wgetnstr(S, win, str, MAXLINE);
}

int mvgetstr(SESSION *S, int y, int x, char *str)
{
    PDC_LOG(("mvgetstr() - called\n"));

    if (move(S, y, x) == ERR)
        return ERR;

    return wgetnstr(S, S->stdscr, str, MAXLINE);
}

int mvwgetstr(SESSION *S, WINDOW *win, int y, int x, char *str)
{
    PDC_LOG(("mvwgetstr() - called\n"));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return wgetnstr(S, win, str, MAXLINE);
}

int getnstr(SESSION *S, char *str, int n)
{
    PDC_LOG(("getnstr() - called\n"));

    return wgetnstr(S, S->stdscr, str, n);
}

int mvgetnstr(SESSION *S, int y, int x, char *str, int n)
{
    PDC_LOG(("mvgetnstr() - called\n"));

    if (move(S, y, x) == ERR)
        return ERR;

    return wgetnstr(S, S->stdscr, str, n);
}

int mvwgetnstr(SESSION *S, WINDOW *win, int y, int x, char *str, int n)
{
    PDC_LOG(("mvwgetnstr() - called\n"));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return wgetnstr(S, win, str, n);
}

