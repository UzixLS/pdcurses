/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: slk.c,v 1.61 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         slk

  Synopsis:
        int slk_init(int fmt);
        int slk_set(int labnum, const char *label, int justify);
        int slk_refresh(void);
        int slk_noutrefresh(void);
        char *slk_label(int labnum);
        int slk_clear(void);
        int slk_restore(void);
        int slk_touch(void);
        int slk_attron(const chtype attrs);
        int slk_attr_on(const attr_t attrs, void *opts);
        int slk_attrset(const chtype attrs);
        int slk_attr_set(const attr_t attrs, short color_pair, void *opts);
        int slk_attroff(const chtype attrs);
        int slk_attr_off(const attr_t attrs, void *opts);
        int slk_color(short color_pair);

        int slk_wset(int labnum, const wchar_t *label, int justify);

        int PDC_mouse_in_slk(int y, int x);
        void PDC_slk_free(void);
        void PDC_slk_initialize(void);

        wchar_t *slk_wlabel(int labnum)

  Description:
        These functions manipulate a window that contain Soft Label Keys
        (SLK). To use the SLK functions, a call to slk_init() must be
        made BEFORE initscr() or newterm(). slk_init() removes 1 or 2
        lines from the useable screen, depending on the format selected.

        The line(s) removed from the screen are used as a separate
        window, in which SLKs are displayed.

        slk_init() requires a single parameter which describes the
        format of the SLKs as follows:

                0       3-2-3 format
                1       4-4 format
                2       4-4-4 format (ncurses extension)
                3       4-4-4 format with index line (ncurses extension)
                        2 lines used
                55      5-5 format (pdcurses format)

        slk_refresh(), slk_noutrefresh() and slk_touch() are analogous
        to refresh(), noutrefresh() and touch().

  Return Value:
        All functions return OK on success and ERR on error.

  Portability                                X/Open    BSD    SYS V
        slk_init                                Y       -       Y
        slk_set                                 Y       -       Y
        slk_refresh                             Y       -       Y
        slk_noutrefresh                         Y       -       Y
        slk_label                               Y       -       Y
        slk_clear                               Y       -       Y
        slk_restore                             Y       -       Y
        slk_touch                               Y       -       Y
        slk_attron                              Y       -       Y
        slk_attrset                             Y       -       Y
        slk_attroff                             Y       -       Y
        slk_attr_on                             Y
        slk_attr_set                            Y
        slk_attr_off                            Y
        slk_wset                                Y
        PDC_mouse_in_slk                        -       -       -
        PDC_slk_free                            -       -       -
        PDC_slk_initialize                      -       -       -
        slk_wlabel                              -       -       -

**man-end****************************************************************/


enum { LABEL_NORMAL = 8, LABEL_EXTENDED = 10, LABEL_NCURSES_EXTENDED = 12 };



struct SLK {
    chtype label[32];
    int len;
    int format;
    int start_col;
};

/* slk_init() is the slk initialization routine.
   This must be called before initscr().

   label_fmt = 0, 1 or 55.
       0 = 3-2-3 format
       1 = 4 - 4 format
       2 = 4-4-4 format (ncurses extension for PC 12 function keys)
       3 = 4-4-4 format (ncurses extension for PC 12 function keys -
    with index line)
      55 = 5 - 5 format (extended for PC, 10 function keys) */

int slk_init(SESSION *S, int fmt)
{
    PDC_LOG(("slk_init() - called\n"));

    if (!S || S->SP)
        return ERR;

    switch (fmt)
    {
    case 0:  /* 3 - 2 - 3 */
        S->slk_labels = LABEL_NORMAL;
        break;

    case 1:   /* 4 - 4 */
        S->slk_labels = LABEL_NORMAL;
        break;

    case 2:   /* 4 4 4 */
        S->slk_labels = LABEL_NCURSES_EXTENDED;
        break;

    case 3:   /* 4 4 4  with index */
        S->slk_labels = LABEL_NCURSES_EXTENDED;
        break;

    case 55:  /* 5 - 5 */
        S->slk_labels = LABEL_EXTENDED;
        break;

    default:
        return ERR;
    }

    S->slk_label_fmt = fmt;

    S->slk = PDC_calloc(S->slk_labels, sizeof(struct SLK));

    if (!S->slk)
        S->slk_labels = 0;

    return S->slk ? OK : ERR;
}

/* draw a single button */

static void _drawone(SESSION *S, int num)
{
    int i, col, slen;

    if (!S)
        return;

    if (S->slk_hidden)
        return;

    slen = S->slk[num].len;

    switch (S->slk[num].format)
    {
    case 0:  /* LEFT */
        col = 0;
        break;

    case 1:  /* CENTER */
        col = (S->slk_label_length - slen) / 2;

        if (col + slen > S->slk_label_length)
            --col;
        break;

    default:  /* RIGHT */
        col = S->slk_label_length - slen;
    }

    wmove(S, S->SP->slk_winptr, S->slk_label_line, S->slk[num].start_col);

    for (i = 0; i < S->slk_label_length; ++i)
        waddch(S, S->SP->slk_winptr, (i >= col && i < (col + slen)) ?
                S->slk[num].label[i - col] : ' ');
}

