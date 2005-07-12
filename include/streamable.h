// streamable.h
//

#ifndef STREAMABLE_H_46485D7840AF7A70129D1687392DDAC5
#define STREAMABLE_H_46485D7840AF7A70129D1687392DDAC5

#include <iostream>
#include <fstream>
using namespace std;

/// \brief Defines a base class for classes that use iostream.
/// IO to file and std[in,out]. Overloaded << and >> for the whole family.
///
class Streamable {
public:
    inline virtual     ~Streamable (void) {}
    inline void		Load (const char * filename)	{ ifstream is (filename); Read (is); }
    inline void		Save (const char * filename)	{ ofstream os (filename); Write (os); }
    inline virtual void Read (istream&)		{}
    inline virtual void Write (ostream&)	{}
    inline virtual void	Read (ifstream&)	{}
    inline virtual void	Write (ofstream&)	{}
};

//----------------------------------------------------------------------

inline istream& operator>> (istream& is, Streamable& obj)
{
    obj.Read (is);
    return (is);
}

inline ostream& operator<< (ostream& is, Streamable& obj)
{
    obj.Write (is);
    return (is);
}

//----------------------------------------------------------------------

#endif

