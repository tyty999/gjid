/* mob.cc
**
**	Moving object is an image with background, that can be moved on screen.
** Typical mob is a mouse.
*/

#include <game/mob.h>

Mob :: Mob (void)
{
    ox = nx = oy = ny = 0;
    Hidden = TRUE;
}

void Mob :: SetImage (const Icon& NewImage)
{
    backgr = NewImage;
    image = NewImage;
    projection = NewImage;
}

void Mob :: RescanBackgr (void)
{
    backgr.Get (nx, ny);
}

void Mob :: Show (void)
{
    if (Blending == NoBlend)
       image.Put (nx,ny);
    else {
       projection = image;
       projection.BlendWith (backgr, Blending);
       projection.Put (nx,ny);
    }
    Hidden = FALSE;
}

void Mob :: Hide (void)
{
    if (!Hidden) {
       backgr.Put (nx, ny);
       Hidden = TRUE;
    }
}

void Mob :: MoveTo (WORD x, WORD y)
{
    if (nx != x || ny != y) {
       ox = nx; oy = ny; 
       nx = x; ny = y;
       if (!Hidden)
          backgr.Put (ox, oy);
       backgr.Get (nx, ny);
       if (!Hidden) {
	  if (Blending == NoBlend)
	     image.Put (nx, ny);
	  else {
	     projection = image;
	     projection.BlendWith (backgr, Blending);
	     projection.Put (nx,ny);
	  }
       }
    }
    else if (!Hidden)
       Show();
}

Mob :: ~Mob (void)
{
}

