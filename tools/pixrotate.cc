/* pixrotate.cc
**
**	Rotates a .pix file by 90 degrees.
*/

#include <fstream.h>
#include <game/icon.h>

int main (int argc, char ** argv)
{
Icon pix, rpix;
int x, y;

    if (argc < 2) {
       cout << "Usage: " << argv[0] << " <file.pix>\n";
       return (1);
    }

    pix.Load (argv[1]);
    if (pix.Height() != pix.Width()) {
       cout << "Sorry, only square pictures are supported.\n";
       return (0);
    }
       
    rpix = pix;

    for (y = 0; y < pix.Height(); ++ y)
       for (x = 0; x < pix.Width(); ++ x)
	  rpix.SetPixel (pix.Height() - (y + 1), x, pix.GetPixel (x, y));

    rpix.Save (argv[1]);
    return (0); 
}

