/* graph_gl.cc
**
**	These routines use a global variable.
*/

#include "graph.h"
#include <iostream.h>

typedef struct __GCStackEntry {
    GraphicsContext 	gc;
    __GCStackEntry * 	next;
} GCStackEntry;

static GCStackEntry *GCStack = NULL;

/*
** First, some initialization routines. Most graphic systems use 'contexts',
** so we'll use one too. Do not assume anything about its structure.
*/

void GraphInit (int mode)
{
    vga_init();
    vga_setmousesupport (1);
    vga_setmode (mode);
    gl_setcontextvga (mode);
}

void SetGC (void * vbuf, WORD bitspp)
{
    gl_setcontextvirtual (0, 0, bitspp / 8, bitspp, vbuf);
}

void SetGC (GC NewGC)
{
    gl_setcontext (NewGC);
}

GC GetGC (void)
{
GC Current;
    Current = new GraphicsContext;
    gl_getcontext (Current);
    return (Current);
}

void PrintGC (void)
{
GraphicsContext Current;

    gl_getcontext (&Current);

    cout << "-----------------------------\n";
    cout << "Current context:\n";
    cout << "-----------------------------\n";
    cout << "Modetype\t" << (WORD)Current.modetype << "\n";
    cout << "Modeflags\t" << (WORD)Current.modeflags << "\n";
    cout << "Flippage\t" << (WORD)Current.flippage << "\n";
    cout << "width\t\t" << Current.width << "\n";
    cout << "height\t\t" << Current.height << "\n";
    cout << "bytesperpixel\t" << Current.bytesperpixel << "\n";
    cout << "colors\t\t" << Current.colors << "\n";
    cout << "bitsperpixel\t" << Current.bitsperpixel << "\n";
    cout << "bytewidth\t" << Current.bytewidth << "\n";
    cout << "vbuf\t\t" << (WORD)Current.vbuf << "\n";
    cout << "clip\t\t" << Current.clip << "\n";
    cout << "clipx1\t\t" << Current.clipx1 << "\n";
    cout << "clipy1\t\t" << Current.clipy1 << "\n";
    cout << "clipx2\t\t" << Current.clipx2 << "\n";
    cout << "clipy2\t\t" << Current.clipy2 << "\n";
    cout << "ff\t\t" << (WORD)(&Current.ff) << "\n";
    cout << "-----------------------------\n";
    cout.flush();
}

void PushGC (void)
{
GCStackEntry * NewEntry;

    NewEntry = new GCStackEntry;
    gl_getcontext (&NewEntry->gc);
    NewEntry->next = GCStack;
    GCStack = NewEntry;
}

void PopGC (void)
{
GCStackEntry * Entry;
    
    Entry = GCStack;
    if (Entry != NULL) {
       GCStack = Entry->next;
       gl_setcontext (&Entry->gc);
       delete (Entry);
    }
}

void ResizeGC (WORD NewWidth, WORD NewHeight)
{
    gl_setcontextwidth (NewWidth);
    gl_setcontextheight (NewHeight);
}

void EnableClipping (void)
{
    gl_enableclipping();
}

void SetClipWindow (WORD x1, WORD y1, WORD x2, WORD y2)
{
    gl_setclippingwindow (x1, y1, x2 - 1, y2 - 1); 
}

void SetClipWindow (const Rectangle& area)
{
    gl_setclippingwindow (area.x1, area.y1, area.x2 - 1, area.y2 - 1); 
}

void DisableClipping (void)
{
    gl_disableclipping();
}

void GraphClose (void)
{
    vga_setmode (TEXT);
}

/*
** Keyboard handling.
*/

WORD GetKey (void)
{
    return (vga_getkey());
}

WORD GetKeyWait (void)
{
WORD total = 0, key;
    while ((key = vga_getkey()) == 0);
    total += key;
    while ((key = vga_getkey()) != 0)
       total += key;
    return (total);
}

