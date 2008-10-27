/*
 * OdmrpMemberTable.{cc,hh} -- data structure for ODMRP groups and receivers
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
#include "odmrpmemtable.hh"
#include <click/ipaddress.hh>
#include <click/elemfilter.hh>
#include <click/router.hh>
#include <click/confparse.hh>
#include "debug.hh"


OdmrpMemberTable::OdmrpMemberTable()
{
}


OdmrpMemberTable::~OdmrpMemberTable()
{
}


int
OdmrpMemberTable::initialize(ErrorHandler *)
{  
}


/*******************************************************************************************
 *                                                                                         *
 * configure: get base information for the odmrp member table                             *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpMemberTable::configure(Vector<String> &conf, ErrorHandler *errh)
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
		addgroup(t_addr, mode);		
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
OdmrpMemberTable::addgroup(IPAddress mcast, MODE m){
	if(mcast == IPAddress("255.255.255.255")) return ODMRP_FAIL;
	debug_msg("Add multicast mode %d",m);
  if(head == NULL){
    head = new memberTable;
    head->mcast_addr = mcast;
    head->mode = m;
    tail = head;
    head->next = NULL;
		tail = head;
    return ODMRP_SUCCESS;
  }else{
    tail->next = new memberTable;
    tail->next->mcast_addr = mcast;
    tail->next->mode = m;
    tail = tail->next;
    tail->next = NULL;
    return ODMRP_SUCCESS;
  }  
  return ODMRP_FAIL;  
}


/*******************************************************************************************
 *                                                                                         *
 *  remove multicast group and source                                                      *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpMemberTable::removergroup(IPAddress mcast){
  memberTable *temp = head;
  if(head->mcast_addr == mcast){
    if(head == tail){
      head = tail = NULL;
    }else{
      head = head->next;
    }
    delete temp;
    return ODMRP_SUCCESS;
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
				return ODMRP_SUCCESS;
      }
      temp = temp1;
      temp1 = temp->next;
    }
  }
  return ODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 *  update  multicast mode                                                                 *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpMemberTable::updatemode(IPAddress mcast, MODE m){
	// if the table is empty, add multicast group
	if(head == NULL){
		addgroup(mcast, m);
	  return ODMRP_SUCCESS;
	}

	if(head->mcast_addr == mcast){
	  head->mode = m;
	  return ODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mcast_addr == mcast){
		  temp->mode = m;
		  return ODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	// if there is no entry, add multicast group
	addgroup(mcast, m);
	return ODMRP_SUCCESS;
  
}


/*******************************************************************************************
 *                                                                                         *
 *  Return mode                                                                            *
 *                                                                                         *
 *******************************************************************************************/
uint8_t
OdmrpMemberTable::returnmode(IPAddress mcast){
	if(head == NULL){
	  return LISTENER_MODE;
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
	return LISTENER_MODE;
}
	

/*******************************************************************************************
 *                                                                                         *
 * check whether it is a source node and return result                                     *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpMemberTable::is_sender(){
	if(head == NULL){		
	  return ODMRP_FAIL;
	}

	if(head->mode == SOURCE_MODE){
	  return ODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode == SOURCE_MODE){
		  return ODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	return ODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * check whether it is a receiver or forwarder node and return result                      *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpMemberTable::is_receiver(){
	if(head == NULL){
	  return ODMRP_FAIL;
	}

	if(head->mode == RECEIVER_MODE || head->mode == FORRECEIVER_MODE){
	  return ODMRP_SUCCESS;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode == RECEIVER_MODE || temp->mode == FORRECEIVER_MODE){
		  return ODMRP_SUCCESS;
		}
		temp = temp->next;
	  }
	  
	}
	return ODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * return a multicast address that a node is a source                                      *
 *                                                                                         *
 *******************************************************************************************/
IPAddress
OdmrpMemberTable::sender_mcast_addr(){
	if(head == NULL){
	  return ODMRP_FAIL;
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
	return (IPAddress)ODMRP_FAIL;
}


/*******************************************************************************************
 *                                                                                         *
 * return a multicast address that a node is a receiver or forwarder                       *
 *                                                                                         *
 *******************************************************************************************/
IPAddress
OdmrpMemberTable::receiver_mcast_addr(){
	if(head == NULL){
	  return ODMRP_FAIL;
	}

	if(head->mode == RECEIVER_MODE || head->mode == FORRECEIVER_MODE){
	  return head->mcast_addr;
	}else{
	  memberTable *temp = head->next;
	  while(temp){
		if(temp->mode == RECEIVER_MODE || temp->mode == FORRECEIVER_MODE){
		  return temp->mcast_addr;
		}
		temp = temp->next;
	  }
	}
	return (IPAddress)ODMRP_FAIL;

}


void
OdmrpMemberTable::forwarder_expire(){
	memberTable *temp = head;
	while(temp){
		if(temp->mode == FORWARDER_MODE){
			temp->mode = LISTENER_MODE;
		}else if(temp->mode == FORRECEIVER_MODE){
			temp->mode = RECEIVER_MODE;
		}

		temp = temp->next;
	}
	debug_msg("Forwarder Expire");
	return;
}
  
EXPORT_ELEMENT(OdmrpMemberTable)
