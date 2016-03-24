/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: window.c,v 1.62 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         window

  Synopsis:
        WINDOW *newwin(int nlines, int ncols, int begy, int begx);
        WINDOW *derwin(WINDOW* orig, int nlines, int ncols,
                int begy, int begx);
        WINDOW *subwin(WINDOW* orig, int nlines, int ncols,
                int begy, int begx);
        WINDOW *dupwin(WINDOW *win);
        int delwin(WINDOW *win);
        int mvwin(WINDOW *win, int y, int x);
        int mvderwin(WINDOW *win, int pary, int parx);
        int syncok(WINDOW *win, bool bf);
        void wsyncup(WINDOW *win);
        void wcursyncup(WINDOW *win);
        void wsyncdown(WINDOW *win);

        WINDOW *resize_window(WINDOW *win, int nlines, int ncols);
        int wresize(WINDOW *win, int nlines, int ncols);
        WINDOW *PDC_makelines(WINDOW *win);
        WINDOW *PDC_makenew(int nlines, int ncols, int begy, int begx);
        void PDC_sync(WINDOW *win);

  Description:
        newwin() creates a new window with the given number of lines,
        nlines and columns, ncols. The upper left corner of the window
        is at line begy, column begx. If nlines is zero, it defaults to
        LINES - begy; ncols to COLS - begx. Create a new full-screen
        window by calling newwin(0, 0, 0, 0).

        delwin() deletes the named window, freeing all associated
        memory. In the case of overlapping windows, subwindows should be
        deleted before the main window.

        mvwin() moves the window so that the upper left-hand corner is
        at position (y,x). If the move would cause the window to be off
        the screen, it is an error and the window is not moved. Moving
        subwindows is allowed.

        subwin() creates a new subwindow within a window.  The
        dimensions of the subwindow are nlines lines and ncols columns.
        The subwindow is at position (begy, begx) on the screen.  This
        position is relative to the screen, and not to the window orig.
        Changes made to either window will affect both.  When using this
        routine, you will often need to call touchwin() before calling
        wrefresh().

        derwin() is the same as subwin(), except that begy and begx are
        relative to the origin of the window orig rather than the
        screen.  There is no difference between subwindows and derived
        windows.

        mvderwin() moves a derived window (or subwindow) inside its
        parent window.  The screen-relative parameters of the window are
        not changed.  This routine is used to display different parts of
        the parent window at the same physical position on the screen.

        dupwin() creates an exact duplicate of the window win.

        wsyncup() causes a touchwin() of all of the window's parents.

        If wsyncok() is called with a second argument of TRUE, this
        causes a wsyncup() to be called every time the window is
        changed.

        wcursyncup() causes the current cursor position of all of a
        window's ancestors to reflect the current cursor position of the
        current window.

        wsyncdown() causes a touchwin() of the current window if any of
        its parent's windows have been touched.

        resize_window() allows the user to resize an existing window. It
        returns the pointer to the new window, or NULL on failure.

        wresize() is an ncurses-compatible wrapper for resize_window().
        Note that, unlike ncurses, it will NOT process any subwindows of
        the window. (However, you still can call it _on_ subwindows.) It
        returns OK or ERR.

        PDC_makenew() allocates all data for a new WINDOW * except the
        actual lines themselves. If it's unable to allocate memory for
        the window structure, it will free all allocated memory and
        return a NULL pointer.

        PDC_makelines() allocates the memory for the lines.

        PDC_sync() handles wrefresh() and wsyncup() calls when a window
        is changed.

  Return Value:
        newwin(), subwin(), derwin() and dupwin() return a pointer
        to the new window, or NULL on failure. delwin(), mvwin(),
        mvderwin() and syncok() return OK or ERR. wsyncup(),
        wcursyncup() and wsyncdown() return nothing.

  Errors:
        It is an error to call resize_window() before calling initscr().
        Also, an error will be generated if we fail to create a newly
        sized replacement window for curscr, or stdscr. This could
        happen when increasing the window size. NOTE: If this happens,
        the previously successfully allocated windows are left alone;
        i.e., the resize is NOT cancelled for those windows.

  Portability                                X/Open    BSD    SYS V
        newwin                                  Y       Y       Y
        delwin                                  Y       Y       Y
        mvwin                                   Y       Y       Y
        subwin                                  Y       Y       Y
        derwin                                  Y       -       Y
        mvderwin                                Y       -       Y
        dupwin                                  Y       -      4.0
        wsyncup                                 Y       -      4.0
        syncok                                  Y       -      4.0
        wcursyncup                              Y       -      4.0
        wsyncdown                               Y       -      4.0
        resize_window                           -       -       -
        wresize                                 -       -       -
        PDC_makelines                           -       -       -
        PDC_makenew                             -       -       -
        PDC_sync                                -       -       -

