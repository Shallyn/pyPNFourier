/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#include <string.h>
#include "utils_StringVector.h"
#include "basic_Error.h"
#include "basic_Alloc.h"
#include "basic_String.h"

StringVector *
AppendString2Vector (StringVector *vect,		/**< input string-vector to append to */
			 const CHAR *string		/**< string to append */
			 )
{
  UINT4 oldlen;
  StringVector *ret;

  if ( !string ) {
    XPrintError ("\n%s: NULL 'string' passed to append\n\n", __func__ );
    X_ERROR_NULL ( X_EINVAL );
  }

  if ( ! vect )
    { /* special case: NULL string-vector to append to */
      if ( (ret = CreateStringVector ( string, NULL )) == NULL) {
        XPrintError ("%s: CreateStringVector() failed!\n", __func__ );
        X_ERROR_NULL ( X_EFUNC );
      }
    }
  else
    {
      ret = vect;
      oldlen = ret->length;

      if ( (ret->data = XRealloc ( ret->data, (oldlen + 1)*sizeof( *ret->data ) )) == NULL ) {
        XPrintError ("%s: XRealloc(%d) failed!\n", __func__, oldlen + 1 );
	X_ERROR_NULL ( X_ENOMEM );
      }

      ret->length ++;

      if ( (ret->data[oldlen] = XCalloc(1, strlen(string) + 1 )) == NULL ) {
        XPrintError ("%s: XCalloc(%zu) failed!\n", __func__, strlen(string) + 1 );
	X_ERROR_NULL ( X_ENOMEM );
      }

      strcpy ( ret->data[oldlen], string );
    }

  return ret;

} /* XAppendString2Vector() */




/**
 * Create a StringVector from the list of strings passed as arguments
 * \note All arguments MUST be CHAR* strings.
 * The last argument MUST be NULL, as C cannot deduce the number of arguments
 * otherwise.
 */
StringVector *
CreateStringVector ( const CHAR *str1, ... )
{
  StringVector *ret;
  const CHAR *next;
  va_list ap;

  if ( !str1 ) {
    XPrintError ("%s: invalid NULL input string 'str1'\n", __func__ );
    X_ERROR_NULL ( X_EINVAL );
  }

  size_t len;

  /* set up return vector of strings, and handle first argument */
  len = sizeof(*ret);
  if ( (ret = XCalloc ( 1, len )) == NULL )
    goto failed;

  len = sizeof(ret->data[0]);
  if ( (ret->data = XCalloc ( 1, len )) == NULL)
    goto failed;

  len = strlen(str1)+1;
  if ( (ret->data[0] = XCalloc ( len, sizeof(CHAR) )) == NULL )
    goto failed;

  strcpy ( ret->data[0], str1 );
  ret->length ++;

  /* handle remaining variable-length list of (assumed)string arguments */
  va_start(ap, str1);

  while ( (next = va_arg(ap, const CHAR *)) != NULL )
    {
      ret->length ++;

      len = ret->length * sizeof(ret->data[0]);
      if ( (ret->data = XRealloc ( ret->data, len)) == NULL )
	goto failed;

      len = strlen(next)+1;
      if ( (ret->data[ret->length-1] = XCalloc( len, sizeof(CHAR) )) == NULL )
	goto failed;

      strcpy ( ret->data[ret->length-1], next );

    } /* while more arguments */

  va_end(ap);

  return ret;

 failed:
  va_end(ap);
  XPrintError ("%s: failed to allocate '%zu' bytes\n", __func__, len );
  DestroyStringVector ( ret );
  X_ERROR_NULL ( X_ENOMEM );

} /* CreateStringVector() */


/**
 * Create an empty string vector of the given length
 */
StringVector *CreateEmptyStringVector ( UINT4 length )
{
  StringVector * vector;
  vector = MYMalloc( sizeof( *vector ) );
  if ( ! vector )
    X_ERROR_NULL( X_ENOMEM );
  vector->length = length;
  if ( ! length ) /* zero length: set data pointer to be NULL */
    vector->data = NULL;
  else /* non-zero length: allocate memory for data */
  {
    vector->data = MYCalloc( length, sizeof( *vector->data ) );
    if ( ! vector->data )
    {
      MYFree( vector );
      X_ERROR_NULL( X_ENOMEM );
    }
  }
  return vector;
}


/**
 * Create a copy of a string vector
 */
StringVector *CopyStringVector( const StringVector *vect )
{
  X_CHECK_NULL( vect != NULL && vect->data != NULL && vect->length > 0, X_EINVAL );
  StringVector *copy = CreateStringVector( vect->data[0], NULL );
  X_CHECK_NULL( copy != NULL, X_EFUNC );
  for (size_t i = 1; i < vect->length; ++i) {
    copy = AppendString2Vector( copy, vect->data[i] );
    X_CHECK_NULL( copy != NULL, X_EFUNC );
  }
  return copy;
} /* XCopyStringVector() */


/** X-interface: Free a string-vector ;) */
void
DestroyStringVector ( StringVector *vect )
{
  UINT4 i;

  if ( !vect )
    return;

  if ( vect->data )
    {
      for ( i=0; i < vect->length; i++ )
	{
	  if ( vect->data[i] )
	    XFree ( vect->data[i] );
	}

      XFree ( vect->data );
    }

  XFree ( vect );

  return;

} /* XDestroyStringVector() */



