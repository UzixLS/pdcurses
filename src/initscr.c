/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: initscr.c,v 1.114 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         initscr

  Synopsis:
        WINDOW *initscr(void);
        WINDOW *Xinitscr(int argc, char *argv[]);
        int endwin(void);
        bool isendwin(void);
        SCREEN *newterm(const char *type, FILE *outfd, FILE *infd);
        SCREEN *set_term(SCREEN *new);
        void delscreen(SCREEN *sp);

        int resize_term(int nlines, int ncols);
        bool is_termresized(void);
        const char *curses_version(void);

  Description:
        initscr() should be the first curses routine called.  It will
        initialize all curses data structures, and arrange that the
        first call to refresh() will clear the screen.  In case of
        error, initscr() will write a message to standard error and end
        the program.

        endwin() should be called before exiting or escaping from curses
        mode temporarily.  It will restore tty modes, move the cursor to
        the lower left corner of the screen and reset the terminal into
        the proper non-visual mode.  To resume curses after a temporary
        escape, call refresh() or doupdate().

        isendwin() returns TRUE if endwin() has been called without a
        subsequent refresh, unless SP is NULL.

        In some implementations of curses, newterm() allows the use of
        multiple terminals. Here, it's just an alternative interface for
        initscr(). It always returns SP, or NULL.

        delscreen() frees the memory allocated by newterm() or
        initscr(), since it's not freed by endwin(). This function is
        usually not needed. In PDCurses, the parameter must be the
        value of SP, and delscreen() sets SP to NULL.

        set_term() does nothing meaningful in PDCurses, but is included
        for compatibility with other curses implementations.

        resize_term() is effectively two functions: When called with
        nonzero values for nlines and ncols, it attempts to resize the
        screen to the given size. When called with (0, 0), it merely
        adjusts the internal structures to match the current size after
        the screen is resized by the user. On the currently supported
        platforms, this functionality is mutually exclusive: X11 allows
        user resizing, while DOS, OS/2 and Win32 allow programmatic
        resizing. If you want to support user resizing, you should check
        for getch() returning KEY_RESIZE, and/or call is_termresized()
        at appropriate times; if either condition occurs, call
        resize_term(0, 0). Then, with either user or programmatic
        resizing, you'll have to resize any windows you've created, as
        appropriate; resize_term() only handles stdscr and curscr.

        is_termresized() returns TRUE if the curses screen has been
        resized by the user, and a call to resize_term() is needed.
        Checking for KEY_RESIZE is generally preferable, unless you're
        not handling the keyboard.

        curses_version() returns a string describing the version of
        PDCurses.

  Return Value:
        All functions return NULL on error, except endwin(), which
        returns ERR on error.

  Portability                                X/Open    BSD    SYS V
        initscr                                 Y       Y       Y
        endwin                                  Y       Y       Y
        isendwin                                Y       -      3.0
        newterm                                 Y       -       Y
        set_term                                Y       -       Y
        delscreen                               Y       -      4.0
        resize_term                             -       -       -
        is_termresized                          -       -       -
        curses_version                          -       -       -

**man-end****************************************************************/

#include <stdbool.h>
#include <stdlib.h>

static const char *_curses_notice = "PDCurses 3.4 - Public Domain 2008";



