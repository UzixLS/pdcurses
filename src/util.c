/* Public Domain Curses */

#include "curspriv.h"

RCSID("$Id: util.c,v 1.71 2008/07/13 16:08:18 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         util

  Synopsis:
        char *unctrl(chtype c);
        void filter(void);
        void use_env(bool x);
        int delay_output(int ms);

        int getcchar(const cchar_t *wcval, wchar_t *wch, attr_t *attrs,
                     short *color_pair, void *opts);
        int setcchar(cchar_t *wcval, const wchar_t *wch, const attr_t attrs,
                     short color_pair, const void *opts);
        wchar_t *wunctrl(cchar_t *wc);

        int PDC_mbtowc(wchar_t *pwc, const char *s, size_t n);
        size_t PDC_mbstowcs(wchar_t *dest, const char *src, size_t n);
        size_t PDC_wcstombs(char *dest, const wchar_t *src, size_t n);

  Description:
        unctrl() expands the text portion of the chtype c into a
        printable string. Control characters are changed to the "^X"
        notation; others are passed through. wunctrl() is the wide-
        character version of the function.

        filter() and use_env() are no-ops in PDCurses.

        delay_output() inserts an ms millisecond pause in output.

        getcchar() works in two modes: When wch is not NULL, it reads
        the cchar_t pointed to by wcval and stores the attributes in
        attrs, the color pair in color_pair, and the text in the
        wide-character string wch. When wch is NULL, getcchar() merely
        returns the number of wide characters in wcval. In either mode,
        the opts argument is unused.

        setcchar constructs a cchar_t at wcval from the wide-character
        text at wch, the attributes in attr and the color pair in
        color_pair. The opts argument is unused.

        Currently, the length returned by getcchar() is always 1 or 0.
        Similarly, setcchar() will only take the first wide character
        from wch, and ignore any others that it "should" take (i.e.,
        combining characters). Nor will it correctly handle any
        character outside the basic multilingual plane (UCS-2).

  Return Value:
        unctrl() and wunctrl() return NULL on failure. delay_output()
        always returns OK.

        getcchar() returns the number of wide characters wcval points to
        when wch is NULL; when it's not, getcchar() returns OK or ERR.

        setcchar() returns OK or ERR.

  Portability                                X/Open    BSD    SYS V
        unctrl                                  Y       Y       Y
        filter                                  Y       -      3.0
        use_env                                 Y       -      4.0
        delay_output                            Y       Y       Y
        getcchar                                Y
        setcchar                                Y
        wunctrl                                 Y
        PDC_mbtowc                              -       -       -
        PDC_mbstowcs                            -       -       -
        PDC_wcstombs                            -       -       -

**man-end****************************************************************/


char *unctrl(chtype c)
{
    static char strbuf[3] = {0, 0, 0};

    chtype ic;

    PDC_LOG(("unctrl() - called\n"));

    ic = c & A_CHARTEXT;

    if (ic >= 0x20 && ic != 0x7f)       /* normal characters */
    {
        strbuf[0] = (char)ic;
        strbuf[1] = '\0';
        return strbuf;
    }

    strbuf[0] = '^';            /* '^' prefix */

    if (ic == 0x7f)             /* 0x7f == DEL */
        strbuf[1] = '?';
    else                    /* other control */
        strbuf[1] = (char)(ic + '@');

    return strbuf;
}

void filter(SESSION *S)
{
    PDC_LOG(("filter() - called\n"));
}

void use_env(SESSION *S, bool x)
{
    PDC_LOG(("use_env() - called: x %d\n", x));
}

int delay_output(SESSION *S, int ms)
{
    PDC_LOG(("delay_output() - called: ms %d\n", ms));

    return napms(S, ms);
}

int LINES(SESSION *S)
{
    return S->LINES;
}

int COLS(SESSION *S)
{
    return S->COLS;
}

WINDOW* stdscr(SESSION *S)
{
    return S->stdscr;
}

unsigned size_of_SESSION(void)
{
    return sizeof(SESSION);
}

extern MOUSE_STATUS *Mouse_status(SESSION *S)
{
    return &S->Mouse_status;
}