/* redraw each button */

static void _redraw(SESSION *S)
{
    int i;

    for (i = 0; i < S->slk_labels; ++i)
        _drawone(S, i);
}

/* slk_set() Used to set a slk label to a string.

   labnum  = 1 - 8 (or 10) (number of the label)
   label   = string (8 or 7 bytes total), or NULL
   justify = 0 : left, 1 : center, 2 : right  */

int slk_set(SESSION *S, int labnum, const char *label, int justify)
{
    PDC_LOG(("slk_set() - called\n"));

    if (!S)
        return ERR;

    if (labnum < 1 || labnum > S->slk_labels || justify < 0 || justify > 2)
        return ERR;

    labnum--;

    if (!label || !(*label))
    {
        /* Clear the label */

        *S->slk[labnum].label = 0;
        S->slk[labnum].format = 0;
        S->slk[labnum].len = 0;
    }
    else
    {
        int i, j = 0;

        /* Skip leading spaces */

        while (label[j] == ' ')
            j++;

        /* Copy it */

        for (i = 0; i < S->slk_label_length; i++)
        {
            chtype ch = label[i + j];

            S->slk[labnum].label[i] = ch;

            if (!ch)
                break;
        }

        /* Drop trailing spaces */

        while ((i + j) && (label[i + j - 1] == ' '))
            i--;

        S->slk[labnum].label[i] = 0;
        S->slk[labnum].format = justify;
        S->slk[labnum].len = i;
    }

    _drawone(S, labnum);

    return OK;
}

int slk_refresh(SESSION *S)
{
    PDC_LOG(("slk_refresh() - called\n"));

    return (slk_noutrefresh(S) == ERR) ? ERR : doupdate(S);
}

int slk_noutrefresh(SESSION *S)
{
    PDC_LOG(("slk_noutrefresh() - called\n"));

    return wnoutrefresh(S, S->SP->slk_winptr);
}

char *slk_label(SESSION *S, int labnum)
{
    chtype *p;
    int i;

    PDC_LOG(("slk_label() - called\n"));

    if (!S)
        return (char *)0;

    if (labnum < 1 || labnum > S->slk_labels)
        return (char *)0;

    for (i = 0, p = S->slk[labnum - 1].label; *p; i++)
        S->slk_label_temp[i] = *p++;

    S->slk_label_temp[i] = '\0';

    return S->slk_label_temp;
}

int slk_clear(SESSION *S)
{
    PDC_LOG(("slk_clear() - called\n"));

    if (!S)
        return ERR;

    S->slk_hidden = TRUE;
    werase(S, S->SP->slk_winptr);
    return wrefresh(S, S->SP->slk_winptr);
}

int slk_restore(SESSION *S)
{
    PDC_LOG(("slk_restore() - called\n"));

    if (!S)
        return ERR;

    S->slk_hidden = FALSE;
    _redraw(S);
    return wrefresh(S, S->SP->slk_winptr);
}

int slk_touch(SESSION *S)
{
    PDC_LOG(("slk_touch() - called\n"));

    return touchwin(S, S->SP->slk_winptr);
}

int slk_attron(SESSION *S, const chtype attrs)
{
    int rc;

    PDC_LOG(("slk_attron() - called\n"));

    if (!S)
        return ERR;

    rc = wattron(S, S->SP->slk_winptr, attrs);
    _redraw(S);

    return rc;
}

int slk_attr_on(SESSION *S, const attr_t attrs, void *opts)
{
    PDC_LOG(("slk_attr_on() - called\n"));

    return slk_attron(S, attrs);
}

int slk_attroff(SESSION *S, const chtype attrs)
{
    int rc;

    PDC_LOG(("slk_attroff() - called\n"));

    if (!S)
        return ERR;

    rc = wattroff(S, S->SP->slk_winptr, attrs);
    _redraw(S);

    return rc;
}

int slk_attr_off(SESSION *S, const attr_t attrs, void *opts)
{
    PDC_LOG(("slk_attr_off() - called\n"));

    return slk_attroff(S, attrs);
}

int slk_attrset(SESSION *S, const chtype attrs)
{
    int rc;

    PDC_LOG(("slk_attrset() - called\n"));

    if (!S)
        return ERR;

    rc = wattrset(S, S->SP->slk_winptr, attrs);
    _redraw(S);

    return rc;
}

int slk_color(SESSION *S, short color_pair)
{
    int rc;

    PDC_LOG(("slk_color() - called\n"));

    if (!S)
        return ERR;

    rc = wcolor_set(S, S->SP->slk_winptr, color_pair, NULL);
    _redraw(S);

    return rc;
}

