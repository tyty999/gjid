// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "../gjid.h"
#include "../font.h"

class CDataBuilder {
public:
   static CDataBuilder&	Instance (void);
    void		Run (void);
private:
			CDataBuilder (void) {}
    void		LoadFromFiles (void);
    void		SaveToDat (const char* filename);
    void		RotatePixClockwise (PicIndex src, PicIndex dest);
private:
    CFont		m_Font;
    vector<Level>	m_Levels;
    picvec_t		m_Pics;
};

/*static*/ CDataBuilder& CDataBuilder::Instance (void)
{
    static CDataBuilder s_App;
    return (s_App);
}

void CDataBuilder::Run (void)
{
    LoadFromFiles();
    SaveToDat ("data/gjid.dat");
}

template <typename T>
void Load (T& v, const char* filename)
{
    static memblock buf;
    buf.read_file (filename);
    istream is (buf);
    is >> v;
}

template <typename T>
void Save (const T& v, const char* filename)
{
    static memblock buf;
    buf.resize (stream_size_of (v));
    ostream os (buf);
    os << v;
    buf.write_file (filename);
}

void CDataBuilder::LoadFromFiles (void)
{
    Load (m_Font, "data/default.fnt");
    static const char* gifs [NumberOfPics] = {
	"data/dispose.gif",
	"data/exit.gif",
	"data/floor.gif",
	"data/oneway_n.gif",
	"data/oneway_s.gif",
	"data/oneway_e.gif",
	"data/oneway_w.gif",
	"data/wall1.gif",
	"data/wall2.gif",
	"data/wall3.gif",
	"data/back1.gif",
	"data/back2.gif",
	"data/back3.gif",
	"data/robot_n.gif",
	"data/robot_s.gif",
	"data/robot_e.gif",
	"data/robot_w.gif",
	"data/barrel1.gif",
	"data/barrel2.gif",
	"data/logo_g.gif",
	"data/logo_j.gif",
	"data/logo_i.gif",
	"data/logo_d.gif"
    };
    m_Pics.resize (VectorSize(gifs));
    for (uoff_t i = 0; i < VectorSize(gifs); ++ i)
	Load (m_Pics[i], gifs[i]);
    Load (m_Levels, "data/levels.dat");
}

void CDataBuilder::SaveToDat (const char* filename)
{
    const size_t dataSize = stream_size_of(m_Font) +
		stream_size_of (m_Pics) +
		stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Font;
    os << m_Pics;
    os << m_Levels;

    buf.write_file (filename);
}

int main (void)
{
    int rv = EXIT_FAILURE;
    try {
	CDataBuilder::Instance().Run();
	rv = EXIT_SUCCESS;
    } catch (exception& e) {
	cout << "Error: " << e << endl;
	#ifndef NDEBUG
	    cout << e.backtrace();
	#endif
    } catch (...) {
	cout << "Unexpected error has occured." << endl;
    }
    return (rv);
}
