#include "filter.h"

void FilterEnqueue(FilterQueue *q, float v)
{
	if(((q->tail+1)&FilterQueueSize)==q->head)
		return;
	q->val[q->tail]=v;
	q->tail=(q->tail+1)&FilterQueueSize;
	q->count++;
}

float FilterDequeue(FilterQueue *q)
{
	if(q->tail==q->head)
		return 0;
	float r=q->val[q->head];
	q->head=(q->head+1)&FilterQueueSize;
	q->count--;
	return r;
}

float FilterSeek(FilterQueue *q, u8 index)
{
	if(index>=q->count)
		return 0;
	return q->val[(q->head+index)&FilterQueueSize];
}

void FilterClearQueue(FilterQueue *q)
{
	q->head=0;
	q->tail=0;
	q->count=0;
}

void FilterClear(Filter *f)
{
	FilterClearQueue(&f->xs);
	FilterClearQueue(&f->ys);
}

float FilterProcess(Filter *f,float val)
{
	float res;
	u8 i;
	
	if(f->ys.count<f->order)
	{
		FilterEnqueue(&f->xs,val);
		res=f->num[0]*FilterSeek(&f->xs,f->xs.count-1);
		for(i=0;i<f->xs.count-1;i++)
		{
			res+=f->num[i]*FilterSeek(&f->xs,f->xs.count-1-i);
			res-=f->den[i]*FilterSeek(&f->ys,f->ys.count-i);
		}
		FilterEnqueue(&f->ys,res);
		return val;
	}
	FilterEnqueue(&f->xs,val);
	res=f->num[0]*FilterSeek(&f->xs,f->order);
	for(i=1;i<=f->order;i++)
	{
		res+=f->num[i]*FilterSeek(&f->xs,f->order-i);
		res-=f->den[i]*FilterSeek(&f->ys,f->order-i);
	}
	
	res/=f->den[0];
	FilterDequeue(&f->ys);
	FilterDequeue(&f->xs);
	FilterEnqueue(&f->ys,res);
	f->val=res;
	
	return res;
}
