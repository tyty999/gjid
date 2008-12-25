#include "stdtest.h"

void TestStringTable (void)
{
    static const char sep[] = "\n%\n";
    string src, v;
    src.read_file ("bvt03.std");

    CStringTable strt;
    foreach (string::const_iterator, i, src) {
	string::const_iterator iend = src.iat (src.find (sep, i - src.begin()));
	v.link (i, iend);
	strt.push_back (v);
	i = min (iend + VectorSize(sep) - 2, src.end() - 1);
    }

    string dest (stream_size_of (strt));
    ostream os (dest);
    os << strt;

    strt.clear();
    istream is (dest);
    is >> strt;

    if (!strt.empty())
	cout << strt[0];
    for (uoff_t i = 1; i < strt.size(); ++ i)
	cout << sep << strt[i];
}

StdBvtMain (TestStringTable)
