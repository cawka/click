/*
 * OdmrpAgent.{cc,hh} -- OdmrpAgent processing element
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
  Output 0 : Discard
  Output 1 : Forwading packets -> encapsulate ethernet header
  Output 2 : Generate and send packets -> encapsulate UDP header
             -> encapsulate ethernet header
*/

#include <click/config.h>
#include <click/router.hh>
#include <click/elemfilter.hh>
#include "odmrpagent.hh"
#include <click/error.hh>
#include <click/packet_anno.hh>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include <click/confparse.hh>
#include "debug.hh"
//#include <time.h>
CLICK_DECLS


/*******************************************************************************************
 *                                                                                         *
 * Join Query timer is expired. Now generating Join Query Packets. It is only for source   *
 *                                                                                         *
 *******************************************************************************************/
static void
JQ_hook(Timer *, void *thunk)
{
  OdmrpAgent *e = (OdmrpAgent *)thunk;
  e->run_JQ_timer();
}


/*******************************************************************************************
 *                                                                                         *
 * Forwarder timer is expired. Now check and expire Forwader state                         *
 *                                                                                         *
 *******************************************************************************************/
static void
Forwarder_hook(Timer *, void *thunk)
{
  OdmrpAgent *e = (OdmrpAgent *)thunk;
  e->run_Forwarder_timer();
}
	
  
/*******************************************************************************************
 *                                                                                         *
 * Constructor function. Define timers.                                                   *
 *                                                                                         *
 *******************************************************************************************/
OdmrpAgent::OdmrpAgent():
  JQ_timer_(JQ_hook, this),
  Forwarder_timer_(Forwarder_hook, this)
{
}


/*******************************************************************************************
 *                                                                                         *
 * Destructor function. Do nothing                                                        *
 *                                                                                         *
 *******************************************************************************************/
OdmrpAgent::~OdmrpAgent()
{
}

/*******************************************************************************************
 *                                                                                         *
 * Initializing OdmrpAgent - initialize each timer and variables                          *
 * 							Set Join Query Timer 1 second and Data Timer 3 second                      *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpAgent::initialize(ErrorHandler *errh){
  // Initializing timers
  JQ_timer_.initialize(this);
  send_con_pkt = 0;
  seqno = num_con = received_JQ= received_JR = 0;
  last_JR.set(0, 0);
 
  // Send Join Query after 1 second and receiver join after 3 seconds
  JQ_timer_.schedule_after_sec(1);
  
   return 0;
}

/*******************************************************************************************
 *                                                                                         *
 * configure: get OdmrpAgent interface and OdmrpAgentForwardingTable element             *
 *                                                                                         *
 *******************************************************************************************/
int
OdmrpAgent::configure(Vector<String> &conf, ErrorHandler *errh)
{
  if (conf.size() < 3)
    return errh->error("wrong number of arguments; expected 'OdmrpAgent_SPT(OdmrpMemberTable OdmrpRoutingTable OdmrpAgent_SPT_interface)'");

  // get a Member Table element
  Element *e0 = cp_element(conf[0], this, errh);
  if (!e0) {
		return -1;
  }
  else {		
		memberTable = (OdmrpMemberTable *)e0->cast("OdmrpMemberTable");
		debug_msg("Get a Member Table");
  }

  // get a Routing Table element
  Element *e = cp_element(conf[1], this, errh);
  if (!e) {
		return -1;
  }
  else {		
		routingTable = (OdmrpRoutingTable *)e->cast("OdmrpRoutingTable");
		debug_msg("Get a Routing Table");
  }

  // get my IP Address
  IPAddress a;
  if (!cp_ip_address(conf[2], &a)) { 
		debug_msg("cp_ip_address failed!");
		return errh->error("expects IPADDRESS -a ");
  }
  else {
		my_addr = a;
		const unsigned char *p = my_addr.data();
		debug_msg("OdmrpAgent configured interface address: %d.%d.%d.%d", p[0], p[1], p[2], p[3]);
  }  
  
  return 0;
}


/*******************************************************************************************
 *                                                                                         *
 * Push - Receive packets and process them                                                 *
 *                                                                                         *
 *******************************************************************************************/
