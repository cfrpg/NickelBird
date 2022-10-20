#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"

#define FilterQueueSize 0x07

typedef struct
{
	u8 count;
	u8 head;
	u8 tail;
	u8 _pad;
	float val[8];
	
} FilterQueue;

typedef struct
{
	s32 order;
	float num[8];
	float den[8];
	float val;
	FilterQueue xs;
	FilterQueue ys;	
} Filter;

void FilterEnqueue(FilterQueue *q, float v);
float FilterDequeue(FilterQueue *q);
float FilterSeek(FilterQueue *q, u8 index);
void FilterClearQueue(FilterQueue *q);

void FilterClear(Filter *f);
float FilterProcess(Filter *f,float val);


#endif
