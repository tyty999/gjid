// graph_fb.cc
//
//	Defines graphic functions in graph.h using the framebuffer.
//

#include "graph_fb.h"

void GraphInit (int)
{
}

void SetGC (void*, WORD)
{
}

void SetGC (GC)
{
}

GC GetGC (void)
{
    return (GC());
}

void PrintGC (void)
{
}

void PushGC (void)
{
}

void PopGC (void)
{
}

void ResizeGC (WORD, WORD)
{
}

void EnableClipping (void)
{
}

void SetClipWindow (WORD, WORD, WORD, WORD)
{
}

void SetClipWindow (const Rectangle&)
{
}

void DisableClipping (void)
{
}

void GraphClose (void)
{
}

void WaitRetrace (void)
{
}

void ClearScreen (ColorID)
{
}

void SetPixel (WORD, WORD, ColorID)
{
}

void SetPixel (const Point&, ColorID)
{
}

void SetPixelRGB (WORD, WORD, PalID, PalID, PalID)
{
}

void SetPixelRGB (const Point&, PalID, PalID, PalID)
{
}

ColorID GetPixel (WORD, WORD)
{
    return (ColorID());
}

ColorID GetPixel (const Point&)
{
    return (ColorID());
}

void GetPixelRGB (WORD, WORD, PalID *, PalID *, PalID *)
{
}

void GetPixelRGB (const Point&, PalID *, PalID *, PalID *)
{
}

void Line (int, int, int, int, ColorID)
{
}

void Line (const Point&, const Point&, ColorID)
{
}

void DrawCircle (int, int, int, int)
{
}

void DrawCircle (const Point&, int, int)
{
}

void DrawRect (int, int, int, int, ColorID)
{
}

void DrawRect (const Rectangle&, ColorID)
{
}

void FillRect (int, int, int, int, ColorID)
{
}

void FillRect (const Rectangle&, ColorID)
{
}

//
// Image functions - blits, copy, scale, etc.
//
void PutImage (int, int, WORD, WORD, ColorID*)
{
}

void GetImage (int, int, WORD, WORD, ColorID*)
{
}

void CopyImage (int, int, WORD, WORD, int, int)
{
}

void CopyImage (GC, int, int, WORD, WORD, int, int)
{
}

void CopyImage (int, int, WORD, WORD, GC, int, int)
{
}

void ScaleImage (WORD, WORD, ColorID*, WORD, WORD, ColorID*)
{
}

//
// Palette functions.
//
void SetPaletteColor (ColorID, PalID, PalID, PalID)
{
}

void GetPaletteColor (ColorID, PalID*, PalID*, PalID*)
{
}

void SetPaletteColors (WORD, WORD, void*)
{
}

void GetPaletteColors (WORD, WORD, void*)
{
}

//
// Mouse functions.
//
void MouseInit (void)
{
}

void MouseSetPosition (WORD, WORD)
{
}

void MouseSetPosition (const Point&)
{
}

void MouseUpdate (void)
{
}

WORD MouseXpos (void)
{
    return (0);
}

WORD MouseYpos (void)
{
    return (0);
}

WORD MouseButton (void)
{
    return (0);
}

void MouseClose (void)
{
}

//
// Keyboard handling.
//
WORD GetKey (void)
{
    return (0);
}

WORD GetKeyWait (void)
{
    return (0);
}

