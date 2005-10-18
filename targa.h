// targa.h
//
// TARGA format information
//

#ifndef TARGA_H_598431C414F8965F48645DF62A61BF90
#define TARGA_H_598431C414F8965F48645DF62A61BF90

#include "icon.h"

/// \class CTargaHeader targa.h "targa.h"
///
/// \brief Header structure for the TARGA graphics format.
///
class CTargaHeader {
public:
    enum EImageType {
	timg_None,
	timg_CMapUncompressed,
	timg_RGBUncompressed,
	timg_MonoUncompressed,
	timg_CMapRLE = 9,
	timg_RGBRLE,
	timg_MonoRLE,
	timg_CMapHuffman = 32,
	timg_CMapHuffman4Pass
    };
    enum EFlags {
	f_Attr0,
	f_Attr1,
	f_Attr2,
	f_Attr3,
	f_RightOrigin,
	f_UpperOrigin,
	f_2WayInterleave,
	f_4WayInterleave,
	f_Last
    };
public:
			CTargaHeader (void);
    void		read (istream& is);
    void		write (ostream& os) const;
    size_t		stream_size (void) const;
    void		SetPalette (const CPalette& pal);
    void		SetImage (const Icon& img);
private:
    uint8_t		m_ImgIDLen;
    uint8_t		m_bHasCMap;
    uint8_t		m_Type;
    uint8_t		m_CMapBits;
    uint16_t		m_CMapFirst;
    uint16_t		m_CMapLength;
    uint16_t		m_XOrigin;
    uint16_t		m_YOrigin;
    uint16_t		m_Width;
    uint16_t		m_Height;
    uint8_t		m_Depth;
    uint8_t		m_Flags;	///< See EFlags for bit values.
private:
    static const char	s_IDField[6];
};

STD_STREAMABLE (CTargaHeader)

#endif

