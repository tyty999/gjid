#include "stdtest.h"

void TestGif (void)
{
    memblock inbuf;
    inbuf.read_file ("bvt/bullet1.gif");
    istream is (inbuf);
    
    CImage img;
    is >> img;

    memblock outbuf (stream_size_of (img));
    try {
	ostream os (outbuf);
	os << img;
	istream ins (outbuf);
	ins >> img;
    } catch (...) {
	outbuf.write_file ("bvt/bvt02.gif");
	throw;
    }

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
