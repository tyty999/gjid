#include "gjid.h"
#include "font.h"

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
    Font		m_Font;
    vector<Level>	m_Levels;
    CStringTable	m_Strings;
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
	"data/oneway.gif",
	"data/oneway.gif",
	"data/oneway.gif",
	"data/oneway.gif",
	"data/wall1.gif",
	"data/wall2.gif",
	"data/wall3.gif",
	"data/back1.gif",
	"data/back2.gif",
	"data/back3.gif",
	"data/robot.gif",
	"data/robot.gif",
	"data/robot.gif",
	"data/robot.gif",
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

    RotatePixClockwise (OWDNorthPix, OWDEastPix);
    RotatePixClockwise (OWDEastPix, OWDSouthPix);
    RotatePixClockwise (OWDSouthPix, OWDWestPix);
    RotatePixClockwise (RobotNorthPix, RobotEastPix);
    RotatePixClockwise (RobotEastPix, RobotSouthPix);
    RotatePixClockwise (RobotSouthPix, RobotWestPix);

    Load (m_Levels, "data/levels.dat");
    Load (m_Strings, "data/strings.strt");
}

void CDataBuilder::RotatePixClockwise (PicIndex src, PicIndex dest)
{
    m_Pics[dest].SetImage (m_Pics[src].Height(), m_Pics[src].Width(), m_Pics[src].begin());
    for (dim_t y = 0; y < m_Pics[src].Height(); ++ y)
	for (dim_t x = 0; x < m_Pics[src].Width(); ++ x)
	    m_Pics[dest].SetPixel (m_Pics[src].Height() - (y + 1), x, m_Pics[src].GetPixel (x, y));
}

void CDataBuilder::SaveToDat (const char* filename)
{
    const size_t dataSize = stream_size_of(m_Font) +
		stream_size_of (m_Pics) +
		stream_size_of (m_Strings) +
		stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Font;
    os << m_Pics;
    os << m_Strings;
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

