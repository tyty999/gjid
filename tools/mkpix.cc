/* mkpic.cc
**
**	Program to make .pix pictures.
*/

#include <game/icon.h>

int main (int argc, char ** argv)
{
int xdim, ydim, x, y, color;
Icon * NewPic;

    if (argc != 5) {
       cout << "Usage: mkpix <x> <y> <color> <name>\n";
       return (1);
    }

    xdim = atoi(argv[1]);
    ydim = atoi(argv[2]);
    color = atoi(argv[3]);

    NewPic = new Icon (xdim, ydim);

    for (y = 0; y < ydim; ++ y)
       for (x = 0; x < xdim; ++ x)
	  NewPic->SetPixel (x, y, color);

    NewPic->Write (argv[4]);

    delete NewPic;

    return (0);
}

