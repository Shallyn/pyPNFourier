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

#define SERIESTYPE CONCAT2(DATATYPE,TimeSeries)
#define SEQUENCETYPE CONCAT2(DATATYPE,Vector)

#define DSERIES CONCAT2(Destroy,SERIESTYPE)
#define CSERIES CONCAT2(Create,SERIESTYPE)
#define XSERIES CONCAT2(Cut,SERIESTYPE)

#define DSEQUENCE CONCAT2(Destroy,SEQUENCETYPE)
#define CSEQUENCE CONCAT2(Create,SEQUENCETYPE)
#define XSEQUENCE CONCAT2(Cut,SEQUENCETYPE)

void DSERIES (
	SERIESTYPE *series
)
{
	if(series)
		DSEQUENCE (series->data);
	MYFree(series);
}


SERIESTYPE *CSERIES (
	const REAL8 epoch,
	REAL8 deltaT,
	size_t length
)
{
	SERIESTYPE *new;
	SEQUENCETYPE *sequence;

	new = MYMalloc(sizeof(*new));
	sequence = CSEQUENCE (length);
	if(!new || !sequence) {
		MYFree(new);
		DSEQUENCE (sequence);
		X_ERROR_NULL(X_EFUNC);
	}

    new->epoch = epoch;
	new->deltaT = deltaT;
	new->data = sequence;

	return new;
}


SERIESTYPE *XSERIES (
	const SERIESTYPE *series,
	size_t first,
	size_t length
)
{
	SERIESTYPE *new;
	SEQUENCETYPE *sequence;

	new = MYMalloc(sizeof(*new));
	sequence = XSEQUENCE (series->data, first, length);
	if(!new || !sequence) {
		MYFree(new);
		DSEQUENCE (sequence);
		X_ERROR_NULL(X_EFUNC);
	}

	*new = *series;
	new->data = sequence;
    new->epoch = new->epoch + first * new->deltaT;
	// XLALGPSAdd(&new->epoch, first * new->deltaT);
	return new;
}




#undef SERIESTYPE
#undef SEQUENCETYPE

#undef DSERIES
#undef CSERIES
#undef XSERIES
#undef MSERIES

#undef DSEQUENCE
#undef CSEQUENCE
#undef XSEQUENCE

