/* util_macros.h
**
**	Defines some widely used macros.
**
*/

#ifndef __UTIL_MACROS_H
#define __UTIL_MACROS_H

#ifndef	min
#   define	min(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef	max
#   define	max(a,b)	((a) > (b) ? (a) : (b))
#endif							 
#ifndef diff
#   define	diff(a,b)	((a) > (b) ? ((a) - (b)) : ((b) - (a)))
#endif
#ifndef randnum
#   define	randnum(range)	((range) != 0 ? random() % (range) : 0)
#endif
#ifndef sign
#   define	sign(x)		((x) < 0 ? (-1) : (1))
#endif

#define member(a,b)		("error Sorry, must use C++ to use member(a,b)")

#endif

