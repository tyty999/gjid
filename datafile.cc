// datafile.cc
//
//	Implements data file routines.
//

#include "font.h"
#include "gjid.h"

void LoadData (const char* filename)
{
    memblock buf;
    buf.read_file (filename);
    istream is (buf);
    is >> pal;
    is >> font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	is >> pics[i];
    is >> story;
    is.align();
    is >> levels;
    nLevels = levels.size();
}

void SaveData (const char* filename)
{
    size_t dataSize = stream_size_of(pal) + stream_size_of(font);
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	dataSize += stream_size_of (pics[i]);
    dataSize += Align (stream_size_of (story));
    dataSize += stream_size_of (levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << pal;
    os << font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	os << pics[i];
    os << story;
    os.align();
    os << levels;

    buf.write_file (filename);
}

