/* chain.h
**
** 	Defines a template for a chain of objects. Each link is
** an instance of ChainLink which has a private pointer to actual data.
** Since it is quite inconvenient to use due to the requirement to know
** a pointer to the actual ChainLinks, it is only used as a base class
** for Linked List type things.
**
** includes:					   
**	mdefs.h		- for NULL
**	iostream.h	- for cout (error messages)
*/
	      
#ifndef __CHAIN_H
#define __CHAIN_H				   

#include <mdefs.h>
#include <streamable.h>

template <class ChainEl>
class Chain;

template <class DataType> 
class ChainLink {
private:
    DataType *			data;
    ChainLink<DataType> *	next;
    ChainLink<DataType> *	prev;

public:			   
    inline			ChainLink (void) { 
					data = NULL; 
					next = prev = NULL;
				};
    inline void			SetData (DataType * NewData) { 
					data = NewData;
				};
    inline DataType *		GetData (void) const { 
					return (data);
				};

    friend class		Chain<DataType>;
};
    
template <class ChainEl> 
class Chain {
private:
    ChainLink<ChainEl> *	head;
    ChainLink<ChainEl> *	tail;

protected:
    inline ChainLink<ChainEl> *		MoveToHead (void) const;
    inline ChainLink<ChainEl> *		MoveToTail (void) const;
    inline ChainLink<ChainEl> *		MoveToNext (const ChainLink<ChainEl> * current) const;
    inline ChainLink<ChainEl> *		MoveToPrev (const ChainLink<ChainEl> * current) const;

    inline void		Disconnect (ChainLink<ChainEl>  * current);
    inline void    	ConnectAfter (ChainLink<ChainEl>  * current, ChainLink<ChainEl>  * newLink);
    inline void    	ConnectBefore (ChainLink<ChainEl>  * current, ChainLink<ChainEl>  * newLink);
    
public:
    inline 			Chain (void);
    inline BOOL			IsEmpty (void) const;
    virtual inline 	       ~Chain (void);
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Functions ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


template <class ChainEl> 
inline Chain<ChainEl> :: Chain (void)
{
    head = tail = NULL;
}

template <class ChainEl>
inline BOOL Chain<ChainEl> :: IsEmpty (void) const
{
    if (head == NULL && tail == NULL)
       return (TRUE);
    else if ((head == NULL || tail == NULL) && head != tail)
       cout << "Chain: empty, but head != tail";
    return (FALSE);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToHead
(void) const
{
    return (head);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToTail
(void) const
{
    return (tail);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToNext
(const ChainLink<ChainEl> * current) const
{
    return (current->next);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToPrev
(const ChainLink<ChainEl> * current) const
{
    return (current->next);
}

template <class ChainEl> 
inline void Chain<ChainEl> :: Disconnect
(ChainLink<ChainEl> * current)
{
    if (current == NULL) {
       cout << "Chain: Cannot disconnect nonexistant links!\n";
       return;
    }		 

    if (current == head)
       head = current->next;
    else
       current->prev->next = current->next; 
       
    if (current == tail)
       tail = current->prev;
    else
       current->next->prev = current->prev;
    
    current->next = NULL;
    current->prev = NULL;

    // Check for the memory error that messes up pointers
    if ((head == NULL || tail == NULL) && (head != tail)) {
       cout << "Chain: head != tail in empty list!\n";
       exit(1);
    }
}

template <class ChainEl> 
inline void Chain<ChainEl> :: ConnectAfter
(ChainLink<ChainEl> * current, ChainLink<ChainEl> * newLink)
{
    if (current == NULL) {
       if (IsEmpty()) {
          head = newLink;
          tail = newLink;
       }
       else {
          cout << "ERROR: Cannot insert after nonexistant links!\n";
          return;
       }
    }
    else {
       newLink->next = current->next;
       newLink->prev = current;

       if (current == tail)
          tail = newLink;
       else		       
          current->next->prev = newLink;
       current->next = newLink;
    }
}

template <class ChainEl>
inline void Chain<ChainEl> :: ConnectBefore
(ChainLink<ChainEl> * current, ChainLink<ChainEl> * newLink)
{
    if (current == NULL) {
       if (IsEmpty()) {
          head = newLink;
          tail = newLink;
       }
       else {
          cout << "ERROR: Cannot insert before nonexistant links!\n";
          return;
       }
    }
    else {
       newLink->next = current;
       newLink->prev = current->prev;

       if (current == head)
          head = newLink;
       else		       
          current->prev->next = newLink;
       current->prev = newLink;
    }
}

template <class ChainEl> 
inline Chain<ChainEl> :: ~Chain 
(void)
{	
ChainLink<ChainEl> * scrap;
ChainEl * data;
					    
    while (!IsEmpty()) {			     
        scrap = MoveToHead();
        Disconnect (scrap);
	if ((data = scrap->GetData()) != NULL)
	   delete (data);
	delete scrap;
    }		    
}      

#endif