int slk_attr_set(SESSION *S, const attr_t attrs, short color_pair, void *opts)
{
    PDC_LOG(("slk_attr_set() - called\n"));

    return slk_attrset(S, attrs | COLOR_PAIR(color_pair));
}

static void _slk_calc(SESSION *S)
{
    int i, center, col = 0;

    if (!S)
        return;

    S->slk_label_length = S->COLS / S->slk_labels;

    if (S->slk_label_length > 31)
        S->slk_label_length = 31;

    switch (S->slk_label_fmt)
    {
    case 0:     /* 3 - 2 - 3 F-Key layout */

        --S->slk_label_length;

        S->slk[0].start_col = col;
        S->slk[1].start_col = (col += S->slk_label_length);
        S->slk[2].start_col = (col += S->slk_label_length);

        center = S->COLS / 2;

        S->slk[3].start_col = center - S->slk_label_length + 1;
        S->slk[4].start_col = center + 1;

        col = S->COLS - (S->slk_label_length * 3) + 1;

        S->slk[5].start_col = col;
        S->slk[6].start_col = (col += S->slk_label_length);
        S->slk[7].start_col = (col += S->slk_label_length);
        break;

    case 1:     /* 4 - 4 F-Key layout */

        for (i = 0; i < 8; i++)
        {
            S->slk[i].start_col = col;
            col += S->slk_label_length;

            if (i == 3)
                col = S->COLS - (S->slk_label_length * 4) + 1;
        }

        break;

    case 2:     /* 4 4 4 F-Key layout */
    case 3:     /* 4 4 4 F-Key layout with index */

        for (i = 0; i < 4; i++)
        {
            S->slk[i].start_col = col;
            col += S->slk_label_length;
        }

        center = S->COLS/2;

        S->slk[4].start_col = center - (S->slk_label_length * 2) + 1;
        S->slk[5].start_col = center - S->slk_label_length - 1;
        S->slk[6].start_col = center + 1;
        S->slk[7].start_col = center + S->slk_label_length + 1;

        col = S->COLS - (S->slk_label_length * 4) + 1;

        for (i = 8; i < 12; i++)
        {
            S->slk[i].start_col = col;
            col += S->slk_label_length;
        }

        break;

    default:    /* 5 - 5 F-Key layout */

        for (i = 0; i < 10; i++)
        {
            S->slk[i].start_col = col;
            col += S->slk_label_length;

            if (i == 4)
                col = S->COLS - (S->slk_label_length * 5) + 1;
        }
    }

    --S->slk_label_length;

    /* make sure labels are all in window */

    _redraw(S);
}

void PDC_slk_initialize(SESSION *S)
{
    if (S && S->slk)
    {
        if (S->slk_label_fmt == 3)
        {
            S->SP->slklines = 2;
            S->slk_label_line = 1;
        }
        else
            S->SP->slklines = 1;

        if (!S->SP->slk_winptr)
        {
            if ( !(S->SP->slk_winptr = newwin(S, S->SP->slklines, S->COLS,
                                           S->LINES - S->SP->slklines, 0)) )
                return;

            wattrset(S, S->SP->slk_winptr, A_REVERSE);
        }

        _slk_calc(S);

        /* if we have an index line, display it now */

        if (S->slk_label_fmt == 3)
        {
            chtype save_attr;
            int i;

            save_attr = S->SP->slk_winptr->_attrs;
            wattrset(S, S->SP->slk_winptr, A_NORMAL);
            wmove(S, S->SP->slk_winptr, 0, 0);
            whline(S, S->SP->slk_winptr, 0, S->COLS);

            for (i = 0; i < S->slk_labels; i++)
                mvwprintw(S, S->SP->slk_winptr, 0, S->slk[i].start_col, "F%d", i + 1);

            S->SP->slk_winptr->_attrs = save_attr;
        }

        touchwin(S, S->SP->slk_winptr);
    }
}

void PDC_slk_free(SESSION *S)
{
    if (S && S->slk)
    {
        if (S->SP->slk_winptr)
        {
            delwin(S, S->SP->slk_winptr);
            S->SP->slk_winptr = (WINDOW *)NULL;
        }

        PDC_free(S->slk);
        S->slk = (struct SLK *)NULL;

        S->slk_label_length = 0;
        S->slk_labels = 0;
        S->slk_label_fmt = 0;
        S->slk_label_line = 0;
        S->slk_hidden = FALSE;
    }
}

int PDC_mouse_in_slk(SESSION *S, int y, int x)
{
    int i;

    PDC_LOG(("PDC_mouse_in_slk() - called: y->%d x->%d\n", y, x));

    /* If the line on which the mouse was clicked is NOT the last line
       of the screen, we are not interested in it. */

    if (!S || !S->slk || !S->SP->slk_winptr || (y != S->SP->slk_winptr->_begy + S->slk_label_line))
        return 0;

    for (i = 0; i < S->slk_labels; i++)
        if (x >= S->slk[i].start_col && x < (S->slk[i].start_col + S->slk_label_length))
            return i + 1;

    return 0;
}
