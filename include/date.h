/*  Copyright (C) 1993   Marc Stern  (internet: stern@mble.philips.be)   */

#ifndef __Date_H
#define __Date_H

#include <c2cpp.h>
#include <time.h>


/***
 *  Function    :   isleapyear
 *
 *  Description :   Check if given year is leap.
 *
 *  Parameters  :   in   int/unsigned   year
 *
 *  Side-effects:   Macro
 *
 *  Return      :   1 if leap
 *                  0 if not leap
 *
 *  OS/Compiler :   All
 ***/

#define isleapyear( year )      ( (! (year % 4) && (year % 100)) || (! (year % 400) && (year % 1000)))



/***
 *  Function    :   isdatevalid
 *
 *  Description :   Check if given date is valid.
 *
 *  Parameters  :   in   int   day
 *                  in   int   month
 *                  in   int   year
 *
 *  Return      :   1 if valid
 *                  0 if not
 *
 *  OS/Compiler :   All
 ***/

EXTERN int isdatevalid( int day, int month, int year );



/***
 *  Function    :   istimevalid
 *
 *  Description :   Check if given time is valid.
 *
 *  Parameters  :   in   int   hour
 *                  in   int   min
 *                  in   int   sec
 *
 *  Return      :   1 if valid
 *                  0 if not
 *
 *  Side-effects:   Macro
 *
 *  OS/Compiler :   All
 ***/

#define istimevalid( hour, min, sec ) ( ((hour) >= 0) && ((hour) < 24) && \
                                        ((min)  >= 0) && ((min)  < 60) && \
                                        ((sec)  >= 0) && ((sec)  < 60)    \
                                      )


/***
 *  Function    :   getdtime
 *
 *  Description :   Get current time and date.
 *
 *  Parameters  :   out  struct tm *dtime  pointer to time & date structure
 *
 *  Return      :   none
 *
 *  OS/Compiler :   ANSI
 ***/

EXTERN void getdtime( struct tm *dtime );



/***
 *  Function    :   diffdate
 *
 *  Description :   Compare two dates.
 *
 *  Parameters  :   out  struct tm *dtime  pointer to time & date structure
 *
 *  Decisions   :   No validity checks are made on dates;
 *		    if dates are not valid, result is unpredictable.
 *                     
 *  Return      :   Number of days between date1 and date2 (date1 - date2)
 *
 *  OS/Compiler :   ANSI
 ***/

EXTERN long diffdate( struct tm dtime1, struct tm dtime2 );



/***
 *  Function    :   day_of_week
 *
 *  Description :   Gives the day of week corresponding to a date
 *
 *  Parameters  :   in   int   day
 *                  in   int   month
 *                  in   int   year
 *
 *  Return      :    0 = sunday, 1 = monday,...
 *                  -1 if date is not valid
 *
 *  OS/Compiler :   ANSI
 ***/

EXTERN int day_of_week( int day, int month, int year );


#endif
