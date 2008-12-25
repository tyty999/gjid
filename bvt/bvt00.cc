#include "stdtest.h"
#include <fbgl/con/mode.h>

void TestModeLoading (void)
{
    typedef vector<CMode> modevec_t;
    modevec_t v;
    string mdbt, reader;
    mdbt.read_file ("bvt/bvt00.std");
    CConsoleMode nm;
    foreach (string::const_iterator, i, mdbt) {
	reader.link (i, mdbt.end());
	--(i = nm.ReadFromModedb (reader));
	v.push_back (nm);
    }
    if (!v.empty() && v.back().Name().empty())
	v.pop_back();
    foreach (modevec_t::const_iterator, m, v)
	cout << *m;
}

StdBvtMain (TestModeLoading)