**man-end****************************************************************/


WINDOW *PDC_makenew(SESSION *S, int nlines, int ncols, int begy, int begx)
{
    WINDOW *win;

    PDC_LOG(("PDC_makenew() - called: lines %d cols %d begy %d begx %d\n",
             nlines, ncols, begy, begx));

    /* allocate the window structure itself */

    if ((win = PDC_calloc(1, sizeof(WINDOW))) == (WINDOW *)NULL)
        return win;

    /* allocate the line pointer array */

    if ((win->_y = PDC_malloc(nlines * sizeof(chtype *))) == NULL)
    {
        PDC_free(win);
        return (WINDOW *)NULL;
    }

    /* allocate the minchng and maxchng arrays */

    if ((win->_firstch = PDC_malloc(nlines * sizeof(short))) == NULL)
    {
        PDC_free(win->_y);
        PDC_free(win);
        return (WINDOW *)NULL;
    }

    if ((win->_lastch = PDC_malloc(nlines * sizeof(short))) == NULL)
    {
        PDC_free(win->_firstch);
        PDC_free(win->_y);
        PDC_free(win);
        return (WINDOW *)NULL;
    }

    /* initialize window variables */

    win->_maxy = nlines;  /* real max screen size */
    win->_maxx = ncols;   /* real max screen size */
    win->_begy = begy;
    win->_begx = begx;
    win->_bkgd = ' ';     /* wrs 4/10/93 -- initialize background to blank */
    win->_clear = (bool) ((nlines == S->LINES) && (ncols == S->COLS));
    win->_bmarg = nlines - 1;
    win->_parx = win->_pary = -1;

    /* init to say window all changed */

    touchwin(S, win);

    return win;
}

WINDOW *PDC_makelines(SESSION *S, WINDOW *win)
{
    int i, j, nlines, ncols;


    if (!win)
        return (WINDOW *)NULL;

    nlines = win->_maxy;
    ncols = win->_maxx;

    PDC_LOG(("PDC_makelines() - called: lines %d cols %d\n", nlines, ncols));

    for (i = 0; i < nlines; i++)
    {
        if ((win->_y[i] = PDC_malloc(ncols * sizeof(chtype))) == NULL)
        {
            /* if error, free all the data */

            for (j = 0; j < i; j++)
                PDC_free(win->_y[j]);

            PDC_free(win->_firstch);
            PDC_free(win->_lastch);
            PDC_free(win->_y);
            PDC_free(win);

            return (WINDOW *)NULL;
        }
    }

    return win;
}

void PDC_sync(SESSION *S, WINDOW *win)
{
    PDC_LOG(("PDC_sync() - called:\n"));

    if (win->_immed)
        wrefresh(S, win);
    if (win->_sync)
        wsyncup(S, win);
}

WINDOW *newwin(SESSION *S, int nlines, int ncols, int begy, int begx)
{
    WINDOW *win;

    PDC_LOG(("newwin() - called:lines=%d cols=%d begy=%d begx=%d\n",
             nlines, ncols, begy, begx));

    if (!S)
        return (WINDOW *)NULL;

    if (!nlines)
        nlines = S->LINES - begy;
    if (!ncols)
        ncols  = S->COLS  - begx;

    if ( (begy + nlines > S->SP->lines || begx + ncols > S->SP->cols)
        || !(win = PDC_makenew(S, nlines, ncols, begy, begx))
        || !(win = PDC_makelines(S, win)) )
        return (WINDOW *)NULL;

    werase(S, win);

    return win;
}

int delwin(SESSION *S, WINDOW *win)
{
    int i;

    PDC_LOG(("delwin() - called\n"));

    if (!win)
        return ERR;

    /* subwindows use parents' lines */

    if (!(win->_flags & (_SUBWIN|_SUBPAD)))
        for (i = 0; i < win->_maxy && win->_y[i]; i++)
            if (win->_y[i])
                PDC_free(win->_y[i]);

    PDC_free(win->_firstch);
    PDC_free(win->_lastch);
    PDC_free(win->_y);
    PDC_free(win);

    return OK;
}

