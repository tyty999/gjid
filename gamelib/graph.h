// graph_fb.h
//
//	Defines graphic functions in graph.h using the framebuffer.
//

#ifndef GRAPH_FB_H_493319565D1D13E24D7DC79C50D05FC5
#define GRAPH_FB_H_493319565D1D13E24D7DC79C50D05FC5

#include <mdefs.h>
#include "ascii.h"
#include "graph_ds.h"

typedef BYTE	ColorID;
typedef int	coord_t;

void GraphInit (int mode);
void SetClipWindow (coord_t x1, coord_t y1, coord_t x2, coord_t y2);
void GraphClose (void);
void ClearScreen (ColorID color = 0);
void SetPixel (coord_t x, coord_t y, ColorID color);
void DrawRect (int x1, int y1, int x2, int y2, ColorID color);
void FillRect (int x1, int y1, int x2, int y2, ColorID color);
void PutImage (int x, int y, coord_t width, coord_t height, ColorID * bits);
void GetImage (int x, int y, coord_t width, coord_t height, ColorID * bits);
void SetPaletteColors (coord_t start, coord_t end, void * buffer);
wchar_t GetKeyWait (void);

#endif

