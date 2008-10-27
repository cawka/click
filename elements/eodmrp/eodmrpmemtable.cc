/*
 * EodmrpMemberTable.{cc,hh} -- data structure for EODMRP groups and receivers
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
#include "eodmrpmemtable.hh"
#include <click/ipaddress.hh>
#include <click/elemfilter.hh>
#include <click/router.hh>
#include <click/confparse.hh>
#include "debug.hh"


EodmrpMemberTable::EodmrpMemberTable()
{
}


EodmrpMemberTable::~EodmrpMemberTable()
{
}


int
EodmrpMemberTable::initialize(ErrorHandler *)
{  
}


/*******************************************************************************************
 *                                                                                         *
 * configure: get base information for the eodmrp member table                             *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::configure(Vector<String> &conf, ErrorHandler *errh)
{
  IPAddress t_addr;
  unsigned int mode;  
	head = tail = NULL;
  
  if (conf.size()) {
  	// get mode
		cp_unsigned(conf[0], &mode); 
		// get a multicast IP address
    cp_ip_address(conf[1], &t_addr);
    // add multicast IP address in the member table
		addgroup(t_addr, mode, DEFAULT_PKT_ARRIVED_INTERVAL);		
		return 0;
  }
  if (conf.size() == 0) {
		return 0;
  }
  else {
		return -1;
  }
}


/*******************************************************************************************
 *                                                                                         *
 *  add multicast group in the member table                                                *
 *                                                                                         *
 *******************************************************************************************/
int 
EodmrpMemberTable::addgroup(IPAddress mcast, MODE m, double interval){
	if(mcast == IPAddress("255.255.255.255")) return EODMRP_FAIL;
	debug_msg("Add multicast mode %d",m);
  if(head == NULL){
    head = new memberTable;
    head->mcast_addr = mcast;
    head->mode = m;
    head->pkt_arrived_interval = interval;
    head->last_pkt_received.set(0,0);
    head->refresh_interval = MINIMUM_REFRESH_INTERVAL;
    head->last_refresh.set(0,0);
    tail = head;
    head->next = NULL;
		tail = head;
    return EODMRP_SUCCESS;
  }else{
    tail->next = new memberTable;
    tail->next->mcast_addr = mcast;
    tail->next->mode = m;
    tail->next->pkt_arrived_interval = interval;
    tail->next->last_pkt_received.set(0,0);
    tail->next->refresh_interval = MINIMUM_REFRESH_INTERVAL;;
    tail->next->last_refresh.set(0,0);
    tail = tail->next;
    tail->next = NULL;
    return EODMRP_SUCCESS;
  }  
  return EODMRP_FAIL;  
}


/*******************************************************************************************
 *                                                                                         *
 *  remove multicast group and source                                                      *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::removergroup(IPAddress mcast){
  memberTable *temp = head;
  if(head->mcast_addr == mcast){
    if(head == tail){
      head = tail = NULL;
    }else{
      head = head->next;
    }
    delete temp;
    return EODMRP_SUCCESS;
  }else{
    memberTable *temp1;
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
 *  update  multicast mode                                                                 *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::updatemode(IPAddress mcast, MODE m){
	// if the table is empty, add multicast group
	if(head == NULL){
		addgroup(mcast, m, DEFAULT_PKT_ARRIVED_INTERVAL);
	  return EODMRP_SUCCESS;
	}

	if(head->mcast_addr == mcast){
	  head->mode = m;
	  return EODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
		  temp->mode = m;
		  return EODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	// if there is no entry, add multicast group
	addgroup(mcast, m, DEFAULT_PKT_ARRIVED_INTERVAL);
	return EODMRP_SUCCESS;
  
}


/*******************************************************************************************
 *                                                                                         *
 *  update last packet arrived time                                                        *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::updatereceivedtime(IPAddress mcast){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mcast_addr == mcast){
	  head->last_pkt_received.set_now();
	  return EODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
		  temp->last_pkt_received.set_now();
		  return EODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	return EODMRP_FAIL;

}
  
  
/*******************************************************************************************
 *                                                                                         *
 * return last packet arrived time                                                         *
 *                                                                                         *
 *******************************************************************************************/
Timestamp
EodmrpMemberTable::returnreceivedtime(IPAddress mcast){
	Timestamp t(0,0);
	if(head == NULL){
	  return t;
	}

	if(head->mcast_addr == mcast){
	  return head->last_pkt_received;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
		  return temp->last_pkt_received;
		}
		temp = temp->next;
	  }
	  
	}
	return t;
}


/*******************************************************************************************
 *                                                                                         *
 *  Return mode                                                                            *
 *                                                                                         *
 *******************************************************************************************/
uint8_t
EodmrpMemberTable::returnmode(IPAddress mcast){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mcast_addr == mcast){
	  return head->mode;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
		  return temp->mode;
		}
		temp = temp->next;
	  }
	  
	}
	return EODMRP_FAIL;
}
	

/*******************************************************************************************
 *                                                                                         *
 * check whether it is a source node and return result                                     *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::is_sender(){
	if(head == NULL){		
	  return EODMRP_FAIL;
	}

	if(head->mode == SOURCE_MODE){
	  return EODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode == SOURCE_MODE){
		  return EODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	return EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * check whether it is a receiver or forwarder node and return result                      *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::is_receiver(){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mode < TEMPFORWARDER_MODE && head->mode != SOURCE_MODE){
	  return EODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode < TEMPFORWARDER_MODE && temp->mode != SOURCE_MODE){
		  return EODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	return EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * return a multicast address that a node is a source                                      *
 *                                                                                         *
 *******************************************************************************************/
