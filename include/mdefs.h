/* mdefs.h
**
**	Miscellaneous definitions for use in other files.
** defines some useful types and macros (like min and max)
*/

#ifndef __MDEFS_H
#define __MDEFS_H

#include <stdlib.h>
#include <stddef.h>
#include <c2cpp.h>
#include <algorithm>
using namespace std;

#define NeedFunctionPrototypes		1

#ifndef TRUE
#   define TRUE			1
#   define FALSE		0
#endif				 

#define AN_ERROR 		(-1)

#ifndef BYTE
typedef	unsigned char		BYTE;
typedef unsigned long int	WORD;
#endif
#ifndef BOOL
typedef BYTE			BOOL;
#endif

typedef enum {
    Off, On
} OnOffType;

/*	In plain C define min and max as macros.
**	In C++ define them as templates for additional type checking.
*/    
#ifndef __cplusplus
#   include <util_macros.h>
#else
#   include <util_templates.h>
#endif

#endif

