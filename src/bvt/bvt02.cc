#include "stdtest.h"

void TestGif (void)
{
    memblock inbuf;
    inbuf.read_file ("bullet1.gif");
    istream is (inbuf);
    
    CImage img;
    is >> img;

    CImage::const_iterator ii (img.begin());
    cout << ios::hex;
    cout.format ("Read image %dx%d\n", img.Width(), img.Height());
    foreach (CPalette::const_iterator, i, img.Palette())
	cout.format ("%06X\n", *i);
    for (int y = 0; y < img.Height(); ++ y) {
	for (int x = 0; x < img.Width(); ++ x)
	    cout.format ("%02X", *ii++);
	cout << endl;
    }
    cout.flush();
}

StdBvtMain (TestGif)
