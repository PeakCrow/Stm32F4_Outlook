/*
  This file is part of CanFestival, a library implementing CanOpen
  Stack.

  Copyright (C): Edouard TISSERANT and Francis DUPIN

  See COPYING file for copyrights details.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
  USA
*/
/*!
** @file   nmtSlave.c
** @author Edouard TISSERANT and Francis DUPIN
** @date   Tue Jun  5 08:50:53 2007
**
** @brief
**
**
*/
#include "nmtSlave.h"
#include "states.h"
#include "canfestival.h"
#include "sysdep.h"

/*!
** put the slave in the state wanted by the master
**
** @param d
** @param m
**/
void proceedNMTstateChange(CO_Data* d, Message *m)
{
  if( d->nodeState == Pre_operational ||
      d->nodeState == Operational ||
      d->nodeState == Stopped ) {

    MSG_WAR(0x3400, "NMT received. for node :  ", (*m).data[1]);

    /* Check if this NMT-message is for this node */
    /* byte 1 = 0 : all the nodes are concerned (broadcast) */
    
	//printf("*m.data1:%d\r\n",(*m).data[1]);
	//printf("*d->bDeviceNodeId:%d\r\n",*d->bDeviceNodeId);		  
		  
		  
    if( ( (*m).data[1] == 0 ) || ( (*m).data[1] == *d->bDeviceNodeId ) ){

      switch( (*m).data[0]){ /* command specifier (cs) */
      case NMT_Start_Node:
	  	MSG_WAR(NMT_Start_Node, "NMT received. for node :  ", (*m).data[0]);
        if ( (d->nodeState == Pre_operational) || (d->nodeState == Stopped) )
          setState(d,Operational);
        break;

      case NMT_Stop_Node:
	  	MSG_WAR(NMT_Stop_Node, "NMT received. for node :  ", (*m).data[0]);
        if ( d->nodeState == Pre_operational ||
             d->nodeState == Operational )
          setState(d,Stopped);
        break;

      case NMT_Enter_PreOperational:
	  	MSG_WAR(NMT_Enter_PreOperational, "NMT received. for node :  ", (*m).data[0]);
        if ( d->nodeState == Operational ||
             d->nodeState == Stopped )
          setState(d,Pre_operational);
        break;

      case NMT_Reset_Node:
	  	MSG_WAR(NMT_Reset_Node, "NMT received. for node :  ", (*m).data[0]);
         if(d->NMT_Slave_Node_Reset_Callback != NULL)
            d->NMT_Slave_Node_Reset_Callback(d);
        setState(d,Initialisation);
        break;

      case NMT_Reset_Comunication:
	  	MSG_WAR(NMT_Reset_Comunication, "NMT received. for node :  ", (*m).data[0]);
         {
         UNS8 currentNodeId = getNodeId(d);
         
            if(d->NMT_Slave_Communications_Reset_Callback != NULL)
               d->NMT_Slave_Communications_Reset_Callback(d);
#ifdef CO_ENABLE_LSS
            // LSS changes NodeId here in case lss_transfer.nodeID doesn't 
            // match current getNodeId()
            
			MSG_WAR(NMT_Reset_Comunication, "d->lss_transfer.nodeID",d->lss_transfer.nodeID);
            if(currentNodeId!=d->lss_transfer.nodeID)
               currentNodeId = d->lss_transfer.nodeID;
#endif

            // clear old NodeId to make SetNodeId reinitializing
            // SDO, EMCY and other COB Ids
            *d->bDeviceNodeId = 0xFF; //zxb
         
            setNodeId(d, currentNodeId);
         }
         setState(d,Initialisation);
         break;

      }/* end switch */

    }/* end if( ( (*m).data[1] == 0 ) || ( (*m).data[1] ==
        bDeviceNodeId ) ) */
  }
}


/*!
**
**
** @param d
**
** @return
**/

Message m_my;
UNS8 slaveSendBootUp(CO_Data* d)
{
  
	int j = 0;	
	//UNS8 temp_noid = 0;
	MSG_WAR(heartbeatInit, "has go into slaveSendBootUp...........\r\n",0);

#ifdef CO_ENABLE_LSS
  if(*d->bDeviceNodeId==0xFF)return 0;
#endif

  MSG_WAR(0x3407, "Send a Boot-Up msg ", 0);

  /* message configuration */
	
	 //temp_noid  = *d->bDeviceNodeId;
  {
		
	  UNS16 tmp = NODE_GUARD << 7 | *(d->bDeviceNodeId); 
	  m_my.cob_id = UNS16_LE(tmp);
		MSG_WAR(0x3407, "d->bDeviceNodeId ", *(d->bDeviceNodeId));
		
		
  }
  m_my.rtr = NOT_A_REQUEST;
  m_my.len = 2;
  m_my.data[0] = 0x0000; //zxb
//	while(1)
	//{
		//j++;
	  canSend(d->canHandle,&m_my);
		//if(5000 == j)
		//{
		//   return 0;
	//	}
	//}

  //return canSend(d->canHandle,&m);
}

