/****
 *  Copyright (C) 1993   Marc Stern  (internet: stern@mble.philips.be)
 *
 *   C2CPP.H :      general function declaration
 *                  for both C & C++ 
 *
 *   Important Remark:  This file must be included before any other one.
 *
 ***/



#ifndef EXTERN

# if defined(__cplusplus) || defined(__cplusplus__) || defined(_cplusplus)
#  define EXTERN   extern "C"
# else
#  define EXTERN   extern 
# endif
                 
# if defined(__BORLANDC__)
# define __MSC   /* for full compatibility with Microsoft C  */
# endif                                                       

#endif
