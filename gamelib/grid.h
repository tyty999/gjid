/* sgrid.h
**
**	Defines a scrolling grid of some sort based on Win.
*/

#ifndef __SGRID_H
#define __SGRID_H

#include <mdefs.h>
#include "graph.h"
#include "win.h"

class Grid : public Win {
protected:
    struct {
       int width;
       int height;
    }				cell;
    Rectangle			world;
    Point			macro;		// TLC position in cells
    Point			micro;		// TLC position in pixels
    BOOL			Wrap;

public:
    inline			Grid (int left, int top, int right, int bottom,
				      int width, int height, int cell_width,
				      int cell_height, WORD AHandle);
    virtual inline void		Draw (void);
    virtual inline void		Draw (int x, int y, int xp, int yp) = 0;
    virtual HandlerStatus	CellEvent (int x, int y, ButtonType) = 0;
   virtual inline HandlerStatus MousePressEvent (int x, int y,
   						 ButtonType button);
    virtual inline void		MoveMacro (int xpos, int ypos);
    virtual inline void		MoveMicro (int xpos, int ypos);
    virtual inline void		EnableWrap (void);
    virtual inline void		DisableWrap (void);
    inline void			GetCellSize (int *sx, int *sy) const;
    inline void			SetCellSize (int sx, int sy);
    inline void			GetMicro (int *xp, int *yp) const;
};

/////////////////////////////////////////////////////////////////////////////

inline Grid :: Grid (int left, int top, int right, int bottom,
	      int width, int height, int cell_width,
	      int cell_height, WORD AHandle)
: Win (left, top, right, bottom, AHandle)
{
    world.Set (0, 0, area.width, area.height);
    cell.width = cell_width;
    cell.height = cell_height;
    micro.x = micro.y = 0;
    macro.x = macro.y = 0;
}

inline void Grid :: Draw (void)
{
int x, y;
int vish, visw;
int cellx, celly;
int aw, ah;

    mouse.Hide();
    PushGC();
    SetClipWindow (area);

    aw = min (area.width, world.width);
    ah = min (area.height, world.height);
    visw = aw / cell.width + ((aw % cell.width) != 0);
    vish = ah / cell.height + ((ah % cell.height) != 0);

    celly = area.y1 - micro.y % cell.height;
    WaitRetrace();
    for (y = 0; y < vish; ++ y) {
       cellx = area.x1 - micro.x % cell.width;
       for (x = 0; x < visw; ++ x) {
	  Draw (macro.x + x, macro.y + y, cellx, celly);
	  cellx += cell.width;
       }
       celly += cell.height;
    }

    PopGC();
    mouse.RescanBackgr();
    mouse.Show();
}

inline HandlerStatus Grid :: MousePressEvent (int x, int y, ButtonType button)
{
    return (CellEvent ((micro.x + x) / cell.width, 
    		       (micro.y + y) / cell.height, button));
}

inline void Grid :: MoveMacro (int xpos, int ypos)
{
    MoveMicro (xpos * cell.width, ypos * cell.height);
}

inline void Grid :: MoveMicro (int xpos, int ypos)
{
register int maxx, maxy;

    if ((area.width >= world.width && xpos != micro.x) ||
        (area.height >= world.height && ypos != micro.y))
       return;

    maxx = max (world.width - area.width, 0);
    maxy = max (world.height - area.height, 0);

    if (!Wrap) {
       if (xpos < 0)
	  xpos = 0;
       else if (xpos > maxx)
	  xpos = maxx;
       if (ypos < 0)
	  ypos = 0;
       else if (ypos > maxy)
	  ypos = maxy;
    }

    // Wrap or not, but clip the coordinates
    if (xpos < 0 || xpos >= world.width)
       xpos = xpos % world.width;
    if (ypos < 0 || ypos >= world.height)
       ypos = ypos % world.height;

    micro.x = xpos;
    micro.y = ypos;
    macro.x = xpos / cell.width;
    macro.y = ypos / cell.height;
}

inline void Grid :: EnableWrap (void)
{
    Wrap = TRUE;
    // No move is needed since enabling wrap does not change the picture
}

inline void Grid :: DisableWrap (void)
{
    Wrap = FALSE;
    // Perform a move to the current position, which will clip if needed
    MoveMicro (micro.x, micro.y);
}

inline void Grid :: GetCellSize (int *sx, int *sy) const
{
    *sx = cell.width;
    *sy = cell.height;
}

inline void Grid :: SetCellSize (int sx, int sy)
{
int ww, wh;

    if (sx != 0 && sy != 0) {
       ww = world.width / cell.width;
       wh = world.height / cell.height;
       world.Set (world.x1, world.y1, world.x1 + ww * sx, world.y1 + wh * sy);

       if (world.width - micro.x < area.width) {
	  micro.x = max (0, world.width - area.width);
          macro.x = micro.x / cell.width;
       }
       if (world.height - micro.y < area.height) {
	  micro.y = max (0, world.height - area.height);
          macro.y = micro.x / cell.height;
       }

       cell.width = sx;
       cell.height = sy;
    }
}

inline void Grid :: GetMicro (int *xp, int *yp) const
{
    *xp = micro.x;
    *yp = micro.y;
}

#endif

