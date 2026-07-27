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

#define FUNC CONCAT3(LAL,TYPECODE,ResizeArray)
#define XFUNC CONCAT2(Resize,ATYPE)
#define XFUNCL CONCAT3(Resize,ATYPE,L)
#define XFUNCV CONCAT3(Resize,ATYPE,V)
#define XCFUNC CONCAT2(Create,ATYPE)
#define XDFUNC CONCAT2(Destroy,ATYPE)

ATYPE * XFUNCL ( ATYPE *array, UINT4 ndim, ... )
{
  enum { maxdim = 16 };
  va_list ap;
  UINT4 dims[maxdim];
  UINT4 dim;

  if ( ! ndim )
  {
    XDFUNC ( array );
    return NULL;
  }
  if ( ndim > maxdim )
    X_ERROR_NULL( X_EINVAL );

  va_start( ap, ndim );
  for ( dim = 0; dim < ndim; ++dim )
    dims[dim] = va_arg( ap, UINT4 );
  va_end( ap );

  return XFUNCV ( array, ndim, dims );
}

ATYPE * XFUNCV ( ATYPE *array, UINT4 ndim, UINT4 *dims )
{
  UINT4Vector dimLength;

  if ( ! ndim )
  {
    XDFUNC ( array );
    return NULL;
  }
  if ( ! dims )
    X_ERROR_NULL( X_EINVAL );

  dimLength.length = ndim;
  dimLength.data   = dims;

  return XFUNC ( array, &dimLength );
}

ATYPE * XFUNC ( ATYPE *array, UINT4Vector *dimLength )
{
  UINT4 size = 1;
  UINT4 ndim;
  UINT4 dim;

  if ( ! array )
    return XCFUNC ( dimLength );
  if ( ! dimLength )
  {
    XDFUNC ( array );
    return NULL;
  }
  if ( ! dimLength->length )
    X_ERROR_NULL( X_EBADLEN );
  if ( ! dimLength->data )
    X_ERROR_NULL( X_EINVAL );

  ndim = dimLength->length;
  for ( dim = 0; dim < ndim; ++dim )
    size *= dimLength->data[dim];

  if ( ! size )
    X_ERROR_NULL( X_EBADLEN );

  /* resize array->dimLength vector if needed */
  if ( array->dimLength->length != ndim )
  {
    array->dimLength = ResizeUINT4Vector( array->dimLength, ndim );
    if ( ! array->dimLength )
      X_ERROR_NULL( X_EFUNC );
  }

  /* copy dimension lengths */
  memcpy( array->dimLength->data, dimLength->data,
      ndim * sizeof( *array->dimLength->data ) );

  /* reallocate data storage */
  array->data = MYRealloc( array->data, size * sizeof( *array->data ) );
  if ( ! array->data )
    X_ERROR_NULL( X_ENOMEM );

  return array;
}


void FUNC ( ProcStatus *status, ATYPE **array, UINT4Vector *dimLength )
{
  ATYPE *tmparr = NULL;

  INITSTATUS(status);

  ASSERT ( array != NULL, status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR );

  if ( ! array )
  {
    tmparr = XCFUNC ( dimLength );
  }
  else if ( ! dimLength )
  {
    XDFUNC ( *array );
    *array = NULL;
  }
  else
  {
    tmparr = XFUNC ( *array, dimLength );
  }

  if ( xErrno )
  {
    int code = xErrno;
    XClearErrno();
    if ( code == X_EINVAL )
    {
      ABORT (status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
    }
    if ( code == X_EBADLEN )
    {
      ABORT (status, AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH);
    }
    if ( code == X_ENOMEM )
    {
      ABORT (status, AVFACTORIESH_EMALLOC, AVFACTORIESH_MSGEMALLOC);
    }
  }

  *array = tmparr;

  RETURN (status);
}

#undef ATYPE
#undef FUNC
#undef XFUNC
#undef XFUNCL
#undef XFUNCV
#undef XCFUNC
#undef XDFUNC
