/**
* Writer: Xiaolin.liu
* xiaolin.liu@mail.bnu.edu.cn
*
* This module contains basic functions for  calculation.
* Functions list:
* Kernel: 
* 20xx.xx.xx, LOC
**/

#define CONCAT2x(a,b) a##b
#define CONCAT2(a,b) CONCAT2x(a,b)
#define CONCAT3x(a,b,c) a##b##c
#define CONCAT3(a,b,c) CONCAT3x(a,b,c)
#define STRING(a) #a

#define ATYPE CONCAT2(TYPE,Array)

#define FUNC CONCAT3(LAL,TYPECODE,CreateArray)
#define XFUNC CONCAT2(Create,ATYPE)
#define XFUNCL CONCAT3(Create,ATYPE,L)
#define XFUNCV CONCAT3(Create,ATYPE,V)

ATYPE * XFUNCL ( UINT4 ndim, ... )
{
  enum { maxdim = 16 };
  va_list ap;
  ATYPE *arr;
  UINT4 dims[maxdim];
  UINT4 dim;

  if ( ! ndim )
    X_ERROR_NULL( X_EBADLEN );
  if ( ndim > maxdim )
    X_ERROR_NULL( X_EINVAL );

  va_start( ap, ndim );
  for ( dim = 0; dim < ndim; ++dim )
    dims[dim] = va_arg( ap, UINT4 );
  va_end( ap );

  arr = XFUNCV ( ndim, dims );
  if ( ! arr )
    X_ERROR_NULL( X_EFUNC );
  return arr;
}

ATYPE * XFUNCV ( UINT4 ndim, UINT4 *dims )
{
  ATYPE *arr;
  UINT4Vector dimLength;

  if ( ! ndim )
    X_ERROR_NULL( X_EBADLEN );
  if ( ! dims )
    X_ERROR_NULL( X_EFAULT );

  dimLength.length = ndim;
  dimLength.data   = dims;

  arr = XFUNC ( &dimLength );
  if ( ! arr )
    X_ERROR_NULL( X_EFUNC );
  return arr;
}


ATYPE * XFUNC ( UINT4Vector *dimLength )
{
  ATYPE *arr;
  UINT4 size = 1;
  UINT4 ndim;
  UINT4 dim;

  if ( ! dimLength )
    X_ERROR_NULL( X_EFAULT );
  if ( ! dimLength->length )
    X_ERROR_NULL( X_EBADLEN );
  if ( ! dimLength->data )
    X_ERROR_NULL( X_EINVAL );

  ndim = dimLength->length;
  for ( dim = 0; dim < ndim; ++dim )
    size *= dimLength->data[dim];

  if ( ! size )
    X_ERROR_NULL( X_EBADLEN );

  /* create array */
  arr = MYMalloc( sizeof( *arr ) );
  if ( ! arr )
    X_ERROR_NULL( X_ENOMEM );

  /* create array dimensions */
  arr->dimLength = CreateUINT4Vector( ndim );
  arr->size = size;
  if ( ! arr->dimLength )
  {
    MYFree( arr );
    X_ERROR_NULL( X_EFUNC );
  }

  /* copy dimension lengths */
  memcpy( arr->dimLength->data, dimLength->data,
      ndim * sizeof( *arr->dimLength->data ) );

  /* allocate data storage */
  arr->data = MYMalloc( size * sizeof( *arr->data ) );
  if ( ! arr->data )
  {
    DestroyUINT4Vector( arr->dimLength);
    MYFree( arr );
    X_ERROR_NULL( X_ENOMEM );
  }

  return arr;
}



void FUNC ( ProcStatus *status, ATYPE **array, UINT4Vector *dimLength )
{
  INITSTATUS(status);

  /* make sure arguments are sane */

  ASSERT (array,             status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
  ASSERT (!*array,           status, AVFACTORIESH_EUPTR, AVFACTORIESH_MSGEUPTR);
  ASSERT (dimLength,         status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
  ASSERT (dimLength->data,   status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
  ASSERT (dimLength->length, status,
          AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH);

  *array = XFUNC ( dimLength );
  if ( ! *array )
  {
    int code = xErrno & ~X_EFUNC; /* turn off subfunction error bit */
    XClearErrno();
    if ( code & X_EFAULT )
    {
      ABORT (status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
    }
    if ( code == X_EBADLEN )
    {
      ABORT (status, AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH);
    }
    if ( code == X_EINVAL )
    {
      ABORT (status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
    }
    if ( code == X_ENOMEM )
    {
      ABORT (status, AVFACTORIESH_EMALLOC, AVFACTORIESH_MSGEMALLOC);
    }
  }

  RETURN (status);
}

#undef ATYPE
#undef FUNC
#undef XFUNC
#undef XFUNCL
#undef XFUNCV
