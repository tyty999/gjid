// pixrotate.cc
//
//	Rotates a .pix file by 90 degrees.
//

#include "../gamelib/icon.h"

int main (int argc, char ** argv)
{
    if (argc < 2) {
	cout << "Usage: " << argv[0] << " <file.pix>\n";
	return (1);
    }

    memblock buf;
    buf.read_file (argv[1]);
    istream is (buf);
    Icon pix;
    is >> pix;

    if (pix.Height() != pix.Width()) {
	cout.format ("Sorry, only square pictures are supported. (this one is %ux%u)\n", pix.Width(), pix.Height());
	return (1);
    }

    Icon rpix (pix);
    for (dim_t y = 0; y < pix.Height(); ++ y)
	for (dim_t x = 0; x < pix.Width(); ++ x)
	    rpix.SetPixel (pix.Height() - (y + 1), x, pix.GetPixel (x, y));

    ostream os (buf);
    os << rpix;
    buf.write_file (argv[1]);
    return (0);
}

