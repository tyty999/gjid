/* set.h
**
**	Defines a generic set template.
**
** includes:
**	mdefs.h	- for BOOL
*/

#ifndef __SET_H
#define __SET_H		  

#include <mdefs.h>

typedef unsigned long 	SetSizeType;

template <class SetEl> 
class Set {
protected:
    SetEl *	       			data;
    SetSizeType      			size;
    
public:
    inline		       		Set (SetSizeType nElem = 0); 
    inline				Set (const Set<SetEl>& ASet);
    inline				Set (const SetEl * ASet, SetSizeType nElem);
    virtual inline Set<SetEl>&		operator= (const Set<SetEl>& toBe);
    virtual inline Set<SetEl>&		operator= (const SetEl * toBe);
    virtual inline BOOL			operator== (const Set<SetEl>& toCompare) const;
    virtual inline 			operator const SetEl * () const;

    virtual inline void			Resize (SetSizeType NewSize);
    virtual inline void			Fill (SetEl value);
    virtual inline SetSizeType    	Size (void) const;
    virtual inline SetSizeType		Find (SetEl match) const;
    virtual inline 	 	       ~Set (void);
};	

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////| Set |////////////////////////////////////////

template <class SetEl>
inline Set<SetEl> :: Set (SetSizeType nElem) 
{ 	   
    size = nElem;
    if (size > 0)
       data = new SetEl [size];
    else
       data = NULL;
};

template <class SetEl>
inline Set<SetEl> :: Set 
(const Set<SetEl>& ASet)
{
SetSizeType i;
    size = ASet.size;
    if (size > 0)
       data = new SetEl [size];
    else
       data = NULL;
    
    for (i = 0; i < size; ++ i)
       data[i] = ASet.data[i];
}

template <class SetEl>
inline Set<SetEl> :: Set 
(const SetEl * ASet, SetSizeType nElem)
{
SetSizeType i;
    size = nElem;
    if (size > 0)
       data = new SetEl [size];
    else
       data = NULL;
    
    for (i = 0; i < size; ++ i)
       data[i] = ASet[i];
}

template <class SetEl>
inline Set<SetEl>& Set<SetEl> :: operator=
(const Set<SetEl>& toBe)
{
SetSizeType i;
    Resize (toBe.size);
    for (i = 0; i < toBe.size; ++ i)
       data[i] = toBe.data[i];
    return (*this);
}

template <class SetEl>
inline Set<SetEl>& Set<SetEl> :: operator=
(const SetEl * toBe)
{
SetSizeType i;
    for (i = 0; i < size; ++ i)
       data[i] = toBe[i];
    return (*this);
}

template <class SetEl>
inline BOOL Set<SetEl> :: operator==
(const Set<SetEl>& toCompare) const
{
SetSizeType i;

    if (size != toCompare.size)
       return (FALSE);
    for (i = 0; i < size; ++ i)
       if (!(data[i] == toCompare.data[i]))	// to require only == in data
          return (FALSE);
    return (TRUE);    
}

template <class SetEl>
inline Set<SetEl> :: operator const SetEl * () const
{ 
    return (data);
};

template <class SetEl>
inline void Set<SetEl> :: Fill (SetEl value)
{
SetSizeType i;
    for (i = 0; i < size; ++ i)
       data[i] = value;
}

template <class SetEl>
inline SetSizeType Set<SetEl> :: Size (void) const
{ 					  
    return (size);
};

template <class SetEl>
inline void Set<SetEl> :: Resize (SetSizeType NewSize)
{ 					  
SetEl * NewSet = NULL;
SetSizeType i;
		 
    if (NewSize > 0) {
       NewSet = new SetEl [NewSize]; 
       for (i = 0; i < min (NewSize, size); ++ i)
          NewSet [i] = data [i];
    }
    if (size > 0)
       delete [] data;
    data = NewSet;
    size = NewSize;
};

template <class SetEl>
inline SetSizeType Set<SetEl> :: Find (SetEl match) const
{			  
SetSizeType i;
long int index = AN_ERROR;

    for (i = 0; i < size; ++ i) {
       if (data[i] == match) {
          index = i;	      
	  break;
       }
    }
    return (index);
};

template <class SetEl>
inline Set<SetEl> :: ~Set (void)
{	 
    if (size > 0)
       delete [] data;
};

////////////////////////////////////////////////////////////////////////////////

#endif

