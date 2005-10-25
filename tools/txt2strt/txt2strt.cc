#include <fbgl.h>
using namespace fbgl;

void ConvertFile (const string& filename)
{
    static const char sep[] = "\n%\n";
    string src, v;
    src.read_file (filename);

    CStringTable strt;
    foreach (string::const_iterator, i, src) {
	string::const_iterator iend = src.find (sep, i);
	v.link (i, iend);
	strt.push_back (v);
	i = min (iend + VectorSize(sep) - 2, src.end() - 1);
    }

    uoff_t idot = distance (filename.begin(), filename.rfind ("."));
    string strtFile, hFile;
    strtFile.assign (filename.begin(), filename.begin() + idot);
    hFile = strtFile;
    strtFile += ".strt";
    hFile += ".h";

    memblock buf (stream_size_of (strt));
    ostream os (buf);
    os << strt;
    buf.write_file (strtFile);
}

int main (int argc, char** argv)
{
    int rv = EXIT_FAILURE;
    if (argc != 2) {
	cout.format ("Usage: %s strings.txt\n", argv[0]);
	return (rv);
    }
    try {
	ConvertFile (argv[1]);
	rv = EXIT_SUCCESS;
    } catch (exception& e) {
	cout << "Error: " << e << endl;
    } catch (...) {
	cout << "Unexpected fatal error has occurred.\n";
    }
    return (rv);
}

