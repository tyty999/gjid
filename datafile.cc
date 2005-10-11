// datafile.cc
//
//	Implements data file routines.
//

#include <font.h>
#include "gjid.h"

void LoadData (const char* filename)
{
    memblock buf;
    buf.read_file (filename);
    istream is (buf);

    cout << "Reading palette.\n";
    // Special palette is used
    is >> pal;

    cout << "Reading font.\n";
    // Font
    is >> font;

    cout << "Reading " << size_t(NumberOfPics) << " pictures";
    // Read the pictures
    for (uoff_t i = 0; i < NumberOfPics; ++ i) {
	is >> pics[i];
	cout << ".";
    }
    cout << "\n";

    // Read the story
    cout << "Reading story.\n";
    is >> StorySize;
    story.resize (StorySize);
    is.read (story.begin(), story.size());

    // Read the levels
    is >> nLevels;
    if (nLevels == 0)
	throw runtime_error ("no levels found in data file!");
    else if (nLevels > MAX_LEVELS)
	throw runtime_error ("too many levels in the input file");
    else
	cout << "Reading " << nLevels << " levels";

    levels.resize (nLevels);
    foreach (vector<Level>::iterator, l, levels) {
	is >> *l;
	cout << ".";
    }
    cout << "\n";
}

void SaveData (const char* filename)
{
    size_t dataSize = stream_size_of(pal) + stream_size_of(font);
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	dataSize += stream_size_of (pics[i]);
    dataSize += stream_size_of (nLevels);
    foreach (vector<Level>::const_iterator, l, levels)
	dataSize += stream_size_of (*l);

    memblock buf (dataSize);
    ostream os (buf);

    os << pal;
    os << font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	os << pics[i];
    os << nLevels;
    foreach (vector<Level>::const_iterator, l, levels)
	os << *l;

    buf.write_file (filename);
}