void 
OdmrpAgent::push(int port, Packet *p_in)
{
	// Change packet to writable packet
  WritablePacket *p = p_in->uniqueify();
  const click_ip *ip = reinterpret_cast<click_ip *>(p->data());
  // this is an udp packet
  const click_udp *udp = reinterpret_cast<click_udp *>(p->data() + sizeof(*ip));
  //debug_msg("OdmrpAgent receives a packet");
  OdmrpHeader *odmrpheader;

  uint8_t type;
  uint16_t seqno;
  uint8_t hop;
  IPAddress src, mcast, prev, sender;
  //double pktinterval, refreshinterval;
  send_con_pkt = 0;
  
  //debug_msg("Sport %d and dport %d",ntohs(udp->uh_sport), ntohs(udp->uh_dport));

  // This is odmrp control packet
  // Find a control packet using sending and destination port
  if(ntohs(udp->uh_sport) == 999 && ntohs(udp->uh_dport) == 998) {     
  	// process OdmrpAgent messages only  	
  	// get odmrp header  	
    odmrpheader=(OdmrpHeader *) (p->data() + sizeof(*ip)+sizeof(*udp)); 

    // get each values
    src = IPAddress(odmrpheader->src_);					//multicast source address
    mcast = IPAddress(odmrpheader->multicast_);	//multicast address
    prev = IPAddress(odmrpheader->prev_hop_addr_);		//previous hop address
    sender = IPAddress(odmrpheader->sender_addr_);		//packet sender address
    hop = (int)odmrpheader->hop_count_;							//hop counting
    const unsigned char *pa = src.data();
    const unsigned char *pm = mcast.data();
    const unsigned char *pp = prev.data();
    const unsigned char *ps = sender.data();
    
    debug_msg("Source : %d.%d.%d.%d  Multicast: %d.%d.%d.%d  Prev Address: %d.%d.%d.%d Sender:%d.%d.%d.%d Hop : %d ",
    		pa[0], pa[1], pa[2], pa[3],pm[0], pm[1], pm[2], pm[3],pp[0], pp[1], pp[2], pp[3],ps[0],ps[1],ps[2],ps[3],hop);

		switch(odmrpheader->type_) 
		  {
		  // Join Query Packet
		  case JQ:
		  	received_JQ++;
				debug_msg("OdmrpAgent : Join Query message arrived from %d.%d.%d.%d Total received JQ %d",
				 pp[0], pp[1], pp[2], pp[3], received_JQ);				
				// update routing table
				routingTable->addroute(mcast, prev, hop);
				// if it is receiver, generate Join Reply
				if(memberTable->returnmode(mcast) < (uint8_t)FORWARDER_MODE){
					// update member table
					//memberTable->set_refresh_interval(mcast, odmrpheader->refresh_interval_);
					//memberTable->update_last_refresh(mcast);
					// set last packet arrived time
				
					//gnerate Join Reply
				  generateJR(mcast, src, prev);					
				}
				// Modify odmrp header and rebroadcast
				odmrpheader->prev_hop_addr_ = IPAddress(my_addr);
				odmrpheader->hop_count_++;
				//num_con++;
				output(1).push(p);  
				break;
		  // Join Reply Packet
		  case JR:				
				// check destination - was this sent to me?
				if(my_addr == IPAddress(odmrpheader->prev_hop_addr_)){
					received_JR++;
					debug_msg("OdmrpAgent : Join Reply message arrived from %d.%d.%d.%d, Total JR received %d",
					 pp[0], pp[1], pp[2], pp[3], received_JR);
					//debug_msg("This Join Reply message is for me");
					// update member table
				  if(memberTable->returnmode(mcast) > (uint8_t)FORWARDER_MODE){
				  	// it was a listener and now it becomes a forwarder
				    memberTable->updatemode(mcast, FORWARDER_MODE);
				  }else if(memberTable->returnmode(mcast) == (uint8_t)RECEIVER_MODE){
				  	// it was a receiver and now it is a receiver and forwarder
				    memberTable->updatemode(mcast, FORRECEIVER_MODE);
				  }
				  // modify odmrp header
				  //odmrpheader->sender_addr_ = IPAddress(my_addr);
				  odmrpheader->hop_count_++;								//increase hop count
				  prev = routingTable->findprevhop(mcast);	//find a next node towarder a source
				  
				  // if no previous hop or it is a source node, discard JR packets
				  if(prev == (IPAddress)0 || memberTable->returnmode(mcast) == (uint8_t)SOURCE_MODE){
				    output(0).push(p);
				  }else{				  	
					last_JR.set_now();
					Forwarder_timer_.schedule_after_sec(REFRESH_INTERVAL*3);
				    odmrpheader->prev_hop_addr_ = prev;				    
				    num_con++;
				    output(1).push(p);  
				  }
				  // it is a join reply packet, but it is not for me
				}else{
				  output(0).push(p);  
			       }		
				break;
		  
		  default:
		  	// it is not a odmrp packet
		  	debug_msg("OdmrpAgent does not know this type of message! It came from %d.%d.%d.%d",pa[0], pa[1], pa[2], pa[3]);
				output(0).push(p);
		  }		  
  }
  else {
		//debug_msg("It is not an Odmrp Control Message");
		// This is not an odmrp control packet
		// get source and multicast address and copy packet for forwarding
  	src = IPAddress(ip->ip_src);
  	mcast = IPAddress(ip->ip_dst);
  	Packet *p = p_in->clone();
		
		// This is a forwarder or receiver
		if(memberTable->returnmode(mcast) == FORWARDER_MODE ||
			memberTable->returnmode(mcast) == FORRECEIVER_MODE){			
    	output(1).push(p);  
		}else{			
	    // do not need to forwarding so drop the packet
	  	output(0).push(p);	  
    }
  }   
}