IPAddress
EodmrpMemberTable::sender_mcast_addr(){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mode == SOURCE_MODE){
	  return head->mcast_addr;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode == SOURCE_MODE){
		  return temp->mcast_addr;
		}
		temp = temp->next;
	  }
	}
	return (IPAddress)EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * return a multicast address that a node is a receiver or forwarder                       *
 *                                                                                         *
 *******************************************************************************************/
IPAddress
EodmrpMemberTable::receiver_mcast_addr(){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mode < TEMPFORWARDER_MODE && head->mode != SOURCE_MODE){
	  return head->mcast_addr;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode < TEMPFORWARDER_MODE && temp->mode != SOURCE_MODE){
		  return temp->mcast_addr;
		}
		temp = temp->next;
	  }
	}
	return (IPAddress)EODMRP_FAIL;

}


/*******************************************************************************************
 *                                                                                         *
 * recalculate and update refresh interval (only source)                                   *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpMemberTable::update_refresh_interval(IPAddress mcast){
	if(head == NULL){
	  return;
	}
	
	// variables
  Timestamp t;
  t.set_now();
  int32_t inter;
  
  // Error occurs this block  
	if(head->mcast_addr == mcast){
		if(head->last_refresh.sec() == 0){
			head->refresh_interval = MINIMUM_REFRESH_INTERVAL;
	  		head->last_refresh.set_now();
			return;
		}
	  inter = t.sec() - head->last_refresh.sec();	
	  head->last_refresh.set_now();
	  // interval is less than minimum value
	  if(inter < MINIMUM_REFRESH_INTERVAL){
        head->refresh_interval = MINIMUM_REFRESH_INTERVAL;
		}else if(inter+1 > head->refresh_interval){		
     	if(head->refresh_interval*2 > MAXIMUM_REFRESH_INTERVAL){
     		// interval reaches the maximum value
     		head->refresh_interval = MAXIMUM_REFRESH_INTERVAL;
     	}else{
     		// interval increases 
     		head->refresh_interval *= 2;
     	}        		
    }else {
     	// interval decreases
     	head->refresh_interval = inter;
    	}
	}else{
	  memberTable *temp = head->next;
	  while(temp){
	    	if(temp->mcast_addr == mcast){
				if(temp->last_refresh.sec() == 0){
					temp->refresh_interval = MINIMUM_REFRESH_INTERVAL;
			  		temp->last_refresh.set_now();
					return;
				}
			  inter = t.sec()-temp->last_refresh.sec();	
			  //t -= temp->last_refresh;
			  temp->last_refresh.set_now();
			  if(inter < MINIMUM_REFRESH_INTERVAL){
			  	// interval is less than minimum value
		      temp->refresh_interval = MINIMUM_REFRESH_INTERVAL;		      
		    }else if(inter+1 > temp->refresh_interval){		    	
		       if(temp->refresh_interval*2 > MAXIMUM_REFRESH_INTERVAL){
		       	 // interval reaches the maximum value
     				 temp->refresh_interval = MAXIMUM_REFRESH_INTERVAL;
     			}else{
     				// interval increases
     				temp->refresh_interval *= 2;
     			}        		
     		}else {
     			// interval decreases
     			temp->refresh_interval = inter;
    		}
		   }	    
	    temp = temp->next;
	  }
	}
  
}


/*******************************************************************************************
 *                                                                                         *
 * update last refresh time                                                                *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpMemberTable::update_last_refresh(IPAddress mcast){
	if(head == NULL){
	  return;
	}

	if(head->mcast_addr == mcast){
	  head->last_refresh.set_now();
	}else{
	  memberTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
			  temp->last_refresh.set_now();
			  return;
			}
			temp = temp->next;
	  }
	}
  return;
}


/*******************************************************************************************
 *                                                                                         *
 * set refresh interval with new value                                                     *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::set_refresh_interval(IPAddress mcast, int t){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

	if(head->mcast_addr == mcast){
	  head->refresh_interval = t;
	  return EODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
			if(temp->mcast_addr == mcast){
				head->refresh_interval = t;
		    return EODMRP_SUCCESS;		  
			}
		temp = temp->next;
	  }
	}
  return EODMRP_FAIL;	
}


/*******************************************************************************************
 *                                                                                         *
 * return refresh interval                                                                 *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpMemberTable::return_refresh_interval(IPAddress mcast){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

  if(head->mcast_addr == mcast){
	  return head->refresh_interval;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
			return temp->refresh_interval;		  
		}
		temp = temp->next;
	  }
	}
  return EODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * return packet arrived time                                                              *
 *                                                                                         *
 *******************************************************************************************/
double
EodmrpMemberTable::return_pkt_arrived_interval(IPAddress mcast){
	if(head == NULL){
	  return EODMRP_FAIL;
	}

  if(head->mcast_addr == mcast){
	  return head->pkt_arrived_interval;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
			return temp->pkt_arrived_interval;		  
		}
		temp = temp->next;
	  }
	}
  return EODMRP_FAIL;	
}

  
EXPORT_ELEMENT(EodmrpMemberTable)
