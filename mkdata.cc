#include "gjid.h"
#include "font.h"

class CDataBuilder {
public:
   static CDataBuilder&	Instance (void);
    void		Run (void);
private:
    typedef tuple<NumberOfPics,Icon>	picvec_t;
private:
			CDataBuilder (void) {}
    void		LoadFromFiles (void);
    void		SaveToDat (const char* filename);
    void		RotatePixClockwise (PicIndex src, PicIndex dest);
private:
    CPalette		m_Palette;
    Font		m_Font;
    vector<Level>	m_Levels;
    string		m_Story;
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
    Load (m_Palette, "data/rgb.pal");
    Load (m_Font, "data/default.fnt");
    static const char* pix [NumberOfPics] = {
	"data/dispose.pix",
	"data/exit.pix",
	"data/floor.pix",
	"data/oneway.pix",
	"data/oneway.pix",
	"data/oneway.pix",
	"data/oneway.pix",
	"data/wall1.pix",
	"data/wall2.pix",
	"data/wall3.pix",
	"data/back1.pix",
	"data/back2.pix",
	"data/back3.pix",
	"data/robot.pix",
	"data/robot.pix",
	"data/robot.pix",
	"data/robot.pix",
	"data/barrel1.pix",
	"data/barrel2.pix",
	"data/logo_g.pix",
	"data/logo_j.pix",
	"data/logo_i.pix",
	"data/logo_d.pix"
    };
    for (uoff_t i = 0; i < VectorSize(pix); ++ i)
	Load (m_Pics[i], pix[i]);

    RotatePixClockwise (OWDNorthPix, OWDEastPix);
    RotatePixClockwise (OWDEastPix, OWDSouthPix);
    RotatePixClockwise (OWDSouthPix, OWDWestPix);
    RotatePixClockwise (RobotNorthPix, RobotEastPix);
    RotatePixClockwise (RobotEastPix, RobotSouthPix);
    RotatePixClockwise (RobotSouthPix, RobotWestPix);

    Load (m_Levels, "data/levels.dat");
    m_Story.read_file ("data/story.txt");
}

void CDataBuilder::RotatePixClockwise (PicIndex src, PicIndex dest)
{
    m_Pics[dest].SetImage (m_Pics[src].Height(), m_Pics[src].Width(), m_Pics[src].GetRow(0));
    for (dim_t y = 0; y < m_Pics[src].Height(); ++ y)
	for (dim_t x = 0; x < m_Pics[src].Width(); ++ x)
	    m_Pics[dest].SetPixel (m_Pics[src].Height() - (y + 1), x, m_Pics[src].GetPixel (x, y));
}

void CDataBuilder::SaveToDat (const char* filename)
{
    const size_t dataSize = stream_size_of(m_Palette) + stream_size_of(m_Font) +
		stream_size_of (m_Pics) + Align (stream_size_of (m_Story)) +
		stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Palette;
    os << m_Font;
    os << m_Pics;
    os << m_Story;
    os.align();
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
    } catch (...) {
	cout << "Unexpected error has occured." << endl;
    }
    return (rv);
}

