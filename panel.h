/* Public Domain Curses */

/* $Id: panel.h,v 1.19 2008/07/13 16:08:16 wmcbrine Exp $ */

/*----------------------------------------------------------------------*
 *                         Panels for PDCurses                          *
 *----------------------------------------------------------------------*/

#ifndef __PDCURSES_PANEL_H__
#define __PDCURSES_PANEL_H__ 1

#include <curses.h>

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
extern "C"
{
#endif

typedef struct panelobs
{
    struct panelobs *above;
    struct panel *pan;
} PANELOBS;

typedef struct panel
{
    WINDOW *win;
    int wstarty;
    int wendy;
    int wstartx;
    int wendx;
    struct panel *below;
    struct panel *above;
    const void *user;
    struct panelobs *obscure;
} PANEL;

int     bottom_panel(SESSION *S, PANEL *pan);
int     del_panel(SESSION *S, PANEL *pan);
int     hide_panel(SESSION *S, PANEL *pan);
int     move_panel(SESSION *S, PANEL *pan, int starty, int startx);
PANEL  *new_panel(SESSION *S, WINDOW *win);
PANEL  *panel_above(SESSION *S, const PANEL *pan);
PANEL  *panel_below(SESSION *S, const PANEL *pan);
int     panel_hidden(SESSION *S, const PANEL *pan);
const void *panel_userptr(const PANEL *pan);
WINDOW *panel_window(const PANEL *pan);
int     replace_panel(SESSION *S, PANEL *pan, WINDOW *win);
int     set_panel_userptr(PANEL *pan, const void *uptr);
int     show_panel(SESSION *S, PANEL *pan);
int     top_panel(SESSION *S, PANEL *pan);
void    update_panels(SESSION *S);

#if defined(__cplusplus) || defined(__cplusplus__) || defined(__CPLUSPLUS)
}
#endif

#endif /* __PDCURSES_PANEL_H__ */
