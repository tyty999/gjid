/* mdefs.h
**
**	Miscellaneous definitions for use in other files.
** defines some useful types and macros (like min and max)
*/

#ifndef __MDEFS_H
#define __MDEFS_H

#include <stdlib.h>
#include <stddef.h>
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
typedef bool			BOOL;
#endif

typedef enum {
    Off, On
} OnOffType;

#include <util_templates.h>

#endif

