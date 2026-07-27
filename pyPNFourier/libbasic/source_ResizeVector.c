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

#define VTYPE CONCAT2(TYPE,Vector)

#define RESIZEVECTOR CONCAT3(LAL,TYPECODE,ResizeVector)
#define XFUNC CONCAT2(Resize,VTYPE)

#define XCFUNC CONCAT2(Create,VTYPE)
#define XDFUNC CONCAT2(Destroy,VTYPE)

#define CUT_FUNC CONCAT2(Cut,VTYPE)
#define COPY_FUNC CONCAT2(Copy,VTYPE)

VTYPE * XFUNC ( VTYPE * vector, UINT4 length )
{
  if ( ! vector )
    return XCFUNC ( length );
  if ( ! length )
  {
    XDFUNC ( vector );
    return NULL;
  }
  vector->data = MYRealloc( vector->data, length * sizeof( *vector->data ) );
  if ( ! vector->data )
  {
    vector->length = 0;
    X_ERROR_NULL( X_ENOMEM );
  }
  vector->length = length;
  return vector;
}



void RESIZEVECTOR ( ProcStatus *status, VTYPE **vector, UINT4 length )
{
  /*
   * Initialize status structure
   */
  INITSTATUS(status);

  ASSERT ( vector != NULL, status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR );
  ASSERT ( ! *vector || (*vector)->length, status, AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH );
  ASSERT ( length || *vector, status, AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH );

  /* Want this to behave like realloc(3), i.e.
   * *vector == NULL => create a new vector
   * length == 0 => destroy the vector
   * otherwise => resize given vector
   */

  *vector = XFUNC ( *vector, length );
  if ( xErrno )
  {
    int code = xErrno;
    XClearErrno();
    if ( code == X_EBADLEN )
    {
      ABORT( status, AVFACTORIESH_ELENGTH, AVFACTORIESH_MSGELENGTH );
    }
    if ( code == X_ENOMEM )
    {
      ABORT( status, AVFACTORIESH_EMALLOC, AVFACTORIESH_MSGEMALLOC );
    }
  }

  RETURN( status );
}

VTYPE *CUT_FUNC (
	VTYPE *vec,
	size_t first,
	size_t length
)
{
  VTYPE *new = XCFUNC (length);
  if(!new)
    X_ERROR_NULL(X_EFUNC);
  memcpy(new->data, vec->data + first, length * sizeof(*new->data));
  return new;
}

VTYPE *COPY_FUNC (
	VTYPE *vec
)
{
	return CUT_FUNC (vec, 0, vec->length);
}



#undef VTYP
#undef RESIZEVECTOR
#undef XFUNC
#undef XCFUNC
#undef XDFUNC
#undef CUT_FUNC
#undef COPY_FUNC