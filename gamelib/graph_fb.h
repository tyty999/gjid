// graph_fb.h
//
//	Defines graphic functions in graph.h using the framebuffer.
//

#ifndef GRAPH_FB_H_493319565D1D13E24D7DC79C50D05FC5
#define GRAPH_FB_H_493319565D1D13E24D7DC79C50D05FC5

#include <mdefs.h>
#include "graph_ds.h"

typedef int	GC;
typedef BYTE	ColorID;
typedef int	PalID;

enum {
    MOUSE_LEFTBUTTON,
    MOUSE_RIGHTBUTTON
};

//
// First, some initialization routines. Most graphic systems use 'contexts',
// so we'll use one too. Do not assume anything about its structure.
//
void GraphInit (int mode);
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

// 
// Next section describes some graphics primitives.
//
void WaitRetrace (void);
void ClearScreen (ColorID color = 0);
void SetPixel (WORD x, WORD y, ColorID color);
void SetPixel (const Point& loc, ColorID color);
void SetPixelRGB (WORD x, WORD y, PalID r, PalID g, PalID b);
void SetPixelRGB (const Point& loc, PalID r, PalID g, PalID b);
ColorID GetPixel (WORD x, WORD y);
ColorID GetPixel (const Point& loc);
void GetPixelRGB (WORD x, WORD y, PalID * r, PalID * g, PalID * b);
void GetPixelRGB (const Point& loc, PalID * r, PalID * g, PalID * b);
void Line (int x1, int y1, int x2, int y2, ColorID color);
void Line (const Point& from, const Point& to, ColorID color);
void DrawCircle (int x, int y, int r, int c);
void DrawCircle (const Point& center, int r, int c);
void DrawRect (int x1, int y1, int x2, int y2, ColorID color);
void DrawRect (const Rectangle& area, ColorID color);
void FillRect (int x1, int y1, int x2, int y2, ColorID color);
void FillRect (const Rectangle& area, ColorID color);

//
// Image functions - blits, copy, scale, etc.
//
void PutImage (int x, int y, WORD width, WORD height, ColorID * bits);
void GetImage (int x, int y, WORD width, WORD height, ColorID * bits);
void CopyImage (int x1, int y1, WORD width, WORD height, int x2, int y2);
void CopyImage (GC gc, int x1, int y1, WORD width, WORD height, int x2, int y2);
void CopyImage (int x1, int y1, WORD width, WORD height, GC gc, int x2, int y2);
void ScaleImage (WORD w1, WORD h1, ColorID * i1, WORD w2, WORD h2, ColorID * i2);

//
// Palette functions.
//
void SetPaletteColor (ColorID ncol, PalID red, PalID green, PalID blue);
void GetPaletteColor (ColorID ncol, PalID * r, PalID * g, PalID * b);
void SetPaletteColors (WORD start, WORD end, void * buffer);
void GetPaletteColors (WORD start, WORD end, void * buffer);

//
// Mouse functions.
//
void MouseInit (void);
void MouseSetPosition (WORD x, WORD y);
void MouseSetPosition (const Point& loc);
void MouseUpdate (void);
WORD MouseXpos (void);
WORD MouseYpos (void);
WORD MouseButton (void);
void MouseClose (void);

//
// Keyboard handling.
//
WORD GetKey (void);
WORD GetKeyWait (void);

#endif

