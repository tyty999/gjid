/* mpool.h
**
**	Defines a class for memory pool handling.
*/

#ifndef __MPOOL_H
#define __MPOOL_H

typedef WORD			MPSize;

class MemoryPool {
private:
    MPSize			BytesAllocated;
    MPSize			BytesUsed;
 
protected:

public:
    inline			MemoryPool (MPSize size);
};

#endif

