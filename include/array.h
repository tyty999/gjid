/* array.h
**
**	Defines a generic array template. An array is basically an
** indexed set. The only function added is the [] operator.
**
** includes:
**	set.h		- base class
**	streamable.h	- base class
*/

#ifndef __ARRAY_H
#define __ARRAY_H
				    
#include <set.h>
#include <streamable.h>				

typedef SetSizeType 		ArraySizeType;

template <class ArrayEl> 
class Array : public Set<ArrayEl>, public Streamable {
public:
    inline	       			Array (ArraySizeType nElem = 0);
    inline				Array (const Array<ArrayEl>& AnArray);
    inline				Array (const ArrayEl * AnArray, ArraySizeType nElem);
    virtual inline ArrayEl&		operator[] (ArraySizeType index) const;
    virtual inline void			Read (ifstream& is);
    virtual inline void			Write (ofstream& os) const;
};	

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////| Array |////////////////////////////////////////

template <class ArrayEl>
inline Array<ArrayEl> :: Array 
(ArraySizeType nElem) : Set<ArrayEl> (nElem)
{ 	   
};

template <class ArrayEl>
inline Array<ArrayEl> :: Array
(const Array<ArrayEl>& AnArray) : Set<ArrayEl> (AnArray)
{ 	   
};

template <class ArrayEl>
inline Array<ArrayEl> :: Array 
(const ArrayEl * AnArray, ArraySizeType nElem) : Set<ArrayEl> (AnArray, nElem)
{ 	   
};

template <class ArrayEl>
inline ArrayEl& Array<ArrayEl> :: operator[] (ArraySizeType index) const
{ 
    return (data [index]);
};
	       
template <class ArrayEl>
inline void Array<ArrayEl> :: Read (ifstream& is)
{	     
ArraySizeType SavedSize, i;

    is.read (&SavedSize, sizeof(ArraySizeType));
    Resize (SavedSize);
    for (i = 0; i < size; ++ i)
       is.read (&data[i], sizeof(ArrayEl));
};

template <class ArrayEl>
inline void Array<ArrayEl> :: Write (ofstream& os) const
{
ArraySizeType i;

    os.write (&size, sizeof(ArraySizeType));
    for (i = 0; i < size; ++ i)
       os.write (&data[i], sizeof(ArrayEl));
};

////////////////////////////////////////////////////////////////////////////////

#endif