WINDOW *initscr(SESSION *S, void *userargs)
{
    int i;

    PDC_LOG(("initscr() - called\n"));

    if (!S || (S->SP && S->SP->alive))
    {
        fprintf(stderr, "initscr(): Unable to create SP\n");
        return NULL;
    }
    if (PDC_scr_open(S, userargs) == ERR)
    {
        fprintf(stderr, "initscr(): Unable to open SP\n");
        return NULL;
    }

    S->SP->autocr = TRUE;       /* cr -> lf by default */
    S->SP->raw_out = FALSE;     /* tty I/O modes */
    S->SP->raw_inp = FALSE;     /* tty I/O modes */
    S->SP->cbreak = TRUE;
    S->SP->save_key_modifiers = FALSE;
    S->SP->return_key_modifiers = FALSE;
    S->SP->echo = TRUE;
    S->SP->visibility = 1;
    S->SP->resized = FALSE;
    S->SP->_trap_mbe = 0L;
    S->SP->_map_mbe_to_key = 0L;
    S->SP->linesrippedoff = 0;
    S->SP->linesrippedoffontop = 0;
    S->SP->delaytenths = 0;
    S->SP->line_color = -1;

    S->SP->orig_cursor = PDC_get_cursor_mode(S);

    S->LINES = S->SP->lines;
    S->COLS = S->SP->cols;

    S->getch_c_gindex = 1;

    if (S->LINES < 2 || S->COLS < 2)
    {
        fprintf(stderr, "initscr(): LINES=%d COLS=%d: too small.\n",
                S->LINES, S->COLS);
        return NULL;
    }

    if ((S->curscr = newwin(S, S->LINES, S->COLS, 0, 0)) == (WINDOW *)NULL)
    {
        fprintf(stderr, "initscr(): Unable to create curscr.\n");
        return NULL;
    }

    PDC_slk_initialize(S);
    S->LINES -= S->SP->slklines;

    /* We have to sort out ripped off lines here, and reduce the height
       of stdscr by the number of lines ripped off */

    for (i = 0; i < S->linesrippedoff; i++)
    {
        if (S->linesripped[i].line < 0)
            (*S->linesripped[i].init)(newwin(S, 1, S->COLS, S->LINES - 1, 0), S->COLS);
        else
            (*S->linesripped[i].init)(newwin(S, 1, S->COLS,
                    S->SP->linesrippedoffontop++, 0), S->COLS);

        S->SP->linesrippedoff++;
        S->LINES--;
    }

    S->linesrippedoff = 0;

    if (!(S->stdscr = newwin(S, S->LINES, S->COLS, S->SP->linesrippedoffontop, 0)))
    {
        fprintf(stderr, "initscr(): Unable to create stdscr.\n");
        return NULL;
    }

    wclrtobot(S, S->stdscr);

    /* If preserving the existing screen, don't allow a screen clear */

    if (S->SP->_preserve)
    {
        untouchwin(S, S->curscr);
        untouchwin(S, S->stdscr);
        S->stdscr->_clear = FALSE;
        S->curscr->_clear = FALSE;
    }
    else
        S->curscr->_clear = TRUE;

    PDC_init_atrtab(S);  /* set up default colors */

    MOUSE_X_POS(S) = MOUSE_Y_POS(S) = -1;
    BUTTON_STATUS(S,1) = BUTTON_RELEASED;
    BUTTON_STATUS(S,2) = BUTTON_RELEASED;
    BUTTON_STATUS(S,3) = BUTTON_RELEASED;
    S->Mouse_status.changes = 0;

    S->SP->alive = TRUE;

    def_shell_mode(S);

    sprintf(S->ttytype, "pdcurses|PDC on %s", PDC_sysname(S));

    return S->stdscr;
}

int endwin(SESSION *S)
{
    PDC_LOG(("endwin() - called\n"));

    /* Allow temporary exit from curses using endwin() */

    def_prog_mode(S);
    PDC_scr_close(S);

    S->SP->alive = FALSE;

    return OK;
}

bool isendwin(SESSION *S)
{
    PDC_LOG(("isendwin() - called\n"));

    return S->SP ? !(S->SP->alive) : FALSE;
}

SCREEN *newterm(SESSION *S, const char *type, FILE *outfd, FILE *infd, void *userargs)
{
    PDC_LOG(("newterm() - called\n"));

    return initscr(S, userargs) ? S->SP : NULL;
}

SCREEN *set_term(SESSION *S, SCREEN *new)
{
    PDC_LOG(("set_term() - called\n"));

    /* We only support one screen */

    return (new == S->SP) ? S->SP : NULL;
}

void delscreen(SESSION *S)
{
    PDC_LOG(("delscreen() - called\n"));

    if (!S)
        return;

    PDC_slk_free(S);     /* free the soft label keys, if needed */

    delwin(S, S->stdscr);
    delwin(S, S->curscr);
    S->stdscr = (WINDOW *)NULL;
    S->curscr = (WINDOW *)NULL;

    S->SP->alive = FALSE;

    PDC_scr_free(S);     /* free SP and pdc_atrtab */

    S->SP = (SCREEN *)NULL;
}

int resize_term(SESSION *S, int nlines, int ncols)
{
    PDC_LOG(("resize_term() - called: nlines %d\n", nlines));

    if (!S)
        return ERR;

    if (!S->stdscr || PDC_resize_screen(S, nlines, ncols) == ERR)
        return ERR;

    int columns=80, rows=24;
    PDC_get_termsize(S, &columns, &rows);
    S->SP->cols = S->COLS = columns;
    S->SP->lines = rows;
    S->LINES = S->SP->lines - S->SP->linesrippedoff - S->SP->slklines;

    if (wresize(S, S->curscr, S->SP->lines, S->SP->cols) == ERR ||
        wresize(S, S->stdscr, S->LINES, S->COLS) == ERR)
        return ERR;

    S->curscr->_clear = TRUE;

    if (S->SP->slk_winptr)
    {
        if (wresize(S, S->SP->slk_winptr, S->SP->slklines, S->COLS) == ERR)
            return ERR;

        wmove(S, S->SP->slk_winptr, 0, 0);
        wclrtobot(S, S->SP->slk_winptr);
        PDC_slk_initialize(S);
        slk_noutrefresh(S);
    }

    touchwin(S, S->stdscr);
    wnoutrefresh(S, S->stdscr);

    return OK;
}

bool is_termresized(SESSION *S)
{
    PDC_LOG(("is_termresized() - called\n"));
    if (!S)
        return 0;
    return S->SP->resized;
}

const char *curses_version(void)
{
    return _curses_notice;
}
