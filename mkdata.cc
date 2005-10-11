#include "gjid.h"
#include "font.h"

picvec_t		pics;

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
private:
    CPalette		m_Palette;
    Font		m_Font;
    vector<Level>	m_Levels;
    string		m_Story;
};

/*static*/ CDataBuilder& CDataBuilder::Instance (void)
{
    static CDataBuilder s_App;
    return (s_App);
}

void CDataBuilder::Run (void)
{
    LoadFromFiles();
    SaveToDat ("gjid.dat");
}

template <typename T>
void Load (T& v, const char* filename)
{
    static memblock buf;
    buf.read_file (filename);
    istream is (buf);
    is >> v;
}

void CDataBuilder::LoadFromFiles (void)
{
    Load (m_Palette, "data/rgb.pal");
    Load (m_Font, "data/default.fnt");
    static const char* pix [NumberOfPics] = {
	"data/dispose.pix",
	"data/exit.pix",
	"data/floor.pix",
	"data/oneway_n.pix",
	"data/oneway_s.pix",
	"data/oneway_e.pix",
	"data/oneway_w.pix",
	"data/wall1.pix",
	"data/wall2.pix",
	"data/wall3.pix",
	"data/back1.pix",
	"data/back2.pix",
	"data/back3.pix",
	"data/robot_n.pix",
	"data/robot_s.pix",
	"data/robot_e.pix",
	"data/robot_w.pix",
	"data/barrel1.pix",
	"data/barrel2.pix",
	"data/logo_g.pix",
	"data/logo_j.pix",
	"data/logo_i.pix",
	"data/logo_d.pix"
    };
    for (uoff_t i = 0; i < VectorSize(pix); ++ i)
	Load (pics[i], pix[i]);
    Load (m_Levels, "gjid.levels");
    m_Story.read_file ("gjid.story");
}

void CDataBuilder::SaveToDat (const char* filename)
{
    size_t dataSize = stream_size_of(m_Palette) + stream_size_of(m_Font);
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	dataSize += stream_size_of (pics[i]);
    dataSize += Align (stream_size_of (m_Story));
    dataSize += stream_size_of (m_Levels);

    memblock buf (dataSize);
    ostream os (buf);

    os << m_Palette;
    os << m_Font;
    for (uoff_t i = 0; i < NumberOfPics; ++ i)
	os << pics[i];
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