/*******************************************************************************************
 *                                                                                         *
 * Generate Join Query packet with dynamic adaptive interval                               *
 *                                                                                         *
 *******************************************************************************************/
void 
OdmrpAgent::generateJQ(IPAddress mcast){
	// If a node is not source, return	
  if(memberTable->is_sender() != ODMRP_SUCCESS){  	
  	return;
  }
  
  // declare each header
  OdmrpHeader *odmrp_header;
  WritablePacket *q;
  
  // generate a packet
	q = Packet::make(sizeof(*odmrp_header));		  
  odmrp_header = (OdmrpHeader *)((char *)q->data());
 	
 	// create odmrp header
  odmrp_header->type_ = JQ; 
  odmrp_header->seqno_ = seqno;
  odmrp_header->hop_count_ = 1;
  odmrp_header->src_ = IPAddress(my_addr);
  odmrp_header->multicast_ = IPAddress(mcast);
  odmrp_header->prev_hop_addr_ = IPAddress(my_addr);
  odmrp_header->sender_addr_ = IPAddress(my_addr);
  
  num_con++;
  debug_msg("Node generates Join Query : Refresh Interval %d Total control pkt %d",
  REFRESH_INTERVAL, num_con);
  // increase sequence number
  seqno++;
	// send a Join Query packet
  output(2).push(q);  
  
  // set timer
  JQ_timer_.schedule_after_sec(REFRESH_INTERVAL);
}


/*******************************************************************************************
 *                                                                                         *
 * Generate Join Reply packet                                                              *
 *                                                                                         *
 *******************************************************************************************/
void 
OdmrpAgent::generateJR(IPAddress mcast, IPAddress src, IPAddress prev){
   // declare each header
  OdmrpHeader *odmrp_header;
  WritablePacket *q;
  
  // generate a packet
	q = Packet::make(sizeof(*odmrp_header));		  
  odmrp_header = (OdmrpHeader *)((char *)q->data());
 	
 	// create odmrp header
  odmrp_header->type_ = JR; 
  odmrp_header->seqno_ = seqno;
  odmrp_header->hop_count_ = 1;
  odmrp_header->src_ = IPAddress(src);
  odmrp_header->multicast_ = IPAddress(mcast);
  odmrp_header->prev_hop_addr_ = IPAddress(prev);
  odmrp_header->sender_addr_ = IPAddress(my_addr);
  
  num_con++;
  debug_msg("Node generates Join Reply : Total control pkt %d",num_con);
  seqno++;
  output(2).push(q);  
  
}




/*******************************************************************************************
 *                                                                                         *
 * Call generate join query function                                                       *
 *                                                                                         *
 *******************************************************************************************/
void
OdmrpAgent::run_JQ_timer(){
  generateJQ((IPAddress)memberTable->sender_mcast_addr());  
  return;
}


void 
OdmrpAgent::run_Forwarder_timer(){
	Timestamp t_temp,t1_temp;
	t_temp.set_now();
	t_temp -= last_JR;
	t1_temp.set(FORWARDER_EXPIRE,0);

	if(t1_temp < t_temp){
		memberTable->forwarder_expire();
	}
	return;
}


EXPORT_ELEMENT(OdmrpAgent)
