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

#define FUNC CONCAT3(LAL,TYPECODE,DestroyVector)
#define XFUNC CONCAT2(Destroy,VTYPE)

void XFUNC ( VTYPE *vector )
{
  if ( ! vector )
    return;
  if ( ( ! vector->length || ! vector->data ) && ( vector->length || vector->data  ) )
    X_ERROR_VOID( X_EINVAL );
#ifdef USE_ALIGNED_MEMORY_ROUTINES
  if ( vector->data )
    XFreeAligned( vector->data );
#else
  if ( vector->data )
    XFree( vector->data );
#endif
  vector->data = NULL; /* leave length non-zero to detect repeated frees */
  MYFree( vector );
  return;
}


void FUNC ( ProcStatus *status, VTYPE **vector )
{
  /*
   * Initialize status
   */

  INITSTATUS(status);

  /*
   * Check vector: is it non-NULL?
   */

  ASSERT( vector != NULL, status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR );

  /*
   * Check vector: does it point to non-NULL?
   */

  ASSERT( *vector != NULL, status, AVFACTORIESH_EUPTR, AVFACTORIESH_MSGEUPTR );

  /*
   * Check data in vector: does it point to non-NULL
   */

  ASSERT( (*vector)->data != NULL, status,
          AVFACTORIESH_EDPTR, AVFACTORIESH_MSGEDPTR );

  /* Ok, now let's free allocated storage */

  XFUNC ( *vector );
  if ( xErrno )
  {
    int code = xErrno;
    XClearErrno();
    if ( code == X_EFAULT )
    {
      ABORT( status, AVFACTORIESH_EUPTR, AVFACTORIESH_MSGEUPTR );
    }
    if ( code == X_EINVAL )
    {
      ABORT( status, AVFACTORIESH_EDPTR, AVFACTORIESH_MSGEDPTR );
    }
  }

  *vector = NULL;		/* make sure we don't point to freed struct */

  RETURN( status );
}

#undef VTYPE
#undef FUNC
#undef XFUNC


