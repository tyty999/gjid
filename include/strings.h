/*
 *  Copyright (C) 1993   Marc Stern  (internet: stern@mble.philips.be)
 *
 * File         : strings.h
 *
 * Description  : strings management
 *
 */                       


#ifndef __Strings_H
#define __Strings_H


                /*  INCLUDE FILES  */

#include <c2cpp.h>
#include <string.h>
#include <stddef.h>



/***
 *  Function    :   foreach
 *
 *  Description :   Loop on each token of a string
 *
 *  Parameters  :   out    char *word    pointer that will contain word (token)
 *                  in/out char *string  string to tokenize
 *
 *  Side-effects:   Macro, so be careful.
 *                  '\0' will be inserted into string.
 *
 *  Return code :   none
 ***/

#define foreach( word , string ) \
	for ( word = strtok(string," ") ; word ; word = strtok(NULL," ") )




/***
 *  Function    :   STRMIN/STRMAX
 *
 *  Description :   return min/max of two strings
 *
 *  Parameters  :   in   char *str1
 *                  in   char *str2
 *
 *  Side-effects:   Macro, so be careful.
 *
 *  Return      :   pointer to min/max of the two strings
 ***/

#define STRMIN( str1 , str2 )		( (strcmp(str1 , str2) < 0) ? str1 : str2 )
#define STRMAX( str1 , str2 )		( (strcmp(str1 , str2) > 0) ? str1 : str2   )

/***
 *  Function    :   strisequal
 *
 *  Description :   Test equality of two strings
 *
 *  Parameters  :   in   char *str1
 *                  in   char *str2
 *
 *  Return      :   0 or 1
 ***/

#define strisequal( string1 , string2 )	( ! strcmp(string1 , string2) )


    /***   For switch/case-like string manipulation

               usage:   STRWITCH( string )
                                {
                                  STRCASE "string1": ... ;
                                  STRCASE "string2": ... ; break ;
                                  ...
                                  STRDEFAULT    : ... ;
                                } STRENDSWITCH
     ***/

