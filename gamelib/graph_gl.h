/* graph_gl.h
**
**	Defines graphic functions in graph.h using svgalib and gl library.
*/

#ifndef __GRAPH_GL_H
#define __GRAPH_GL_H

#include <mdefs.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>

typedef GraphicsContext	*	GC;
typedef BYTE			ColorID;
typedef int			PalID;

/*
** First, some initialization routines. Most graphic systems use 'contexts',
** so we'll use one too. Do not assume anything about its structure.
*/

void GraphInit (int mode = G320x200x256);
void SetGC (void * vbuf, WORD bitspp = 8);
void SetGC (GC NewGC);
GC GetGC (void);
void PrintGC (void);
void PushGC (void);
void PopGC (void);
void ResizeGC (WORD NewWidth, WORD NewHeight);
void EnableClipping (void);
void SetClipWindow (WORD x1, WORD y1, WORD x2, WORD y2);
void SetClipWindow (const Rectangle& area);
void DisableClipping (void);
void GraphClose (void);

/* 
** Next section describes some graphics primitives.
*/

inline void WaitRetrace (void)
{
    vga_waitretrace();
}

inline void ClearScreen (ColorID color = 0)
{
    gl_clearscreen (color);
}

inline void SetPixel (WORD x, WORD y, ColorID color)
{
    gl_setpixel (x, y, color);
}

inline void SetPixel (const Point& loc, ColorID color)
{
    gl_setpixel (loc.x, loc.y, color);
}

inline void SetPixelRGB (WORD x, WORD y, PalID r, PalID g, PalID b)
{
    gl_setpixelrgb (x, y, r, g, b);
}

inline void SetPixelRGB (const Point& loc, PalID r, PalID g, PalID b)
{
    gl_setpixelrgb (loc.x, loc.y, r, g, b);
}

inline ColorID GetPixel (WORD x, WORD y)
{
    return (gl_getpixel (x, y));
}

inline ColorID GetPixel (const Point& loc)
{
    return (gl_getpixel (loc.x, loc.y));
}

inline void GetPixelRGB (WORD x, WORD y, PalID * r, PalID * g, PalID * b)
{
    gl_getpixelrgb (x, y, r, g, b);
}

inline void GetPixelRGB (const Point& loc, PalID * r, PalID * g, PalID * b)
{
    gl_getpixelrgb (loc.x, loc.y, r, g, b);
}

inline void Line (int x1, int y1, int x2, int y2, ColorID color)
{
    if (y1 == y2)
       gl_hline (x1, y1, x2, color);
    else 
       gl_line (x1, y1, x2, y2, color);
}

inline void Line (const Point& from, const Point& to, ColorID color)
{
    if (from.y == to.y)
       gl_hline (from.x, from.y, to.x, color);
    else 
       gl_line (from.x, from.y, to.x, to.y, color);
}

inline void DrawCircle (int x, int y, int r, int c)
{
    gl_circle (x, y, r, c);
}

inline void DrawCircle (const Point& center, int r, int c)
{
    gl_circle (center.x, center.y, r, c);
}

inline void DrawRect (int x1, int y1, int x2, int y2, ColorID color)
{
    gl_hline (x1, y1, x2, color);
    gl_hline (x1, y2, x2, color);
    ++ y1; -- y2;
    gl_line (x2, y1, x2, y2, color);
    gl_line (x1, y1, x1, y2, color);
}

inline void DrawRect (const Rectangle& area, ColorID color)
{
    gl_hline (area.x1, area.y1, area.x2 - 1, color);
    gl_hline (area.x1, area.y2 - 1, area.x2 - 1, color);
    gl_line (area.x2 - 1, area.y1 + 1, area.x2 - 1, area.y2 - 1, color);
    gl_line (area.x1, area.y1 + 1, area.x1, area.y2 - 1, color);
}

inline void FillRect (int x1, int y1, int x2, int y2, ColorID color)
{
    gl_fillbox (x1, y1, x2 - x1, y2 - y1, color);
}

inline void FillRect (const Rectangle& area, ColorID color)
{
    gl_fillbox (area.x1, area.y1, area.width, area.height, color);
}

/*
** Image functions - blits, copy, scale, etc.
*/

inline void PutImage (int x, int y, WORD width, WORD height, ColorID * bits)
{
    gl_putbox (x, y, width, height, bits);
}

inline void GetImage (int x, int y, WORD width, WORD height, ColorID * bits)
{
    gl_getbox (x, y, width, height, bits);
}

inline void CopyImage (int x1, int y1, WORD width, WORD height, int x2, int y2)
{
    gl_copybox (x1, y1, width, height, x2, y2);
}

inline void CopyImage (GC gc, int x1, int y1, WORD width, 
			WORD height, int x2, int y2)
{
    gl_copyboxfromcontext (gc, x1, y1, width, height, x2, y2);
}

inline void CopyImage (int x1, int y1, WORD width, 
			WORD height, GC gc, int x2, int y2)
{
    gl_copyboxtocontext (x1, y1, width, height, gc, x2, y2);
}

inline void ScaleImage (WORD w1, WORD h1, ColorID * i1, 
			WORD w2, WORD h2, ColorID * i2)
{
    gl_scalebox (w1, h1, i1, w2, h2, i2);
}

/*
** Palette functions.
*/

inline void SetPaletteColor (ColorID ncol, PalID red, PalID green, PalID blue)
{
    gl_setpalettecolor (ncol, red, green, blue);
}

inline void GetPaletteColor (ColorID ncol, PalID * r, PalID * g, PalID * b)
{
    gl_getpalettecolor (ncol, r, g, b);
}

inline void SetPaletteColors (WORD start, WORD end, void * buffer)
{
    gl_setpalettecolors (start, end, buffer);
}

inline void GetPaletteColors (WORD start, WORD end, void * buffer)
{
    gl_getpalettecolors (start, end, buffer);
}

/*
** Mouse functions.
*/

inline void MouseInit (void)
{
    vga_setmousesupport (1);	// This initializes the mouse
}

inline void MouseSetPosition (WORD x, WORD y)
{
    mouse_setposition (x, y);
}

inline void MouseSetPosition (const Point& loc)
{
    mouse_setposition (loc.x, loc.y);
}

inline void MouseUpdate (void)
{
    mouse_update();
}

inline WORD MouseXpos (void)
{
    return (mouse_getx());
}

inline WORD MouseYpos (void)
{
    return (mouse_gety());
}

inline WORD MouseButton (void)
{
    return (mouse_getbutton());
}

inline void MouseClose (void)
{
}

/*
** Keyboard handling.
*/

WORD GetKey (void);
WORD GetKeyWait (void);

#endif

