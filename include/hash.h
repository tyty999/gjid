/* hash.h
**
**	Defines a hash table template. Template is the attributes for
** each string. Thus the string manipulation is completely encapsulated.
*/

#ifndef __HASH_H
#define __HASH_H

#include <mdefs.h>
#include <string.h>

// This seems reasonable (covers the alphabet)
#define DEFAULT_TABLE_SIZE	50

// We are going to use the string space method, allocating
//	space for names with page quantum below:
#define STRING_SPACE_PAGE	100

typedef struct {
    WORD	Index;
    WORD	Length;
} StringSpaceEntry;

// The bucket structure is accessible only to the hash table to
//	hide the above detail.
template <class HashAttr>
class HashTable;

template <class HashAttr>
class Bucket {
private:
    StringSpaceEntry	Name;  	// This is actually an index to StringSpace 

public:
    HashAttr *		Attr;
    Bucket<HashAttr> *	Next;

    friend		HashTable<HashAttr>;
};

// Keys are generated internally, so do not assume anything
typedef WORD		HashKeyType;
    
template <class HashAttr>
class HashTable {
private:
    BYTE *		StringSpace;
    WORD		StringSpaceIndex;
    WORD		PagesAllocated;

protected:
    Bucket<HashAttr> **		Table;
    WORD		TableSize;

protected:
    inline HashKeyType	GetKey (BYTE * Name); 
    inline BOOL		IsEqual (BYTE * Name, Bucket<HashAttr> * Bucket<HashAttr>);

public:
    inline		HashTable (WORD NewTableSize = DEFAULT_TABLE_SIZE);
    inline void		Insert (BYTE * Name, HashAttr * Attr);
    inline void 	Remove (BYTE * Name);
    inline HashAttr *	Lookup (BYTE * Name);
    inline void		Clear (void);
    inline             ~HashTable (void);
};

template <class HashAttr>
inline HashKeyType HashTable<HashAttr> :: GetKey (BYTE * Name)
{
    // Key = (Name[First] + Name[Last]) % TableSize
    return ((HashKeyType)(Name[0] + Name[strlen(Name) % TableSize]));
};

template <class HashAttr>
inline BOOL HashTable<HashAttr> :: IsEqual 
(BYTE * Name, Bucket<HashAttr> * ABucket )
{
WORD NameLength, i, ssi;
    NameLength = strlen (Name);
    ssi = ABucket->Name.Index;	// String space index
    if (NameLength != ABucket->Name.Length)
       return (FALSE);
    for (i = 0; i < NameLength; ++ i) {
       if (Name[i] != StringSpace[ssi])
	  return (FALSE);
       ++ ssi;
    }
    return (TRUE);
};

template <class HashAttr>
inline HashTable<HashAttr> :: HashTable (WORD NewTableSize)
{
HashKeyType i;
    TableSize = NewTableSize;
    Table = new (Bucket<HashAttr>*) [TableSize];
    for (i = 0; i < TableSize; ++ i)
       Table[i] = NULL;
    StringSpaceIndex = 0;
    // Allocate with *alloc to use realloc to expand
    StringSpace = (BYTE*) calloc (1, STRING_SPACE_PAGE * sizeof(BYTE));
    PagesAllocated = 1;
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Insert (BYTE * Name, HashAttr * Attr)
{
HashKeyType NewKey;
Bucket<HashAttr> * NewBucket;
WORD NameLength, NewSSIndex, NeedAllocate;

    // Check if the name is already here
    if (Lookup (Name) != NULL)
       return;

    // First, setup the new bucket
    NewBucket = new Bucket<HashAttr>;
    NewBucket->Attr = Attr;
    // Enter the name in StringSpace
    NameLength = strlen (Name); 
    NewBucket->Name.Length = NameLength;
    NewBucket->Name.Index = StringSpaceIndex;
    NewSSIndex = StringSpaceIndex + NameLength;

    // Check if StringSpace needs to be enlarged and do so
    NeedAllocate = ((NewSSIndex / STRING_SPACE_PAGE) + 1) - PagesAllocated;
    if (NeedAllocate > 0) {
       StringSpace = (BYTE*) realloc (StringSpace, 
       			              (PagesAllocated + NeedAllocate) * 
				      STRING_SPACE_PAGE * sizeof(BYTE));
       PagesAllocated += NeedAllocate;
    }

    strncpy ((BYTE*)(StringSpace + StringSpaceIndex), Name, NameLength);
    StringSpaceIndex += NameLength;

    // Insert the bucket into the table
    NewKey = GetKey (Name);
    NewBucket->Next = Table [NewKey];
    Table [NewKey] = NewBucket;
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Remove (BYTE * Name)
{
HashKeyType Key;
Bucket<HashAttr> * Current, * Parent = NULL;

    Key = GetKey (Name);
    Current = Table [Key];
    while (Current != NULL && !IsEqual (Name, Current)) {
        Parent = Current;
        Current = Current->Next;
    }

    if (Current != NULL) {
       if (Parent == NULL)
	  Table [Key] = NULL;
       else
	  Parent->Next = Current->Next;
       delete Current;
    }
};

template <class HashAttr>
inline HashAttr * HashTable<HashAttr> :: Lookup (BYTE * Name)
{
HashKeyType Key;
Bucket<HashAttr> * Current;

    Key = GetKey (Name);
    Current = Table [Key];
    while (Current != NULL && !IsEqual (Name, Current))
        Current = Current->Next;

    if (Current != NULL)
       return (Current->Attr);
    else
       return (NULL);
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Clear (void)
{
HashKeyType Key;
Bucket<HashAttr> * Current;

    for (Key = 0; Key < TableSize; ++ Key) {
       Current = Table [Key];
       while (Current != NULL) {
	   Table [Key] = Current->Next;
	   delete Current;
	   Current = Table [Key];
       }
    }
};

template <class HashAttr>
inline HashTable<HashAttr> :: ~HashTable (void)
{
    Clear();
    if (Table != NULL)
       delete [] Table;
    if (StringSpace != NULL)
       free (StringSpace);
};

#endif