/* comparison function for strings */
static int StringCompare (const void *p1, const void *p2)
{
  /* this formulation explicitly follows the example given in 'man qsort' for string-array sorting
   * Quoting from there:
   ** The actual arguments to this function are "pointers to
   ** pointers to char", but strcmp(3) arguments are "pointers
   ** to char", hence the following cast plus dereference
   *
   */
  return strcmp ( * ( char * const *) p1, * ( char * const *) p2 );
}

///
/// Concatenate a string vector 'strings', separated by the string 'sep', into a single string
///
char *ConcatStringVector( const StringVector *strings, const char *sep )
{

  // Check input
  X_CHECK_NULL( strings != NULL, X_EFAULT );
  X_CHECK_NULL( sep != NULL, X_EFAULT );

  // Allocate a string of the required size
  int len = 1;
  if ( strings->length > 0 ) {
    len += strlen( strings->data[0] );
    for ( size_t i = 1; i < strings->length; ++i ) {
      len += strlen( sep ) + strlen( strings->data[i] );
    }
  }
  char *s = XCalloc( len, sizeof(*s) );
  X_CHECK_NULL( s != NULL, X_ENOMEM );

  // Concatenate the vector of strings into a single string
  if ( strings->length > 0 ) {
    strcpy( s, strings->data[0] );
    for ( size_t i = 1; i < strings->length; ++i ) {
      strcat( s, sep );
      strcat( s, strings->data[i] );
    }
  }

  return s;

}

///
/// Parse 'string' into a string vector of tokens, delimited by the characters 'delim'
///
StringVector *ParseStringVector( const char *string, const char *delim )
{

  // Check input
  X_CHECK_NULL( string != NULL, X_EFAULT );
  X_CHECK_NULL( delim != NULL, X_EFAULT );

  // Allocate an empty string vector
  StringVector *strings = XCalloc( 1, sizeof(*strings) );
  X_CHECK_NULL( strings != NULL, X_ENOMEM );

  // Split the string into a vector of strings
  char *s = XStringDuplicate( string );
  X_CHECK_NULL( s != NULL, X_EFUNC );
  char *p = s;
  char *t = XStringToken( &p, delim, 1 );
  while ( t != NULL ) {
    X_CHECK_NULL( AppendString2Vector( strings, t ) != NULL, X_EFUNC );
    t = XStringToken( &p, delim, 1 );
  }

  // Cleanup
  XFree( s );

  return strings;

}

/**
 * Sort string-vector alphabetically *in place*
 */
int
SortStringVector (StringVector *strings)
{
  if ( !strings || strings->length == 0 ) {
    XPrintError ("%s: invalid empty or zero-length input 'strings'\n", __func__ );
    X_ERROR ( X_EINVAL );
  }

  qsort ( (void*)(&strings->data[0]), (size_t)(strings->length), sizeof(strings->data[0]), StringCompare );

  return X_SUCCESS;

} /* XSortStringVector() */

/**
 * Copy (and allocate) string from 'start' with length 'len', removing
 * all starting- and trailing blanks!
 */
char *
DeblankString ( const CHAR *start, UINT4 len )
{
  X_CHECK_NULL ( start != NULL, X_EINVAL );
  X_CHECK_NULL ( len > 0, X_EDOM );

  const CHAR *blank_chars = " \t\n";

  /* clip from beginning */
  const char *pos0 = start;
  const char *pos1 = start + len - 1;
  while ( (pos0 < pos1) && strchr ( blank_chars, (*pos0) ) ) {
    pos0 ++;
  }

  /* clip backwards from end */
  while ( (pos1 >= pos0) && strchr ( blank_chars, (*pos1) ) ) {
    pos1 --;
  }

  UINT4 newlen = pos1 - pos0 + 1;
  X_CHECK_NULL ( newlen > 0, X_EFAILED, "newlen==0: Something went wrong here .. probably a coding mistake.\n" );

  CHAR *ret;
  X_CHECK_NULL ( (ret = XCalloc(1, newlen + 1)) != NULL, X_ENOMEM );

  strncpy ( ret, pos0, newlen );
  ret[ newlen ] = 0;

  return ret;

} /* XDeblankString() */

/**
 * Search for string 'needle' in string-vector 'haystack', return index to
 * first matching vector element if found, -1 outherwise.
 *
 * Note: function allows haystack=NULL input, in which case -1 (=not found) will be returned.
 *
 */
INT4
FindStringInVector ( const char *needle, const StringVector *haystack )
{
  if ( !needle ) {
    XPrintError ("%s: invalid NULL input 'needle'!\n", __func__ );
    X_ERROR ( X_EINVAL );
  }

  if ( !haystack || (haystack->length == 0) )	// no vector to search => not found
    return -1;

  UINT4 i;
  for ( i=0; i < haystack->length; i ++ )
    if ( !strcmp ( needle, haystack->data[i] ) )	// found it!
      return i;

  return -1;	// didn't find matching entry

} /* XFindStringInVector() */
