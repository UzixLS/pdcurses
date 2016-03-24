/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: getch.c,v 1.72 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         getch

  Synopsis:
        int getch(void);
        int wgetch(WINDOW *win);
        int mvgetch(int y, int x);
        int mvwgetch(WINDOW *win, int y, int x);
        int ungetch(int ch);
        int flushinp(void);

        int get_wch(wint_t *wch);
        int wget_wch(WINDOW *win, wint_t *wch);
        int mvget_wch(int y, int x, wint_t *wch);
        int mvwget_wch(WINDOW *win, int y, int x, wint_t *wch);
        int unget_wch(const wchar_t wch);

        unsigned long PDC_get_key_modifiers(void);
        int PDC_save_key_modifiers(bool flag);
        int PDC_return_key_modifiers(bool flag);

  Description:
        With the getch(), wgetch(), mvgetch(), and mvwgetch() functions,
        a character is read from the terminal associated with the window.
        In nodelay mode, if there is no input waiting, the value ERR is
        returned. In delay mode, the program will hang until the system
        passes text through to the program. Depending on the setting of
        cbreak(), this will be after one character or after the first
        newline.  Unless noecho() has been set, the character will also
        be echoed into the designated window.

        If keypad() is TRUE, and a function key is pressed, the token for
        that function key will be returned instead of the raw characters.
        Possible function keys are defined in <curses.h> with integers
        beginning with 0401, whose names begin with KEY_.

        If nodelay(win, TRUE) has been called on the window and no input
        is waiting, the value ERR is returned.

        ungetch() places ch back onto the input queue to be returned by
        the next call to wgetch().

        flushinp() throws away any type-ahead that has been typed by the
        user and has not yet been read by the program.

        PDC_get_key_modifiers() returns the keyboard modifiers (shift,
        control, alt, numlock) effective at the time of the last getch()
        call, if PDC_save_key_modifiers(TRUE) has been called before the
        getch(). Use the macros PDC_KEY_MODIFIER_* to determine which
        modifier(s) were set. PDC_return_key_modifiers() tells getch()
        to return modifier keys pressed alone as keystrokes (KEY_ALT_L,
        etc.). These may not work on all platforms.

        NOTE: getch() and ungetch() are implemented as macros, to avoid
        conflict with many DOS compiler's runtime libraries.

  Return Value:
        These functions return ERR or the value of the character, meta
        character or function key token.

  Portability                                X/Open    BSD    SYS V
        getch                                   Y       Y       Y
        wgetch                                  Y       Y       Y
        mvgetch                                 Y       Y       Y
        mvwgetch                                Y       Y       Y
        ungetch                                 Y       Y       Y
        flushinp                                Y       Y       Y
        get_wch                                 Y
        wget_wch                                Y
        mvget_wch                               Y
        mvwget_wch                              Y
        unget_wch                               Y
        PDC_get_key_modifiers                   -       -       -

**man-end****************************************************************/

static int _mouse_key(SESSION *S, WINDOW *win)
{
    int i, key = KEY_MOUSE;
    unsigned long mbe = S->SP->_trap_mbe;

    if (!S)
        return ERR;

    /* Filter unwanted mouse events */

    for (i = 0; i < 3; i++)
    {
        if (S->mouse_status.changes & (1 << i))
        {
            int shf = i * 5;
            short button = S->mouse_status.button[i] & BUTTON_ACTION_MASK;

            if (   (!(mbe & (BUTTON1_PRESSED << shf)) &&
                    (button == BUTTON_PRESSED))

                || (!(mbe & (BUTTON1_CLICKED << shf)) &&
                    (button == BUTTON_CLICKED))

                || (!(mbe & (BUTTON1_DOUBLE_CLICKED << shf)) &&
                    (button == BUTTON_DOUBLE_CLICKED))

                || (!(mbe & (BUTTON1_MOVED << shf)) &&
                    (button == BUTTON_MOVED))

                || (!(mbe & (BUTTON1_RELEASED << shf)) &&
                    (button == BUTTON_RELEASED))
            )
                S->mouse_status.changes ^= (1 << i);
        }
    }

    if (S->mouse_status.changes & PDC_MOUSE_MOVED)
    {
        if (!(mbe & (BUTTON1_MOVED|BUTTON2_MOVED|BUTTON3_MOVED)))
            S->mouse_status.changes ^= PDC_MOUSE_MOVED;
    }

    if (S->mouse_status.changes &
        (PDC_MOUSE_WHEEL_UP|PDC_MOUSE_WHEEL_DOWN))
    {
        if (!(mbe & MOUSE_WHEEL_SCROLL))
            S->mouse_status.changes &=
                ~(PDC_MOUSE_WHEEL_UP|PDC_MOUSE_WHEEL_DOWN);
    }

    if (!S->mouse_status.changes)
        return -1;

    /* Check for click in slk area */

    i = PDC_mouse_in_slk(S, S->mouse_status.y, S->mouse_status.x);

    if (i)
    {
        if (S->mouse_status.button[0] & (BUTTON_PRESSED|BUTTON_CLICKED))
            key = KEY_F(i);
        else
            key = -1;
    }

    return key;
}

