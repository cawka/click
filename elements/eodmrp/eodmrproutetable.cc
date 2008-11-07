/*
 * EodmrpRoutingTable.{cc,hh} -- data structure for EODMRP groups and receivers
 * Soon Y. Oh
 *
 * Copyright (c) 2005, 2006 University of California, Los Angeles
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */


#include <click/config.h>
#include "eodmrproutetable.hh"
#include <click/ipaddress.hh>
#include <click/elemfilter.hh>
#include <click/router.hh>
#include "debug.hh"


EodmrpRoutingTable::EodmrpRoutingTable()
{
}


EodmrpRoutingTable::~EodmrpRoutingTable()
{
}


int
EodmrpRoutingTable::initialize(ErrorHandler *)
{ 
	head = tail = NULL; 
  return EODMRP_SUCCESS;
}


/*******************************************************************************************
 *                                                                                         *
 * configure - no argument                                                                 *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh)
{	
  if (conf.size() == 0) {
		return 0;
  }
  else {
		return -1;
  }
}


/*******************************************************************************************
 *                                                                                         *
 * Add a route in the routing table                                                        *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpRoutingTable::addroute(IPAddress mcast,IPAddress prev, int h){
	if(findRoute(mcast) == EODMRP_SUCCESS){
		// route is already in the routing table. Just update table
		updateroute(mcast, prev,h);
		return EODMRP_SUCCESS;
	}else{
		if(head == NULL){
	    head = new routingTable;
	    head->mcast_addr = mcast;
	    head->prevhop = prev;
	    head->hop_count = h;
	    head->next = NULL;
	    tail = head;
	    return EODMRP_SUCCESS;
	  }else{
	    tail->next = new routingTable;
	    tail->next->mcast_addr = mcast;
	    tail->next->prevhop = prev;
	    tail->next->hop_count = h;
	    tail = tail->next;
	    tail->next = NULL;
	    return EODMRP_SUCCESS;
	  }
	}

  return EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * Remove a route from the routing table                                                   *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpRoutingTable::removeroute(IPAddress mcast){
  routingTable *temp = head;
  if(head->mcast_addr == mcast){
    if(head == tail){
      head = tail = NULL;
    }else{
      head = head->next;
    }
    delete temp;
    return EODMRP_SUCCESS;
  }else{
    routingTable *temp1;
    temp1 = temp->next;
    while(temp1){
      if(temp1->mcast_addr == mcast){
        if(temp1 == tail){
	  			tail = temp;
				}else{
          temp->next = temp1->next;
				}
				delete temp1;
				return EODMRP_SUCCESS;
      }
      temp = temp1;
      temp1 = temp->next;
    }
  }
  return EODMRP_FAIL;	
}


/*******************************************************************************************
 *                                                                                         *
 * Set a previous node IP address                                                          *
 *                                                                                         *
 *******************************************************************************************/
int 
EodmrpRoutingTable::setprevhop(IPAddress mcast, IPAddress prev){
  if(head == NULL){
  	// no entry in the routing table
  	return EODMRP_FAIL;
  }
  
  if(head->mcast_addr == mcast){
	  head->prevhop = prev;
	  return EODMRP_SUCCESS;
	}else{
	  routingTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
			  temp->prevhop = prev;
			  return EODMRP_SUCCESS;
			}
			temp = temp->next;
	  }
	  
	}
	return EODMRP_FAIL;
  
}


/*******************************************************************************************
 *                                                                                         *
 * Find a new route and update a route table entry                                         *
 *                                                                                         *
 *******************************************************************************************/
int 
EodmrpRoutingTable::updateroute(IPAddress mcast, IPAddress prev,int h){
  if(head == NULL){
  	return addroute(mcast, prev, h);  	
  }
    
  if(head->mcast_addr == mcast){
	  head->prevhop = prev;
	  head->hop_count = h;
	  return EODMRP_SUCCESS;
	}else{
	  routingTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
		  	temp->prevhop = prev;
		  	temp->hop_count = h;
		  	return EODMRP_SUCCESS;
			}
			temp = temp->next;
	  }	  
	}
	// there is no route entry so add a route in the table
	return addroute(mcast, prev, h);
}


/*******************************************************************************************
 *                                                                                         *
 * Find a hop count (from the source) and retrun it                                        *
 *                                                                                         *
 *******************************************************************************************/
int 
EodmrpRoutingTable::findhop(IPAddress mcast){
  if(head == NULL){
  	return EODMRP_FAIL;  	
  }

  if(head->mcast_addr == mcast){
	  return head->hop_count;
	}else{
	  routingTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
			  return temp->hop_count;
			}
			temp = temp->next;
	  }	  
	}
	return EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * Find a previous hop address and return it                                               *
 *                                                                                         *
 *******************************************************************************************/
IPAddress
EodmrpRoutingTable::findprevhop(IPAddress mcast){
  if(head == NULL){
  	// no entry
  	return EODMRP_FAIL;  	
  }

	// find a previous hop address and return it
  if(head->mcast_addr == mcast){
	  return head->prevhop;
	}else{
	  routingTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
			  return temp->prevhop;
			}
			temp = temp->next;
	  }	  
	}
	// fail to find it
	return EODMRP_FAIL;

}


/*******************************************************************************************
 *                                                                                         *
 * Find a multicast IP address                                                             *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpRoutingTable::findRoute(IPAddress mcast){
  if(head == NULL){
  	// no entry
  	return EODMRP_FAIL;  	
  }

  if(head->mcast_addr == mcast){
	  return EODMRP_SUCCESS;
	}else{
	  routingTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
			  return EODMRP_SUCCESS;
			}
			temp = temp->next;
	  }	  
	}
	return EODMRP_FAIL;	
}


EXPORT_ELEMENT(EodmrpRoutingTable)

 
