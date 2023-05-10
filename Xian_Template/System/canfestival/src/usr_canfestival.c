#include "sys.h"
#include "data.h"

UNS8 canSend(CAN_PORT port, Message *m)
{
	if(bsp_Can1_Send_buf(m->cob_id,(uint8_t*)m->data,m->len) == 0)
    {
		return 0;
    }
    return 1;    
}
void setTimer(TIMEVAL value)
{
	
}

TIMEVAL getElapsedTime(void)
{
	return 1;
}




