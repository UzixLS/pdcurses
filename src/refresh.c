/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: refresh.c,v 1.56 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         refresh

  Synopsis:
        int refresh(void);
        int wrefresh(WINDOW *win);
        int wnoutrefresh(WINDOW *win);
        int doupdate(void);
        int redrawwin(WINDOW *win);
        int wredrawln(WINDOW *win, int beg_line, int num_lines);

  Description:
        wrefresh() copies the named window to the physical terminal
        screen, taking into account what is already there in order to
        optimize cursor movement. refresh() does the same, using stdscr.
        These routines must be called to get any output on the terminal,
        as other routines only manipulate data structures. Unless
        leaveok() has been enabled, the physical cursor of the terminal
        is left at the location of the window's cursor.

        wnoutrefresh() and doupdate() allow multiple updates with more
        efficiency than wrefresh() alone. wrefresh() works by first
        calling wnoutrefresh(), which copies the named window to the
        virtual screen.  It then calls doupdate(), which compares the
        virtual screen to the physical screen and does the actual
        update. A series of calls to wrefresh() will result in
        alternating calls to wnoutrefresh() and doupdate(), causing
        several bursts of output to the screen.  By first calling
        wnoutrefresh() for each window, it is then possible to call
        doupdate() only once.

        In PDCurses, redrawwin() is equivalent to touchwin(), and
        wredrawln() is the same as touchline(). In some other curses
        implementations, there's a subtle distinction, but it has no
        meaning in PDCurses.

  Return Value:
        All functions return OK on success and ERR on error.

  Portability                                X/Open    BSD    SYS V
        refresh                                 Y       Y       Y
        wrefresh                                Y       Y       Y
        wnoutrefresh                            Y       Y       Y
        doupdate                                Y       Y       Y
        redrawwin                               Y       -      4.0
        wredrawln                               Y       -      4.0

**man-end****************************************************************/

#include <string.h>


int wnoutrefresh(SESSION *S, WINDOW *win)
{
    int begy, begx;     /* window's place on screen   */
    int i, j;

    PDC_LOG(("wnoutrefresh() - called: win=%p\n", win));

    if (!S || !win || (win->_flags & (_PAD|_SUBPAD)))
        return ERR;

    begy = win->_begy;
    begx = win->_begx;

    for (i = 0, j = begy; i < win->_maxy; i++, j++)
    {
        if (win->_firstch[i] != _NO_CHANGE)
        {
            chtype *src = win->_y[i];
            chtype *dest = S->curscr->_y[j] + begx;

            int first = win->_firstch[i]; /* first changed */
            int last = win->_lastch[i];   /* last changed */

            /* ignore areas on the outside that are marked as changed,
               but really aren't */

            while (first <= last && src[first] == dest[first])
                first++;

            while (last >= first && src[last] == dest[last])
                last--;

            /* if any have really changed... */

            if (first <= last)
            {
                memcpy(dest + first, src + first,
                       (last - first + 1) * sizeof(chtype));

                first += begx;
                last += begx;

                if (first < S->curscr->_firstch[j] ||
                    S->curscr->_firstch[j] == _NO_CHANGE)
                    S->curscr->_firstch[j] = first;

                if (last > S->curscr->_lastch[j])
                    S->curscr->_lastch[j] = last;
            }

            win->_firstch[i] = _NO_CHANGE;  /* updated now */
        }

        win->_lastch[i] = _NO_CHANGE;       /* updated now */
    }

    if (win->_clear)
        win->_clear = FALSE;

    if (!win->_leaveit)
    {
        S->curscr->_cury = win->_cury + begy;
        S->curscr->_curx = win->_curx + begx;
    }

    return OK;
}

int doupdate(SESSION *S)
{
    int y;
    bool clearall;

    PDC_LOG(("doupdate() - called\n"));

    if (!S || !S->curscr)
        return ERR;

    if (isendwin(S))         /* coming back after endwin() called */
    {
        reset_prog_mode(S);
        clearall = TRUE;
        S->SP->alive = TRUE;   /* so isendwin() result is correct */
    }
    else
        clearall = S->curscr->_clear;

    for (y = 0; y < S->SP->lines; y++)
    {
        PDC_LOG(("doupdate() - Transforming line %d of %d: %s\n",
                 y, SP->lines, (curscr->_firstch[y] != _NO_CHANGE) ?
                 "Yes" : "No"));

        if (clearall || S->curscr->_firstch[y] != _NO_CHANGE)
        {
            int first, last, len;

            chtype *src = S->curscr->_y[y];

            if (clearall)
            {
                first = 0;
                last = S->COLS - 1;
            }
            else
            {
                first = S->curscr->_firstch[y];
                last = S->curscr->_lastch[y];
            }

            len = last - first + 1;

            if (len)
            {
                PDC_transform_line(S, y, first, len, src+first);
            }

            S->curscr->_firstch[y] = _NO_CHANGE;
            S->curscr->_lastch[y] = _NO_CHANGE;
        }
    }

    S->curscr->_clear = FALSE;

    if (S->SP->visibility)
        PDC_gotoyx(S, S->curscr->_cury, S->curscr->_curx);

    S->SP->cursrow = S->curscr->_cury;
    S->SP->curscol = S->curscr->_curx;

    return OK;
}

int wrefresh(SESSION *S, WINDOW *win)
{
    bool save_clear;

    PDC_LOG(("wrefresh() - called\n"));

    if (!S || !win || (win->_flags & (_PAD|_SUBPAD)) )
        return ERR;

    save_clear = win->_clear;

    if (win == S->curscr)
        S->curscr->_clear = TRUE;
    else
        wnoutrefresh(S, win);

    if (save_clear && win->_maxy == S->SP->lines && win->_maxx == S->SP->cols)
        S->curscr->_clear = TRUE;

    return doupdate(S);
}

int refresh(SESSION *S)
{
    PDC_LOG(("refresh() - called\n"));

    return wrefresh(S, S->stdscr);
}

int wredrawln(SESSION *S, WINDOW *win, int start, int num)
{
    int i;

    PDC_LOG(("wredrawln() - called: win=%p start=%d num=%d\n",
        win, start, num));

    if (!S || !win || start > win->_maxy || start + num > win->_maxy)
        return ERR;

    for (i = start; i < start + num; i++)
    {
        win->_firstch[i] = 0;
        win->_lastch[i] = win->_maxx - 1;
    }

    return OK;
}

int redrawwin(SESSION *S, WINDOW *win)
{
    PDC_LOG(("redrawwin() - called: win=%p\n", win));

    if (!S || !win)
        return ERR;

    return wredrawln(S, win, 0, win->_maxy);
}
