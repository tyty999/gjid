/* ychain.h
**
** 	Defines a template for a tree chain of objects. Each link is
** an instance of YChainLink which has a private pointer to actual data.
** Since it is quite inconvenient to use due to the requirement to know
** a pointer to the actual YChainLinks, it is only used as a base class
** for Tree-type things.
**
** includes:					   
**	mdefs.h		- for NULL
**	iostream.h	- for cout (error messages)
*/
	      
#ifndef __YCHAIN_H
#define __YCHAIN_H				   

#include <mdefs.h>
#include <streamable.h>

template <class YChainEl>
class YChain;

template <class DataType> 
class YChainLink {
private:
    DataType *			data;
    WORD			nParents;
    YChainLink<DataType> **	parents;
    WORD			nChildren;
    YChainLink<DataType> **	children;
    WORD			nLinks;	// n Active parental links

public:			   
    inline			YChainLink (WORD Connectivity = 2) { 
					data = NULL; 
					nParents = nChildren = Connectivity;
					parents = new (YChainLink*)[nParents];
					for (int i = 0; i < nParents; ++ i)
					   parents[i] = NULL;
					children = new (YChainLink*)[nChildren];
					for (i = 0; i < nChildren; ++ i)
					   children[i] = NULL;
					nLinks = 0;
				};
    inline void			SetData (DataType * NewData) { 
					data = NewData;
				};
    inline DataType *		GetData (void) const { 
					return (data);
				};
    inline		       ~YChainLink (void) {
      					if (parents != NULL)
					   delete parents;
      					if (children != NULL)
					   delete children;
					if (data != NULL)
					   delete data;
                                }
    friend			YChain<DataType>;
};
    
template <class YChainEl> 
class YChain {
private:
    YChainLink<YChainEl> *	root;

protected:
    inline YChainLink<YChainEl> *	MoveToRoot (void) const;
    inline YChainLink<YChainEl> *	MoveToChild (WORD which, const 
    						     YChainLink<YChainEl> * 
						     current) const;
    inline YChainLink<YChainEl> *	MoveToParent (WORD which, const 
    						      YChainLink<YChainEl> * 
						      current) const;

    inline void			Disconnect (WORD where, 
    					    YChainLink<YChainEl> * current);
    inline void			Delete (WORD where,
    					YChainLink<YChainEl> * current);
    inline void    		Connect (WORD where, WORD pslot,
    					 YChainLink<YChainEl> * current, 
    					 YChainLink<YChainEl> * newLink);
    
public:
    inline 			YChain (void);
    inline BOOL			IsEmpty (void) const;
    inline 		       ~YChain (void);
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Functions ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


template <class YChainEl> 
inline YChain<YChainEl> :: YChain (void)
{
    root = NULL;
}

template <class YChainEl>
inline BOOL YChain<YChainEl> :: IsEmpty (void) const
{
    return (root == NULL);
}

template <class YChainEl> 
inline YChainLink<YChainEl> * YChain<YChainEl> :: MoveToRoot
(void) const
{
    return (root);
}

template <class YChainEl> 
inline YChainLink<YChainEl> * YChain<YChainEl> :: MoveToChild
(WORD which, const YChainLink<YChainEl> * current) const
{
    if (current != NULL)
       return (current->children [which]);
    else {
       cerr << "NULL link has no children to go to.\n";
       exit(1);
    }
}

template <class YChainEl> 
inline YChainLink<YChainEl> * YChain<YChainEl> :: MoveToParent
(WORD which, const YChainLink<YChainEl> * current) const
{
    if (current != NULL && current->parents [which])
       return (current->parents [which]);
    else
       return (NULL);
}

template <class YChainEl> 
inline void YChain<YChainEl> :: Disconnect
(WORD where, YChainLink<YChainEl> * current)
{
    if (current == NULL) {
       cout << "YChain: Cannot disconnect nonexistant subtrees!\n";
       return;
    }		 

    if (current == root) {
       root = NULL;
    }
    else if (current->parents [where] != NULL) {
       YChainLink<YChainEl> * parent = current->parents [where];
       for (int i = 0; i < parent->nChildren; ++ i) {
	  if (parent->children[i] == current) {
	     parent->children[i] = NULL;
	     break;
	  }
       }
    }
    -- current->nLinks;
}

template <class YChainEl> 
inline void YChain<YChainEl> :: Delete
(WORD where, YChainLink<YChainEl> * current)
{
int i, p, pslot = 0;
YChainEl * scrap;

    if (current == NULL)
       return;

    for (i = 0; i < current->nChildren; ++ i) {
       if (current->children[i] != NULL) {
	  for (p = 0; p < current->children[i]->nParents; ++ p)
	     if (current->children[i]->parents[p] == current)
	        pslot = p;
	  Delete (pslot, current->children[i]);
       }
    }
	
    Disconnect (where, current);
    if (current->nLinks == 0)	// Free only if no other nodes are pointing
       delete (current);
}

template <class YChainEl> 
inline void YChain<YChainEl> :: Connect
(WORD where, WORD pslot, 
 YChainLink<YChainEl> * current, YChainLink<YChainEl> * newLink)
{
    if (current == NULL && IsEmpty()) {
       root = newLink;
       newLink->parents [pslot] = NULL;
       current = root;
    }
    else {
       if (current->children [where] != NULL) {
	  newLink->children [where] = current->children [where];
	  current->children[where]->parents [pslot] = newLink;
       }
       current->children [where] = newLink;
       newLink->parents [pslot] = current;
       ++ newLink->nLinks;
    }
}

template <class YChainEl> 
inline YChain<YChainEl> :: ~YChain 
(void)
{	
    Delete (0, root);
}      

#endif