int mvwin(SESSION *S, WINDOW *win, int y, int x)
{
    PDC_LOG(("mvwin() - called\n"));

    if (!win || (y + win->_maxy > S->LINES || y < 0)
             || (x + win->_maxx > S->COLS || x < 0))
        return ERR;

    win->_begy = y;
    win->_begx = x;
    touchwin(S, win);

    return OK;
}

WINDOW *subwin(SESSION *S, WINDOW *orig, int nlines, int ncols, int begy, int begx)
{
    WINDOW *win;
    int i;
    int j = begy - orig->_begy;
    int k = begx - orig->_begx;

    PDC_LOG(("subwin() - called: lines %d cols %d begy %d begx %d\n",
             nlines, ncols, begy, begx));

    /* make sure window fits inside the original one */

    if (!orig || (begy < orig->_begy) || (begx < orig->_begx) ||
        (begy + nlines) > (orig->_begy + orig->_maxy) ||
        (begx + ncols) > (orig->_begx + orig->_maxx))
        return (WINDOW *)NULL;

    if (!nlines)
        nlines = orig->_maxy - 1 - j;
    if (!ncols)
        ncols  = orig->_maxx - 1 - k;

    if ( !(win = PDC_makenew(S, nlines, ncols, begy, begx)) )
        return (WINDOW *)NULL;

    /* initialize window variables */

    win->_attrs = orig->_attrs;
    win->_bkgd = orig->_bkgd;
    win->_leaveit = orig->_leaveit;
    win->_scroll = orig->_scroll;
    win->_nodelay = orig->_nodelay;
    win->_use_keypad = orig->_use_keypad;
    win->_immed = orig->_immed;
    win->_sync = orig->_sync;
    win->_pary = j;
    win->_parx = k;
    win->_parent = orig;

    for (i = 0; i < nlines; i++, j++)
        win->_y[i] = orig->_y[j] + k;

    win->_flags |= _SUBWIN;

    return win;
}

WINDOW *derwin(SESSION *S, WINDOW *orig, int nlines, int ncols, int begy, int begx)
{
    return subwin(S, orig, nlines, ncols, begy + orig->_begy, begx + orig->_begx);
}

int mvderwin(SESSION *S, WINDOW *win, int pary, int parx)
{
    int i, j;
    WINDOW *mypar;

    if (!win || !(win->_parent))
        return ERR;

    mypar = win->_parent;

    if (pary < 0 || parx < 0 || (pary + win->_maxy) > mypar->_maxy ||
                                (parx + win->_maxx) > mypar->_maxx)
        return ERR;

    j = pary;

    for (i = 0; i < win->_maxy; i++)
        win->_y[i] = (mypar->_y[j++]) + parx;

    win->_pary = pary;
    win->_parx = parx;

    return OK;
}

WINDOW *dupwin(SESSION *S, WINDOW *win)
{
    WINDOW *new;
    chtype *ptr, *ptr1;
    int nlines, ncols, begy, begx, i;

    if (!win)
        return (WINDOW *)NULL;

    nlines = win->_maxy;
    ncols = win->_maxx;
    begy = win->_begy;
    begx = win->_begx;

    if ( !(new = PDC_makenew(S, nlines, ncols, begy, begx))
        || !(new = PDC_makelines(S, new)) )
        return (WINDOW *)NULL;

    /* copy the contents of win into new */

    for (i = 0; i < nlines; i++)
    {
        for (ptr = new->_y[i], ptr1 = win->_y[i];
             ptr < new->_y[i] + ncols; ptr++, ptr1++)
            *ptr = *ptr1;

        new->_firstch[i] = 0;
        new->_lastch[i] = ncols - 1;
    }

    new->_curx = win->_curx;
    new->_cury = win->_cury;
    new->_maxy = win->_maxy;
    new->_maxx = win->_maxx;
    new->_begy = win->_begy;
    new->_begx = win->_begx;
    new->_flags = win->_flags;
    new->_attrs = win->_attrs;
    new->_clear = win->_clear;
    new->_leaveit = win->_leaveit;
    new->_scroll = win->_scroll;
    new->_nodelay = win->_nodelay;
    new->_use_keypad = win->_use_keypad;
    new->_tmarg = win->_tmarg;
    new->_bmarg = win->_bmarg;
    new->_parx = win->_parx;
    new->_pary = win->_pary;
    new->_parent = win->_parent;
    new->_bkgd = win->_bkgd;
    new->_flags = win->_flags;

    return new;
}

