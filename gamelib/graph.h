/* graph.h
**
**	Defines abstract graphic functions.
*/

#ifndef __GRAPH_H
#define __GRAPH_H

/*
** This file is simply a router that decides which graphic implementation
** to include. All the functions will be expanded inline with no overhead.
**
** In the Makefile you must define the platform you want to compile for:
**
**	GL_GR	- compiles svgalib and GL library support
**	X11_GR	- X11
**	MSW_GR	- Microsoft Windows
**	VGA_GR	- DOS VGA
**	MAC_GR	- Macintosh
**
**=============================================================================
** The following functions are defined for general use:
**=============================================================================
**
******* Initializes graphics mode
** void 	GraphInit (int mode)
**
******* Sets graphics context to a buffer, so that everything is drawn there.
** void 	SetGC (void * vbuf, WORD bitspp = 8)
**
******* Sets graphics context to NewGC, previously obtained using GetGC()
** void 	SetGC (GC NewGC)
**
*******	Returns the graphic context currently in use. Don't tweak!!
** GC 		GetGC (void)
**
******* Pushes current GC on internal stack
** void		PushGC (void)
**
******* Restores GC from internal stack
** void		PopGC (void)
**
******* Sets the dimensions of the context (useful with virtual gcs)
** void 	ResizeGC (WORD NewWidth, WORD NewHeight)
**
*******	Enables clipping (duh!) if supported
** void 	EnableClipping (void)
**
******* Sets clipping rectangle (irregular clips are not supported)
** void 	SetClipWindow (WORD x, WORD y1, WORD x2, WORD y2)
** void 	SetClipWindow (const Rectangle& area)
**
******* Disables clipping
** void 	DisableClipping (void)
**
******* Returns to text mode (or whatever)
** void 	GraphClose (void)
**
******* Waits for retrace. Does nothing where not supported.
** void		WaitRetrace (void)
** 
******* Clears the GC
** void 	ClearScreen (ColorID color = 0)
**
******* Draws a point in GC
** void 	SetPixel (WORD x, WORD y, ColorID color)
** void 	SetPixel (const Point& loc, ColorID color)
**
******* Draws a point with given RGB value (if supported) in GC
** void 	SetPixelRGB (WORD x, WORD y, PalID r, PalID g, PalID b)
** void 	SetPixelRGB (const Point& loc, PalID r, PalID g, PalID b)
**
******* Reads the pixel value at x:y in GC
** ColorID 	GetPixel (WORD x, WORD y)
** ColorID 	GetPixel (const Point& loc)
**
******* Same as GetPixel, but returns RGB at x:y in GC
** void 	GetPixelRGB (WORD x, WORD y, PalID * r, PalID * g, PalID * b)
** void 	GetPixelRGB (const Point& loc, PalID * r, PalID * g, PalID * b)
**
******* Draws a line in color in GC
** void 	Line (int x1, int y1, int x2, int y2, ColorID color)
** void 	Line (const Point& from, const Point& to, ColorID color)
**
******* Draws a hollow circle in color in GC
** void 	DrawCircle (int x, int y, int r, int color)
** void 	DrawCircle (const Point& center, int r, int color)
**
******* Draws a hollow rectangle in color in GC
** void 	DrawRect (int x1, int y1, int x2, int y2, ColorID color)
** void 	DrawRect (const Rectangle& area, ColorID color)
**
******* Draws a filled rectangle in color in GC
** void 	FillRect (int x1, int y1, int x2, int y2, ColorID color)
** void 	FillRect (const Rectangle& area, ColorID color)
**
******* Blits the image pointed to by bits into GC
** void 	PutImage (int x, int y, WORD width, WORD height, ColorID * bits)
**
******* Copies GC at x:y into bits
** void 	GetImage (int x, int y, WORD width, WORD height, ColorID * bits)
**
******* Copies x1:y1-w/h block to x2:y2 in same GC
** void 	CopyImage (int x1, int y1, WORD w, WORD h, int x2, int y2)
**
******* Copies image at x1:y1 in gc to x2:y2 in current gc
** void 	CopyImage (GC gc, int x1, int y1, WORD w, WORD h, int x2,int y2)
**
******* Copies image at x1:y1 in current gc to x2:y2 in gc
** void 	CopyImage (int x1, int y1, WORD w, WORD h, GC gc, int x2,int y2)
**
******* Scales image pointed to by i1 into i2 from w1/h1 to w2/h2
** void 	ScaleImage (WORD w1, WORD h1, ColorID * i1, 
**			    WORD w2, WORD h2, ColorID * i2) 
**
******* Sets RGB value of a color
** void 	SetPaletteColor (ColorID ncol, PalID r, PalID g, PalID b)
**
******* Gets RGB value of a color
** void 	GetPaletteColor (ColorID ncol, PalID * r, PalID * g, PalID * b)
**
******* Sets a block of colors at once
** void 	SetPaletteColors (ColorID start, ColorID end, PalID * buffer)
**
******* Gets a block of colors at once
** void 	GetPaletteColors (ColorID start, ColorID end, PalID * buffer)
**
******* Initializes mouse
** void 	MouseInit (void)
**
******* Moves mouse to x:y
** void 	MouseSetPosition (WORD x, WORD y)
** void 	MouseSetPosition (const Point& loc)
**
******* Updates mouse variables, should be called often
** void 	MouseUpdate (void)
**
******* Returns the X position of the mouse as it was during last Update call
** WORD 	MouseXpos (void)
**
******* Returns the Y position of the mouse as it was during last Update call
** WORD 	MouseYpos (void)
**
******* Returns the button status of the mouse as it was during last Update call
** WORD 	MouseButton (void)
**
******* Deinitializes mouse. Should always be called in the end.
** void 	MouseClose (void)
**
******* Reads a key from the keyboard; if none available returns 0.
**
** int		GetKey (void);
******* Waits for a key from the keyboard.
**
** int		GetKeyWait (void);
**=============================================================================
*/

// Define keyboard return codes
#include "ascii.h"
// Define graphical data structures (point, 3d point, rectangle)
#include "graph_ds.h"

// Include one platform-specific include file
#ifdef GL_GR
#include "graph_gl.h"
#endif
#ifdef X11_GR
#error X11 support is not implemented!
#endif
#ifdef VGA_GR
#error VGA support is not implemented!
#endif
#ifdef MSW_GR
#error Microsoft windows support is not implemented!
#endif
#ifdef MAC_GR
#error Macintosh support not implemented!
#endif

#ifndef GL_GR
   #ifndef VGA_GR
      #ifndef X11_GR
	 #ifndef MSW_GR
	    #ifndef MAC_GR
	       #error Must specify platform!
	    #endif
	 #endif
      #endif
   #endif
#endif

#endif