#define STRSWITCH( _str )     { char *_tmp_s = _str ; \
                                int _tmp_flag = 0 ; \
                                do { if ( _tmp_flag )

#define STRCASE( _str )       ( _tmp_flag = 1 ) ; } \
                              if ( _tmp_flag || ! strcmp(_tmp_s, _str) ) { 0 ? 0

#define STRDEFAULT            0 ; } { 0 ? 0

#define STRENDSWITCH          } while (0) ; }




	 	/*  TYPES DEFINITIONS  */


typedef enum { align_left , align_right , align_center } align_type ;

typedef enum { LOWER = 5 , UPPER } casetype ;



		/*  FUNCTIONS DEFINITIONS  */


/***
 *  Function    :  strleft
 *
 *  Description :  Copy the first ... characters of a string.
 *		   Like strncpy but add a '\0' at the end of the output string.
 *
 *  Decisions   :  If given length > string length : normal strcpy
 *		   If given length <= 0 returns an empty string.
 *
 *  Parameters  :  out  char  *out_str     result
 *                 in   char  *in_str      in string
 *                 in   int   length       length to be copied
 *
 *  Return code :   pointer to result.
 *
 *  OS/Compiler :   All
 */

EXTERN char *strleft( char* , const char* , int ) ;




/***
 *  Function    :  strright
 *
 *  Description :  Copy the last ... characters of a string.
 *
 *  Decisions   :  If given length > string length : normal strcpy
 *		   If given length <= 0 returns an empty string.
 *
 *  Parameters  :  out  char  *out_str     result
 *                 in   char  *in_str      in string
 *                 in   int   length       length to be copied
 *
 *  Return code :   pointer to result.
 *
 *  OS/Compiler :   All
 */

EXTERN char *strright( char* , const char* , int ) ;



/***
 *  Function    :  strmid
 *
 *  Description :  Copy n characters of a string, begining at a given position
 *		   ( form 1 to ... )
 *
 *  Decisions   :  Stops at the end of input string if given length
 *		   is too big or length = 0.
 *		   If given length or position < 0 returns an empty string.
 *
 *  Parameters  :  out  char  *out_str     result
 *                 in   char  *in_str      in string
 *                 in   int   pos          position where begin to copy
 *                 in   int   length       length to be copied
 *
 *  Return code :   pointer to result.
 *
 *  OS/Compiler :   All
 */

EXTERN char *strmid( char* , const char* , int , int ) ;



/***
 *  Function    :  stralign
 *
 *  Description :  Copy an input string in an output string
 *		   with specified alignement (blank padding).
 *
 *  Decisions   :  If given length < 0 returns an empty string.
 *
 *  Parameters  :  out  char        *out_str     result
 *                 in   char        *in_str      in string
 *                 in   align_type  casetype     type
 *                 in   int         length       length to be copied
 *
 *  Value       :  type = { align_left, align_center, align_right }
 *
 *  Return code :   pointer to result.
 *
 *  OS/Compiler :   All
 */

EXTERN char *stralign( char* , const char* , align_type , int ) ;



/***
 *  Function    :  strmvstr
 *
 *  Description :  Copy an input string in an output string
 *		   with replacing all occurences of a target string.
 *
 *  Parameters  :  out      char  *out_str    result
 *                 in       char  *in_str     in string
 *                 in       char  *target     target string to replace
 *                 in       char  *new_str    string to put in place of target
 *
 *  Return code :  pointer to result.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strmvstr( char* , const char* , const char* , const char* ) ;



/***
 *  Function    :  strmvchr
 *
 *  Description :  Replace all occurences of a target character
 *		   by a new character.
 *
 *  Parameters  :  in/out   char  *string
 *                 in       char  target     target char to replace
 *                 in       char  new_char   char to put in place of target
 *
 *  Return code :  pointer to result.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strmvchr( char* , char , char ) ;
                     


/***
 *  Function    :  strrmstr
 *
 *  Description :  Removing all occurences of a target string.
 *
 *  Parameters  :  in/out   char  *string
 *                 in       char  *target     target string to remove
 *
 *  Decisions   :  Same implementation as strmvstr without copying
 *		   a replacement string.
 *		   Could also be implemented as
 *                        strmvstr( ptr, ptr, target, "" )
 *		   but should be less efficient.
 *
 *  Return code :  pointer to result.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strrmstr( char* , const char* ) ;



/***
 *  Function    :  strrmchr
 *
 *  Description :  Removing all occurences of a target character.
 *
 *  Parameters  :  in/out   char  *string
 *                 in       char  target     target char to remove
 *
 *  Return code :  pointer to result.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strrmchr( char* , char ) ;



/***
 *  Function    :  strinsert
 *
 *  Description :  Insert a string in another.
 *
 *  Parameters  :  out  char   *out_str    out string
 *                 in   char   *in_str     in string
 *                 in   char   *to_insert  in string to insert into the other
 *                 in   int    place       place to insert string
 *
 *  Decisions   :  Does nothing if place specified out of range.
 *
 *  Return      :  pointer to result
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strinsert( char* , const char* , const char* , int ) ;



/***
 *  Function    :  strend
 *
 *  Description :  Returns a pointer to the '\0' ending a string
 *
 *  Parameters  :  in   char        *in_str      in string
 *
 *  Return code :   pointer to the '\0' ending the string..
 *
 *  OS/Compiler :   All
 ***/
    
EXTERN char *strend( const char* ) ;



/***
 *  Function    :   strskip
 *
 *  Description :   Skip 'n' words from a string.
 *
 *  Decisions   :
 *
 *  Parameters  :   in    char    *string     string to be matched
 *                  in    int     word_nb     number of words to skip
 *
 *  Return code :   pointer to word.
 *
 *  OS/Compiler :   All
 ***/

EXTERN char *strskip( const char* , int ) ;



/***
 *  Function    :  strisalpha
 *
 *  Description :  Tests if a string contains only alphabetical character
 *
 *  Parameters  :  in   char   *string
 *
 *  Return      :  1 if string contains only alphabetical characters.
 *                 0 otherwise
 *
 *  OS/Compiler :  All
 ***/

EXTERN int strisalpha( const char* ) ;



/***
 *  Function    :  strisalnum
 *
 *  Description :  Tests if a string contains only alphanumerical character
 *
 *  Parameters  :  in   char   *string
 *
 *  Return      :  1 if string contains only alphanumerical characters.
 *                 0 otherwise
 *
 *  OS/Compiler :  All
 ***/

EXTERN int strisalnum( char* ) ;



/***
 *  Function    :  stris_alnum
 *
 *  Description :  Tests if a string contains only alphanumerical characters or '_'.
 *
 *  Parameters  :  in   char   *string
 *
 *  Return      :  1 if string contains only alphanumerical characters or '_'.
 *                 0 otherwise
 *
 *  OS/Compiler :  All
 ***/

EXTERN int stris_alnum( const char* ) ;



/***
 *  Function    :   strelt
 *
 *  Description :   Return the n-th element (token) in a string.
 *		    The tokens are separated by several input character.
 *
 *  Decisions   :   If tokens are separated by ' ', '\t' or '\n',
 *		    the input characters will be " ".
 *		    If n-th token does not exist, an empty string is returned.
 *
 *  Parameters  :   out    char  *out_str      out string
 *		    in     char  *in_str       in string
 *		    in     char  *token        string containing token
 *		    in     int   element       element number
 *
 *  Return      :   pointer to the n-th token.
 *
 *  OS/Compiler :   All
 */

EXTERN char *strelt( char* , const char* , char* , int ) ;



/***
 *  Function    :  strreduce
 *
 *
 *  Description :  Parse a string to suppress blanks, tabs and newlines
 *                 and transform it in uppercase.
 *
 *
 *  Decisions   :  - Portions of string between double quotes are unmodified
 *                   (quotes are removed).
 *
 *                 - '\' is used to quote the next character.
 *
 *                 - Characters with accent are also transformed to uppercase.
 *
 *  Parameters  :  in/out   char *string
 *
 *  Return      :  pointer to end of string ( '\0' ).
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strreduce( char* ) ;



/***
 *  Function    :   strtoken
 *
 *  Description :   Parse a string to extract the next token.
 *
 *  Decisions   :   The tokens are separated by ' ', '\t' or '\n'.
 *	            Tokens may contain these separators by being quoted with
 *		    one of the following characters : '"`.
 *		    The function returns a pointer to the character following the token.
 *                  Special escape sequences accepted: 
 *                     \b, \f, \n, \r, \t, \e (Esc), \x..., \0..., \...
 *
 *  Parameters  :   in    char *string    string to be tokenized
 *		    out   char *token     string containing token
 *
 *  Return code :   pointer to character following the token.
 *
 *  OS/Compiler :   All
 ***/

EXTERN char *strtoken( const char* , char* ) ;



/***
 *  Function    :  strcase
 *
 *  Description :  change case of a string.
 *   		   All special characters are translated (‚Š…...)
 *
 *  Parameters  :  in   char      *string  string to translate
 *                 in   casetype  type     UPPER/LOWER
 *                                        
 *  Value       :  type = { UPPER, LOWER }
 *
 *  Decisions   :  If character > 255, no change made.
 *
 *  Return      :  pointer to result
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strcase( char* , casetype ) ;



/***
 *  Function    :  chcase
 *
 *  Description :  change case of a 2-byte character.
 *   		   All special characters are translated (‚Š…...)
 *
 *  Parameters  :  in   char      car      char to translate
 *                 in   casetype  type     UPPER/LOWER
 *                                        
 *  Value       :  type = { UPPER, LOWER }
 *
 *  Decisions   :  If character > 255, no change made.
 *
 *  Return      :  code of char translated
 *
 *  OS/Compiler :  All
 ***/

EXTERN int chcase( int , casetype ) ;

/***
 *  Function    :  strcomp
 *
 *  Description :  Compare two strings case-insensitive
 *		   All special characters are translated (‚Š…...)
 *
 *  Parameters  :  in   char  *str1
 *                 in   char  *str2
 *                 
 *  Return      :  -1 if str1 <  str2
 *		    1 if str1 >  str2
 *		    0 if str1 == str2
 *
 *  OS/Compiler :  All
 ***/

EXTERN int strcomp( const char *str1, const char *str2 );
 


/***
 *  Function    :  strncomp
 *
 *  Description :  Compare n characters of two strings case-insensitive
 *		   All special characters are translated (‚Š…...)
 *
 *  Parameters  :  in   char  *str1
 *                 in   char  *str2
 *
 *  Return      :  -1 if str1 <  str2
 *		    1 if str1 >  str2
 *		    0 if str1 == str2
 *
 *  OS/Compiler :  All
 ***/

EXTERN int strncomp( const char *str1, const char *str2, int length );



/***
 *  Function    :  matchset
 *
 *  Description :  Test if a character matches a set expression.
 *
 *  Parameters  :  in    char c          character to be matched
 *                 in    char *pattern   regular expression to match
 *
 *  Parameters  :  in   char   *string
 *
 *  Decisions   :  The following symbols are treated specially:
 *
 *              \  quote next character      -  range of values
 *              ^  non-inclusion (if first character)
 *
 *              ex: aeiou0-9   match a, e, i, o, u, and 0 thru 9
 *                  ^aeiou0-9  match anything but a, e, i, o, u, and 0 thru 9
 *
 *  Side-effects:  pattern contents will be destroyed.
 *
 *  Return      :  1 or 0
 *
 *  OS/Compiler :  All
 ***/

EXTERN int matchset( char c, char *pattern );



/***
 *  Function    :  recursexp
 *
 *  Description :  Returns the number of character of a string matched
 *                 by a regular expression.
 *
 *  Decisions   :  The following symbols are treated specially:
 *
 *                 .  any character             \  quote next character
 *                 *  match zero or more        +  match one or more
 *                 [] set of characters
 *
 *
 *  Parameters  :  in   char *string    input string to be matched
 *                 in   char *pattern   regular expression to match
 *
 *  Return      :  number of character matched by regular expression
 *                 -1 if not matched
 *
 *  OS/Compiler :  All
 ***/

EXTERN int recursexp( const char *string, char *pattern );



/***
 *  Function    :  regexp
 *
 *  Description :  Returns the string matched by a regular expression
 *                 into a string.
 *
 *  Decisions   :  The following symbols are treated specially:
 *
 *                 ^  start of line             $  end of line
 *                 .  any character             \  quote next character
 *                 *  match zero or more        +  match one or more
 *                 ?  match zero or one         [] set of characters
 *
 *              ex: [aeiou0-9]   match a, e, i, o, u, and 0 thru 9
 *                  [^aeiou0-9]  match anything but a, e, i, o, u, and 0 thru 9
 *
 *  Parameters  :  out   char *outstr    resulting string
 *                 in    char *string    input string in which we search
 *                 in    char *pattern   regular expression to match
 *
 *  Return      :  - NULL if not found
 *                 - pointer to resulting string
 *                 - if ( outstr == NULL ) returns pointer to matched string
 *                   inside 'string'.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *regexp( char *outstr, const char *string, const char *pattern );



/***
 *  Function    :   strtohex
 *
 *  Description :   convert a string containing an hexadecimal
 *                  representation into an hexadecimal flow of bytes.
 *
 *  Parameters  :   char *string   in/out
 *
 *  Return      :   length of hexadecimal flow of bytes.
 *                  -1 on error
 *
 *  OS/Compiler :   All
 ***/

EXTERN int strtohex( char *string );



/***
 *  Function    :  hextostr
 *
 *  Description :  Convert an hexadecimal flow of bytes into
 *                 a string containing an hexadecimal representation.
 *
 *  Parameters  :  out   char *string
 *                 in    char *hexa
 *                 in    int  length        number of hexadecimal bytes
 *
 *  Return      :   0 if OK
 *                 -1 on error
 *
 *  OS/Compiler :  All
 ***/

EXTERN int hextostr( char *string, const char *hexa, int length );



/***
 *  Function    :  strcpyb
 *
 *  Description :  Like strcpy but truncates trailing blanks and \n
 *                 and skip leading blanks.
 *
 *  Parameters  :  out   char * target
 *		   in    char * source
 *
 *  Return code :  like strcpy
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strcpyb( char *target, const char *source );



/***
 *  Function    : strncpyb
 *
 *  Description :  Like strncpy but truncates trailing blanks and \n,
 *                 skip leading blanks and adds a '\0'.
 *
 *  Decisions   :
 *
 *  Parameters  :  out   char * target
 *	           in    char * source
 *                 in    int    length
 *
 *  Return code :  like strncpy
 *
 *  Side-effects:  Pad target string to length with '\0'.
 *
 *  OS/Compiler :  All
 ***/

EXTERN char *strncpyb( char *target, const char *source, int length );


/***
 *  Function    :  strcalc
 *
 *  Description :  Mathematical expression evaluation 
 *
 *  Parameters  :  in/out   char *   calcul      expression to calculate
 *
 *                    + - * / () = < > <= >= <> (or !=)  on floats
 *                    %                                  on ints
 *
 *  Return      :  result   if OK
 *                 HUGE_VAL if error  
 *
 *  Remark      :  Hexadecimal numbers must be followed by 'H'
 *
 *  OS/Compiler :  All
 ***/
                   
                
EXTERN long double strcalc( char *calcul );

#endif

