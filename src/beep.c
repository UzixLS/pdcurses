/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: beep.c,v 1.34 2008/07/13 16:08:17 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         beep

  Synopsis:
        int beep(void);
        int flash(void);

  Description:
        beep() sounds the audible bell on the terminal, if possible;
        if not, it calls flash().

        flash() "flashes" the screen, by inverting the foreground and
        background of every cell, pausing, and then restoring the
        original attributes.

  Return Value:
        These functions return OK.

  Portability                                X/Open    BSD    SYS V
        beep                                    Y       Y       Y
        flash                                   Y       Y       Y

**man-end****************************************************************/

int beep(SESSION *S)
{
    PDC_LOG(("beep() - called\n"));

    if (!S)
        return ERR;

    if (S->SP->audible)
        PDC_beep(S);
    else
        flash(S);

    return OK;
}

int flash(SESSION *S)
{
    int z, y, x;

    PDC_LOG(("flash() - called\n"));

    if (!S)
        return ERR;

    /* Reverse each cell; wait; restore the screen */

    for (z = 0; z < 2; z++)
    {
        for (y = 0; y < S->LINES; y++)
            for (x = 0; x < S->COLS; x++)
                S->curscr->_y[y][x] ^= A_REVERSE;

        wrefresh(S, S->curscr);

        if (!z)
            napms(S, 50);
    }

    return OK;
}
