/* streamable.h
**
**	Defines a base class for classes that use iostream.
** IO to file and std[in,out]. Overloaded << and >> for the whole family.
**
** Warning: do not include <unistd,h>, <stdio.h> or any other C output headers.
**		They contain conflicts with <iostream.h>
**
** includes:
**	iostream.h	- for <<, >> and streams
**	fstream.h	- for ifstream and ofstream
*/

#ifndef __STREAMABLE_H
#define __STREAMABLE_H		 

#include <iostream.h>
#include <fstream.h>

class Streamable {
public:
    inline void			Load (char * filename);
    inline void			Save (char * filename);

    virtual inline void 	Read (istream& is) {};
    virtual inline void 	Write (ostream& os) {};

    virtual inline void 	Read (ifstream& is) {};
    virtual inline void 	Write (ofstream& os) {};
};
		   
inline void Streamable :: Load (char * filename)
{
ifstream is (filename);
    Read (is);
}

inline void Streamable :: Save (char * filename)
{
ofstream os (filename);
    Write (os);
}

///////////////////////////////////////////////////////////////////////////
			      
inline istream& operator>> (istream& is, Streamable& obj)
{
    obj.Read (is);
    return (is);
};

inline ostream& operator<< (ostream& is, Streamable& obj)
{
    obj.Write (is);
    return (is);
};

///////////////////////////////////////////////////////////////////////////

#endif