WINDOW *resize_window(SESSION *S, WINDOW *win, int nlines, int ncols)
{
    WINDOW *new;
    int i, save_cury, save_curx, new_begy, new_begx;

    PDC_LOG(("resize_window() - called: nlines %d ncols %d\n",
             nlines, ncols));

    if (!S || !win)
        return (WINDOW *)NULL;

    if (win->_flags & _SUBPAD)
    {
        if ( !(new = subpad(S, win->_parent, nlines, ncols,
                            win->_begy, win->_begx)) )
            return (WINDOW *)NULL;
    }
    else if (win->_flags & _SUBWIN)
    {
        if ( !(new = subwin(S, win->_parent, nlines, ncols,
                            win->_begy, win->_begx)) )
            return (WINDOW *)NULL;
    }
    else
    {
        if (win == S->SP->slk_winptr)
        {
            new_begy = S->SP->lines - S->SP->slklines;
            new_begx = 0;
        }
        else
        {
            new_begy = win->_begy;
            new_begx = win->_begx;
        }

        if ( !(new = PDC_makenew(S, nlines, ncols, new_begy, new_begx)) )
            return (WINDOW *)NULL;
    }

    save_curx = min(win->_curx, new->_maxx);
    save_cury = min(win->_cury, new->_maxy);

    if (!(win->_flags & (_SUBPAD|_SUBWIN)))
    {
        if ( !(new = PDC_makelines(S, new)) )
            return (WINDOW *)NULL;

        werase(S, new);

        copywin(S, win, new, 0, 0, 0, 0, min(win->_maxy, new->_maxy) - 1,
                min(win->_maxx, new->_maxx) - 1, FALSE);

        for (i = 0; i < win->_maxy && win->_y[i]; i++)
            if (win->_y[i])
                PDC_free(win->_y[i]);
    }

    new->_flags = win->_flags;
    new->_attrs = win->_attrs;
    new->_clear = win->_clear;
    new->_leaveit = win->_leaveit;
    new->_scroll = win->_scroll;
    new->_nodelay = win->_nodelay;
    new->_use_keypad = win->_use_keypad;
    new->_tmarg = (win->_tmarg > new->_maxy - 1) ? 0 : win->_tmarg;
    new->_bmarg = (win->_bmarg == win->_maxy - 1) ?
                  new->_maxy - 1 : min(win->_bmarg, (new->_maxy - 1));
    new->_parent = win->_parent;
    new->_immed = win->_immed;
    new->_sync = win->_sync;
    new->_bkgd = win->_bkgd;

    new->_curx = save_curx;
    new->_cury = save_cury;

    PDC_free(win->_firstch);
    PDC_free(win->_lastch);
    PDC_free(win->_y);

    *win = *new;
    PDC_free(new);

    return win;
}

int wresize(SESSION *S, WINDOW *win, int nlines, int ncols)
{
    return (resize_window(S, win, nlines, ncols) ? OK : ERR);
}

void wsyncup(SESSION *S, WINDOW *win)
{
    WINDOW *tmp;

    PDC_LOG(("wsyncup() - called\n"));

    for (tmp = win; tmp; tmp = tmp->_parent)
        touchwin(S, tmp);
}

int syncok(SESSION *S, WINDOW *win, bool bf)
{
    PDC_LOG(("syncok() - called\n"));

    if (!win)
        return ERR;

    win->_sync = bf;

    return OK;
}

void wcursyncup(SESSION *S, WINDOW *win)
{
    WINDOW *tmp;

    PDC_LOG(("wcursyncup() - called\n"));

    for (tmp = win; tmp && tmp->_parent; tmp = tmp->_parent)
        wmove(S, tmp->_parent, tmp->_pary + tmp->_cury, tmp->_parx + tmp->_curx);
}

void wsyncdown(SESSION *S, WINDOW *win)
{
    WINDOW *tmp;

    PDC_LOG(("wsyncdown() - called\n"));

    for (tmp = win; tmp; tmp = tmp->_parent)
    {
        if (is_wintouched(S, tmp))
        {
            touchwin(S, win);
            break;
        }
    }
}
