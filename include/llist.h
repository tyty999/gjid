/* llist.h
**
**	Defines a template linked list. The current element is pointed
** to by one of the 10 built-in 'windows'. Window #0 is the default for
** all operations. Each window can point to a different element.
**	The location of window #0 is undefined after all function calls.
**
** This file includes chain.h	- base class
**		      mdefs.h	- for BOOL
*/

#ifndef __LLIST_H
#define __LLIST_H
 	 
#include <mdefs.h>
#include <chain.h>

// The following constant sets the number of windows available
#define  MaxWindows		10
       
// The following type is used to reference windows
typedef unsigned int		ListWin;

// The following are the iteration functions for Find* and ForEach
typedef void			(*LListIteratorFunc) (void * elem, void * param);
typedef BOOL			(*LListSearchFunc) (void * elem);

// Define the type for size of the list
typedef WORD			ListSizeType;

/*
** Here is the actual LList template
*/
template <class LListEl> 
class LList : public Chain<LListEl> {
private:
    ListSizeType		size; 
 
protected:				
    ChainLink<LListEl> **	windows;
  
public:
    inline			LList (void);
				
    virtual inline void	        Next (const ListWin wid = 0);
    virtual inline void	      	Prev (const ListWin wid = 0); 
    inline void        		Head (const ListWin wid = 0); 
    inline void	       		Tail (const ListWin wid = 0); 
    inline void			Skip (const int count, const ListWin wid = 0);
    inline BOOL	      		OnList (const ListWin wid = 0) const;
					
    inline void			InsertBefore (LListEl * data, const ListWin wid = 0);
    inline void			InsertAfter (LListEl * data, const ListWin wid = 0);
    inline LListEl *  		Remove (const ListWin wid = 0);
    inline void			Delete (const ListWin wid = 0);
    inline LListEl *		LookAt (const ListWin wid = 0) const;
    inline LListEl *		operator[] (const ListSizeType index);
    inline ListSizeType		Size (void) const;

    virtual inline LListEl *	FindFirst (LListSearchFunc f, const ListWin wid = 0);
    virtual inline LListEl *	FindLast (LListSearchFunc f, const ListWin wid = 0);
    virtual inline void		ForEach (LListIteratorFunc f, void * param = NULL);
    
    virtual inline	       ~LList (void);
};

////////////////////////////////////////////////////////////////////////////
///////////////////// Functions ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

template <class LListEl>
inline LList<LListEl> :: LList (void)
{				 		  
    windows = new ChainLink<LListEl> * [MaxWindows];
    size = 0;
}
    
template <class LListEl>
inline void LList<LListEl> :: Next 
(const ListWin wid)
{ 
    windows [wid] = MoveToNext (windows [wid]);
}

template <class LListEl>
inline void LList<LListEl> :: Prev
(const ListWin wid)
{ 
    windows [wid] = MoveToPrev (windows [wid]);
}

template <class LListEl>
inline void LList<LListEl> :: Head 
(const ListWin wid)
{ 
    windows [wid] = this->MoveToHead ();
}

template <class LListEl>
inline void LList<LListEl> :: Tail
(const ListWin wid)
{ 
    windows [wid] = this->MoveToTail ();
}

template <class LListEl>
inline void LList<LListEl> :: Skip
(const int count, const ListWin wid)
{ 
int i;
    
    if (count < 0) {
       for (i = 0; i < abs (count); ++ i) 
          Prev (wid); 
    }   
    else if (count > 0) {
       for (i = 0; i < abs (count); ++ i) 
          Next (wid); 
    }   
}

template <class LListEl>
inline BOOL LList<LListEl> :: OnList
(const ListWin wid) const
{ 
    return (windows[wid] != NULL);
}

template <class LListEl>
inline LListEl * LList<LListEl> :: LookAt
(const ListWin wid) const
{ 
    return ((windows[wid] == NULL) ? NULL : windows[wid]->GetData());
}

template <class LListEl>
inline LListEl * LList<LListEl> :: operator[]
(const ListSizeType index)
{   	 
ListSizeType i;

    if (index >= size) {
       cout << "LList: index out of range.\n";
       return (NULL);
    }
    		     
    Head (0);
    for (i = 0; i < index; ++ i)
       Next (0);
    
    return (LookAt (0));
}

template <class LListEl>
inline LListEl * LList<LListEl> :: Remove 
(const ListWin wid)
{	   
ChainLink<LListEl> * ObtainedLink;
LListEl * ObtainedData;
	       
    if (this->IsEmpty()) {
       cout << "LList: Removing an element from an empty list!\n";
       return (NULL);
    }		 
    
    Disconnect (windows [wid]);
    ObtainedLink = windows [wid];
    windows [wid] = this->MoveToHead();
    ObtainedData = ObtainedLink->GetData();
    delete ObtainedLink;
    -- size;
    return (ObtainedData);
}

template <class LListEl>
inline void LList<LListEl> :: Delete 
(const ListWin wid)
{	   
LListEl * ObtainedData;
    ObtainedData = Remove (wid);
    if (ObtainedData != NULL)
       delete ObtainedData;
}
	       
template <class LListEl>
inline void LList<LListEl> :: InsertBefore
(LListEl * data, const ListWin wid)
{
ChainLink<LListEl> * NewLink;

    NewLink = new ChainLink<LListEl>;
    NewLink->SetData (data);
    ConnectBefore (windows [wid], NewLink);
    ++ size;
}

template <class LListEl>
inline void LList<LListEl> :: InsertAfter
(LListEl * data, const ListWin wid)
{
ChainLink<LListEl> * NewLink;

    NewLink = new ChainLink<LListEl>;
    NewLink->SetData (data);
    ConnectAfter (windows [wid], NewLink);
    ++ size;
}

template <class LListEl>
inline ListSizeType LList<LListEl> :: Size 
(void) const
{
    return (size);
}

template <class LListEl>
inline LListEl * LList<LListEl> :: FindFirst
(LListSearchFunc f, ListWin wid)
{
BOOL result = FALSE;

    windows[wid] = this->MoveToHead();
    while (OnList(wid) && (result = f (LookAt (wid)) != TRUE))
       Next (wid);
       
    if (result == TRUE)
       return (LookAt (wid));
    else
       return (NULL);
}

template <class LListEl>
inline LListEl * LList<LListEl> :: FindLast
(LListSearchFunc f, ListWin wid)
{
BOOL result = FALSE;

    windows[wid] = this->MoveToTail();
    while (OnList(wid) && (result = f (LookAt (wid)) != TRUE))
       Prev (wid);
    if (result == TRUE)
       return (LookAt (wid));
    else
       return (NULL);
}

template <class LListEl>
inline void LList<LListEl> :: ForEach
(LListIteratorFunc f, void * param)
{	   
    if (this->IsEmpty())
       return;

    Head(0);		 
    while (OnList (0)) {
       f (LookAt (0), param);
       Next(0);
    }
}

template <class LListEl>
inline LList<LListEl> :: ~LList (void)
{				 		  
    delete [] windows;
}

#endif

