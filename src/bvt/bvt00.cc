#include <fbgl.h>
using namespace fbgl;

int main (void)
{
    typedef vector<CFbMode> modevec_t;
    modevec_t v;
    string mdbt, reader;
    mdbt.read_file ("bvt00.std");
    foreach (string::const_iterator, i, mdbt) {
	v.push_back();
	reader.link (i, mdbt.end());
	--(i = v.back().ReadFromModedb (reader));
    }
    if (!v.empty() && v.back().Name().empty())
	v.pop_back();
    foreach (modevec_t::const_iterator, m, v)
	cout << *m;
    return (EXIT_SUCCESS);
}