int wgetch(SESSION *S, WINDOW *win)
{
    int key, waitcount;

    PDC_LOG(("wgetch() - called\n"));

    if (!S || !win)
        return ERR;

    waitcount = 0;

     /* set the number of 1/20th second napms() calls */

    if (S->SP->delaytenths)
        waitcount = 2 * S->SP->delaytenths;
    else
        if (win->_delayms)
        {
            /* Can't really do millisecond intervals, so delay in
               1/20ths of a second (50ms) */

            waitcount = win->_delayms / 50;
            if (!waitcount)
                waitcount = 1;
        }

    /* refresh window when wgetch is called if there have been changes
       to it and it is not a pad */

    if (!(win->_flags & _PAD) && ((!win->_leaveit &&
         (win->_begx + win->_curx != S->SP->curscol ||
          win->_begy + win->_cury != S->SP->cursrow)) || is_wintouched(S, win)))
        wrefresh(S, win);

    /* if ungotten char exists, remove and return it */

    if (S->getch_c_ungind)
        return S->getch_c_ungch[--S->getch_c_ungind];

    /* if normal and data in buffer */

    if ((!S->SP->raw_inp && !S->SP->cbreak) && (S->getch_c_gindex < S->getch_c_pindex))
        return S->getch_buffer[S->getch_c_gindex++];

    /* prepare to buffer data */

    S->getch_c_pindex = 0;
    S->getch_c_gindex = 0;

    /* to get here, no keys are buffered. go and get one. */

    for (;;)            /* loop for any buffering */
    {
        /* is there a keystroke ready? */

        if (!PDC_check_key(S))
        {
            /* if not, handle timeout() and halfdelay() */

            if (S->SP->delaytenths || win->_delayms)
            {
                if (!waitcount)
                    return ERR;

                waitcount--;
            }
            else
                if (win->_nodelay)
                    return ERR;

            napms(S, 50);  /* sleep for 1/20th second */
            continue;   /* then check again */
        }

        /* if there is, fetch it */

        key = PDC_get_key(S);

        if (S->SP->key_code)
        {
            /* filter special keys if not in keypad mode */

            if (!win->_use_keypad)
                key = -1;

            /* filter mouse events; translate mouse clicks in the slk
               area to function keys */

            else if (key == KEY_MOUSE)
                key = _mouse_key(S, win);
        }

        /* unwanted key? loop back */

        if (key == -1)
            continue;

        /* translate CR */

        if (key == '\r' && S->SP->autocr && !S->SP->raw_inp)
            key = '\n';

        /* if echo is enabled */

        if (S->SP->echo && !S->SP->key_code)
        {
            waddch(S, win, key);
            wrefresh(S, win);
        }

        /* if no buffering */

        if (S->SP->raw_inp || S->SP->cbreak)
            return key;

        /* if no overflow, put data in buffer */

        if (key == '\b')
        {
            if (S->getch_c_pindex > S->getch_c_gindex)
                S->getch_c_pindex--;
        }
        else
            if (S->getch_c_pindex < _INBUFSIZ - 2)
                S->getch_buffer[S->getch_c_pindex++] = key;

        /* if we got a line */

        if (key == '\n' || key == '\r')
            return S->getch_buffer[S->getch_c_gindex++];
    }
}

int mvgetch(SESSION *S, int y, int x)
{
    PDC_LOG(("mvgetch() - called\n"));

    if (move(S, y, x) == ERR)
        return ERR;

    return wgetch(S, S->stdscr);
}

int mvwgetch(SESSION *S, WINDOW *win, int y, int x)
{
    PDC_LOG(("mvwgetch() - called\n"));

    if (wmove(S, win, y, x) == ERR)
        return ERR;

    return wgetch(S, win);
}

int PDC_ungetch(SESSION *S, int ch)
{
    PDC_LOG(("ungetch() - called\n"));

    if (!S)
        return ERR;

    if (S->getch_c_ungind >= NUNGETCH)   /* pushback stack full */
        return ERR;

    S->getch_c_ungch[S->getch_c_ungind++] = ch;

    return OK;
}

int flushinp(SESSION *S)
{
    PDC_LOG(("flushinp() - called\n"));

    if (!S)
        return ERR;

    PDC_flushinp(S);

    S->getch_c_gindex = 1;           /* set indices to kill buffer */
    S->getch_c_pindex = 0;
    S->getch_c_ungind = 0;           /* clear c_ungch array */

    return OK;
}

unsigned long PDC_get_key_modifiers(SESSION *S)
{
    PDC_LOG(("PDC_get_key_modifiers() - called\n"));
    if (!S)
        return ERR;
    return S->key_modifiers;
}

int PDC_save_key_modifiers(SESSION *S, bool flag)
{
    PDC_LOG(("PDC_save_key_modifiers() - called\n"));
    if (!S)
        return ERR;
    S->SP->save_key_modifiers = flag;
    return OK;
}

int PDC_return_key_modifiers(SESSION *S, bool flag)
{
    PDC_LOG(("PDC_return_key_modifiers() - called\n"));
    if (!S)
        return ERR;
    S->SP->return_key_modifiers = flag;
    return PDC_modifiers_set(S);
}

