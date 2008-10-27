/*
 * CheckDupPkt.{cc,hh} -- Check duplicated packets element
 * Soon Y. Oh
 *
 * Copyright (c) 2006 University of California, Los Angeles
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

/*
  Input : receive multicast packets	- odmrp control packets & multicast data packets
  Output 0 : send packets to odmrp agent element
  Output 1 : Discard  
*/

#include <click/config.h>
#include <click/router.hh>
#include <click/elemfilter.hh>
#include "checkduppkt.hh"
#include <click/error.hh>
#include <click/packet_anno.hh>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include <click/confparse.hh>
#include "debug.hh"


/*******************************************************************************************
 *                                                                                         *
 * Constructor - initialize variables                                                      *
 *                                                                                         *
 *******************************************************************************************/
CheckDupPkt::CheckDupPkt()
{
  int i;
  
  for(i=0 ; i<MSG_CACHE_SIZE ; i++) {
    msg_cache[i].src = 0;
    msg_cache[i].group = 0;
    msg_cache[i].pid = 0;
  }

  for(i=0 ; i<CON_CACHE_SIZE ; i++) {
    con_cache[i].src = 0;
    con_cache[i].group = 0;
    con_cache[i].pid = 0;
  }
  
  d_index = c_index = 0;
}


/*******************************************************************************************
 *                                                                                         *
 * Destructor                                                                              *
 *                                                                                         *
 *******************************************************************************************/
CheckDupPkt::~CheckDupPkt()
{
}


/*******************************************************************************************
 *                                                                                         *
 * Enter the message in the cache table                                                    *
 *                                                                                         *
 *******************************************************************************************/
void
CheckDupPkt::enterInCache(IPAddress s, IPAddress m, uint16_t id){
  msg_cache[d_index].src = s;
  msg_cache[d_index].group = m;
  msg_cache[d_index].pid = id;

  d_index = (d_index+1)%MSG_CACHE_SIZE;

  return;
}


/*******************************************************************************************
 *                                                                                         *
 * Look up in the message cache                                                            *
 *                                                                                         *
 *******************************************************************************************/
int
CheckDupPkt::lookup(IPAddress s, IPAddress m, uint16_t id){
  int i;
  
  for(i=0 ; i<MSG_CACHE_SIZE ; i++){
    if(msg_cache[i].src == s && msg_cache[i].group == m && msg_cache[i].pid == id){
      return 1;
    }
  }
  return 0;
}


/*******************************************************************************************
 *                                                                                         *
 * Enter the control packet in the control cache table                                     *
 *                                                                                         *
 *******************************************************************************************/
void
CheckDupPkt::enterInCache_con(IPAddress s, IPAddress m, uint16_t id){
  con_cache[c_index].src = s;
  con_cache[c_index].group = m;
  con_cache[c_index].pid = id;

  c_index = (c_index+1)%CON_CACHE_SIZE;

  return;
}


/*******************************************************************************************
 *                                                                                         *
 * Look up the control cache                                                               *
 *                                                                                         *
 *******************************************************************************************/
int
CheckDupPkt::lookup_con(IPAddress s, IPAddress m, uint16_t id){
  int i;
  
  for(i=0 ; i<CON_CACHE_SIZE ; i++){
    if(con_cache[i].src == s && con_cache[i].group == m && con_cache[i].pid == id){
      return 1;
    }
  }
  return 0;
}

/*******************************************************************************************
 *                                                                                         *
 * configure - pass a self IP Address to check packet sent by itself                       *
 *                                                                                         *
 *******************************************************************************************/
int
CheckDupPkt::configure(Vector<String> &conf, ErrorHandler *errh)
{
  if (conf.size() == 0){
    return errh->error("wrong number of arguments; ");
  }	
  IPAddress a;
  if (!cp_ip_address(conf[0], &a)) { 
		return errh->error("expects IPADDRESS -a ");
  }
  else {
		my_addr = a;		
  }  
  
  return 0;
}


/*******************************************************************************************
 *                                                                                         *
 * Push - receive packets and check duplication
 *                                                                                         *
 *******************************************************************************************/
void CheckDupPkt::push(int /*port*/, Packet *p)

{
	// declare variables
  IPAddress src_addr, mcast_addr;
  uint16_t seq;
  click_ip* ip;
  ip = (click_ip *)p->data();
  click_udp *udp = (click_udp *)(p->data() + sizeof(*ip));
  
  // get source and multicast IP address and IP sequence number
  src_addr = IPAddress(ip->ip_src);
  mcast_addr = IPAddress(ip->ip_dst);
  seq = ip->ip_id;

  if(ntohs(udp->uh_sport) == 999 && ntohs(udp->uh_dport) == 998){
  	// This is an odmrp control packet
		OdmrpHeader *e_header = (OdmrpHeader *)(p->data()+sizeof(*ip)+sizeof(*udp));
		seq = e_header->seqno_;
		src_addr = e_header->sender_addr_;
		
  	if(lookup_con(src_addr, mcast_addr, seq)||src_addr == my_addr){
    	output(1).push(p);
  	}	else{
    	enterInCache_con(src_addr, mcast_addr, seq);
    	output(0).push(p);
  	}
  }else{
	//  This is a multicast data packet  
		if(seq == 0 && (udp->uh_dport==7788 || ntohs(udp->uh_dport) == 7788)){
			struct temp_pack *temp_p = (struct temp_pack *)(p->data() + sizeof(*ip) +	sizeof(*udp));
			seq = (uint16_t)temp_p->seq_no;
		}
		if(lookup(src_addr, mcast_addr, seq)){
		//debug_msg("Find duplicated packets seq num %d\n",seq);
    	output(1).push(p);
  	}else if(src_addr == my_addr){
		output(1).push(p);
	}else{
    	enterInCache(src_addr, mcast_addr, seq);
    	output(0).push(p);
  	}
  }

}

EXPORT_ELEMENT(CheckDupPkt)
