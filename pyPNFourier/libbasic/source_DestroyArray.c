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

#define FUNC CONCAT3(LAL,TYPECODE,DestroyArray)
#define XFUNC CONCAT2(Destroy,ATYPE)

void XFUNC ( ATYPE *array )
{
  if ( ! array )
    X_ERROR_VOID( X_EFAULT );
  if ( ! array->dimLength
      || ! array->dimLength->length
      || ! array->dimLength->data
      || ! array->data )
    X_ERROR_VOID( X_EINVAL );
  DestroyUINT4Vector( array->dimLength );
  MYFree( array->data );
  MYFree( array );
  return;
}


void FUNC ( ProcStatus *status, ATYPE **array )
{
  INITSTATUS(status);

  ASSERT (array,          status, AVFACTORIESH_EVPTR, AVFACTORIESH_MSGEVPTR);
  ASSERT (*array,         status, AVFACTORIESH_EUPTR, AVFACTORIESH_MSGEUPTR);
  ASSERT ((*array)->data, status, AVFACTORIESH_EDPTR, AVFACTORIESH_MSGEDPTR);

  /* Free allocated storage */

  XFUNC ( *array );
  if ( xErrno )
  {
    int code = xErrno;
    XClearErrno();
    if ( code == X_EFAULT )
    {
      ABORT (status, AVFACTORIESH_EUPTR, AVFACTORIESH_MSGEUPTR);
    }
    if ( code == X_EINVAL )
    {
      ABORT (status, AVFACTORIESH_EDPTR, AVFACTORIESH_MSGEDPTR);
    }
  }
  *array = NULL;	    /* make sure we don't point to freed struct */

  RETURN (status);
}

#undef ATYPE
#undef FUNC
#undef XFUNC
