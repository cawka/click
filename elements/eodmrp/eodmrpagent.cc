/*
 * EodmrpAgent.{cc,hh} -- EodmrpAgent processing element
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
  Input : receive multicast packets	- eodmrp control packets & multicast data packets
  Output 0 : Discard
  Output 1 : Forwading packets -> encapsulate ethernet header
  Output 2 : Generate and send packets -> encapsulate UDP header
             -> encapsulate ethernet header
*/

#include <click/config.h>
#include <click/router.hh>
#include <click/elemfilter.hh>
#include "eodmrpagent.hh"
#include <click/error.hh>
#include <click/packet_anno.hh>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
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
  EodmrpAgent *e = (EodmrpAgent *)thunk;
  e->run_JQ_timer();
}


/*******************************************************************************************
 *                                                                                         *
 * Lost packet sequentially. Now start local recovery                                      *
 *                                                                                         *
 *******************************************************************************************/
static void
Data_hook(Timer *, void *thunk)
{
  EodmrpAgent *e = (EodmrpAgent *)thunk;
  e->run_Data_timer();
}


/*******************************************************************************************
 *                                                                                         *
 * Temporary Forwarder Timeout                            *
 *                                                                                         *
 *******************************************************************************************/
static void
timeout_hook(Timer *, void *thunk)
{
  EodmrpAgent *e = (EodmrpAgent *)thunk;
  e->run_timeout_timer();
}

  
/*******************************************************************************************
 *                                                                                         *
 * Constructor function. Define timers.                                                   *
 *                                                                                         *
 *******************************************************************************************/
EodmrpAgent::EodmrpAgent():
  JQ_timer_(JQ_hook, this),
  Data_timer_(Data_hook, this),
  timeout_timer_(timeout_hook, this)
{
}


/*******************************************************************************************
 *                                                                                         *
 * Destructor function. Do nothing                                                        *
 *                                                                                         *
 *******************************************************************************************/
EodmrpAgent::~EodmrpAgent()
{
}

/*******************************************************************************************
 *                                                                                         *
 * Initializing EodmrpAgent - initialize each timer and variables                          *
 * 							Set Join Query Timer 1 second and Data Timer 3 second                      *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpAgent::initialize(ErrorHandler *errh){
  // Initializing timers
  JQ_timer_.initialize(this);
  Data_timer_.initialize(this);
  timeout_timer_.initialize(this);
  send_con_pkt = 0;
  seqno = num_con = received_JQ = received_JR = received_LR = received_RR = 0;
  last_pkt.set(0, 0);
 
  // Send Join Query after 1 second and receiver join after 3 seconds
  JQ_timer_.schedule_after_sec(1);
  Data_timer_.schedule_after_sec(3);
  
   return 0;
}

/*******************************************************************************************
 *                                                                                         *
 * configure: get EodmrpAgent interface and EodmrpAgentForwardingTable element             *
 *                                                                                         *
 *******************************************************************************************/
int
EodmrpAgent::configure(Vector<String> &conf, ErrorHandler *errh)
{
  if (conf.size() < 3)
    return errh->error("wrong number of arguments; expected 'EodmrpAgent_SPT(EodmrpMemberTable EodmrpRoutingTable EodmrpAgent_SPT_interface)'");

  // get a Member Table element
  Element *e0 = cp_element(conf[0], this, errh);
  if (!e0) {
		return -1;
  }
  else {		
		memberTable = (EodmrpMemberTable *)e0->cast("EodmrpMemberTable");
		debug_msg("Get a Member Table");
  }

  // get a Routing Table element
  Element *e = cp_element(conf[1], this, errh);
  if (!e) {
		return -1;
  }
  else {		
		routingTable = (EodmrpRoutingTable *)e->cast("EodmrpRoutingTable");
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
		debug_msg("EodmrpAgent configured interface address: %d.%d.%d.%d", p[0], p[1], p[2], p[3]);
  }  
  
  return 0;
}


/*******************************************************************************************
 *                                                                                         *
 * Push - Receive packets and process them                                                 *
 *                                                                                         *
 *******************************************************************************************/
void 
EodmrpAgent::push(int port, Packet *p_in)
{
	// Change packet to writable packet
  WritablePacket *p = p_in->uniqueify();
  const click_ip *ip = reinterpret_cast<click_ip *>(p->data());
  // this is an udp packet
  const click_udp *udp = reinterpret_cast<click_udp *>(p->data() + sizeof(*ip));
  //debug_msg("EodmrpAgent receives a packet");
  EodmrpHeader *eodmrpheader;

  uint8_t type;
  uint16_t seqno;
  uint8_t hop;
  IPAddress src, mcast, prev, sender;
  //double pktinterval, refreshinterval;
  send_con_pkt = 0;
  
  //debug_msg("Sport %d and dport %d",ntohs(udp->uh_sport), ntohs(udp->uh_dport));

  // This is eodmrp control packet
  // Find a control packet using sending and destination port
  if(ntohs(udp->uh_sport) == 999 && ntohs(udp->uh_dport) == 998) {     
  	// process EodmrpAgent messages only  	
  	// get eodmrp header  	
    eodmrpheader=(EodmrpHeader *) (p->data() + sizeof(*ip)+sizeof(*udp)); 

    // get each values
    src = IPAddress(eodmrpheader->src_);					//multicast source address
    mcast = IPAddress(eodmrpheader->multicast_);	//multicast address
    prev = IPAddress(eodmrpheader->prev_hop_addr_);		//previous hop address
    sender = IPAddress(eodmrpheader->sender_addr_);		//packet sender address
    hop = (int)eodmrpheader->hop_count_;							//hop counting
    const unsigned char *pa = src.data();
    const unsigned char *pm = mcast.data();
    const unsigned char *pp = prev.data();
    const unsigned char *ps = sender.data();
    
    debug_msg("Source : %d.%d.%d.%d  Multicast: %d.%d.%d.%d  Prev Address: %d.%d.%d.%d Sender:%d.%d.%d.%d Hop : %d ",
    		pa[0], pa[1], pa[2], pa[3],pm[0], pm[1], pm[2], pm[3],pp[0], pp[1], pp[2], pp[3],ps[0],ps[1],ps[2],ps[3],hop);

		switch(eodmrpheader->type_) 
		  {
		  // Join Query Packet
		  case JQ:
		  	received_JQ++;
				debug_msg("EodmrpAgent : Join Query message arrived from %d.%d.%d.%d Total received JQ %d",
				 pp[0], pp[1], pp[2], pp[3], received_JQ);				
				// update routing table
				routingTable->addroute(mcast, prev, hop);
				//debug_msg("I am a %d",memberTable->returnmode(mcast));
				// if it is receiver, generate Join Reply
				if(memberTable->returnmode(mcast) < (uint8_t)FORWARDER_MODE){
					// update member table
					memberTable->set_refresh_interval(mcast, eodmrpheader->refresh_interval_);
					memberTable->update_last_refresh(mcast);
					// set last packet arrived time
					last_pkt.set_now();
					//gnerate Join Reply
				  generateJR(mcast, src, prev);					
				}
				// Modify eodmrp header and rebroadcast
				eodmrpheader->prev_hop_addr_ = IPAddress(my_addr);
				eodmrpheader->hop_count_++;
				//num_con++;
				output(1).push(p);  
				break;
		  // Join Reply Packet
		  case JR:				
				// check destination - was this sent to me?
				if(my_addr == IPAddress(eodmrpheader->prev_hop_addr_)){
					received_JR++;
					debug_msg("EodmrpAgent : Join Reply message arrived from %d.%d.%d.%d Total received JR %d",
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
				  // modify eodmrp header
				  //eodmrpheader->sender_addr_ = IPAddress(my_addr);
				  eodmrpheader->hop_count_++;								//increase hop count
				  prev = routingTable->findprevhop(mcast);	//find a next node towarder a source
				  
				  // if no previous hop or it is a source node, discard JR packets
				  if(prev == (IPAddress)0 || memberTable->returnmode(mcast) == (uint8_t)SOURCE_MODE){
				    output(0).push(p);
				  }else{				  	
				    eodmrpheader->prev_hop_addr_ = prev;				    
				    //num_con++;
				    output(1).push(p);  
				  }
				  // it is a join reply packet, but it is not for me
				}else{
				  output(0).push(p);  
			       }		
				break;
		  // Local Recovery Packet
		  case LR:
		  	received_LR++;
				debug_msg("EodmrpAgent : Local Recovery message arrived from %d.%d.%d.%d Total received JR %d",
				 pa[0], pa[1], pa[2], pa[3], received_LR);
				
				if(memberTable->returnmode(mcast) == (uint8_t)LISTENER_MODE){
					// i am a listener node and becoming a temporary forwarder
					memberTable->updatemode(mcast,TEMPFORWARDER_MODE);
					temp_mcast = mcast;			// save multicast address
					//timeout_timer_.schedule_after_sec(1);		// forwarding packets next 1 second
				}
				// discard a packet
				output(0).push(p);
				break;
		  // Route Refresh Packet
		  case RR:
		  	received_RR++;
				debug_msg("EodmrpAgent : Route Refresh message arrived from %d.%d.%d.%d Total received RR %d",
				 pa[0], pa[1], pa[2], pa[3], received_RR);
												
				if(memberTable->returnmode(mcast) != (uint8_t)SOURCE_MODE){
					// if i am not a source, just rebroadcasting
					//num_con++;
					output(1).push(p); 
				}else if(memberTable->returnmode(mcast) == (uint8_t)SOURCE_MODE){
					// I am a source and now refresh a route (broadcast JQ)
					generateJQ(mcast);
		    	output(0).push(p);	
				}
				break;
		  // Dummy Packet
		  case DUMMY:	
		  	// it is not completed. Now just drop a packet
				debug_msg("EodmrpAgent : Dummy message arrived from %d.%d.%d.%d", pa[0], pa[1], pa[2], pa[3]);
		    output(0).push(p);	
				break;
		  
		  default:
		  	// it is not a eodmrp packet
		  	debug_msg("EodmrpAgent does not know this type of message! It came from %d.%d.%d.%d",pa[0], pa[1], pa[2], pa[3]);
				output(0).push(p);
		  }		  
  }
  else {
		//debug_msg("It is not an Eodmrp Control Message");
		// This is not an eodmrp control packet
		// get source and multicast address and copy packet for forwarding
  	src = IPAddress(ip->ip_src);
  	mcast = IPAddress(ip->ip_dst);
  	Packet *p = p_in->clone();
		
		// This is a forwarder or receiver
		if(memberTable->returnmode(mcast) < LISTENER_MODE){
			// update member table and forward it
			memberTable->updatereceivedtime(mcast);
			// set last packet arrived time
			last_pkt.set_now();
    	output(1).push(p);  
		}else{
			// this is a listener or nothing
	  	if(memberTable->returnmode(mcast)> LISTENER_MODE){
	  		// This is not a listener and now becomes a listener
	    	memberTable->updatemode(mcast,LISTENER_MODE);
	    }
	    // update packet received time
	    memberTable->updatereceivedtime(mcast);
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
EodmrpAgent::generateJQ(IPAddress mcast){
	// If a node is not source, return	
  if(memberTable->is_sender() != EODMRP_SUCCESS){  	
  	return;
  }
  
  // get time value
  /*char timebuff[10];
  struct tm newtime;
	time_t t;
	time(&t);
	strftime(timebuff, sizeof(timebuff), "%H:%M:%S",localtime(&t));*/
  
  // re-calculate refresh interval and update last refresh time
  memberTable->update_refresh_interval(mcast);  
  
  // declare each header
  EodmrpHeader *eodmrp_header;
  WritablePacket *q;
  
  // generate a packet
	q = Packet::make(sizeof(*eodmrp_header));		  
  eodmrp_header = (EodmrpHeader *)((char *)q->data());
 	
 	// create eodmrp header
  eodmrp_header->type_ = JQ; 
  eodmrp_header->seqno_ = seqno;
  eodmrp_header->hop_count_ = 1;
  eodmrp_header->src_ = IPAddress(my_addr);
  eodmrp_header->multicast_ = IPAddress(mcast);
  eodmrp_header->prev_hop_addr_ = IPAddress(my_addr);
  eodmrp_header->sender_addr_ = IPAddress(my_addr);
  eodmrp_header->datasendinginterval_ = memberTable->return_pkt_arrived_interval(mcast);
  eodmrp_header->refresh_interval_ = memberTable->return_refresh_interval(mcast); 
  
  num_con++;
  debug_msg("Node generates Join Query : Refresh Interval %d Total control pkt %d",
  memberTable->return_refresh_interval(mcast), num_con);
  // increase sequence number
  seqno++;
  //debug_msg("Size of JQ is %d, and size of eodmrp %d",sizeof(*q), sizeof(*eodmrp_header));
  //debug_msg("refresh interval %d, %f",memberTable->return_refresh_interval(mcast),
	//	  memberTable->return_pkt_arrived_interval(mcast));
	// send a Join Query packet
  output(2).push(q);  
  
  // set timer
  JQ_timer_.schedule_after_sec(memberTable->return_refresh_interval(mcast));
}


/*******************************************************************************************
 *                                                                                         *
 * Generate Join Reply packet                                                              *
 *                                                                                         *
 *******************************************************************************************/
void 
EodmrpAgent::generateJR(IPAddress mcast, IPAddress src, IPAddress prev){
   // declare each header
  EodmrpHeader *eodmrp_header;
  WritablePacket *q;
  
  // generate a packet
	q = Packet::make(sizeof(*eodmrp_header));		  
  eodmrp_header = (EodmrpHeader *)((char *)q->data());
 	
 	// create eodmrp header
  eodmrp_header->type_ = JR; 
  eodmrp_header->seqno_ = seqno;
  eodmrp_header->hop_count_ = 1;
  eodmrp_header->src_ = IPAddress(src);
  eodmrp_header->multicast_ = IPAddress(mcast);
  eodmrp_header->prev_hop_addr_ = IPAddress(prev);
  eodmrp_header->sender_addr_ = IPAddress(my_addr);
  eodmrp_header->datasendinginterval_ = 0.0;
  eodmrp_header->refresh_interval_ = 0; 
  
  num_con++;
  debug_msg("Node generates Join Reply : Total control pkt %d",num_con);
  seqno++;
  output(2).push(q);  
  
}


/*******************************************************************************************
 *                                                                                         *
 * Generate Local Recovery or Route Refresh packet                                         *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpAgent::generateLR(IPAddress mcast){  
   
  IPAddress src,prev;
  // it should be multicast source address
  src = (IPAddress)my_addr;  
  prev = routingTable->findprevhop(mcast);

  // declare each header
  EodmrpHeader *eodmrp_header;
  WritablePacket *q;
  
  // generate a packet
	q = Packet::make(sizeof(*eodmrp_header));		  
  eodmrp_header = (EodmrpHeader *)((char *)q->data());
  
  // deciding local recovery or refresh request
	if(send_con_pkt > 1){
		eodmrp_header->type_ = RR; 
	}else{
  	eodmrp_header->type_ = LR; 
	}
	// create eodmrp header
  eodmrp_header->seqno_ = seqno;
  eodmrp_header->hop_count_ = 1;
  eodmrp_header->src_ = IPAddress(src);
  eodmrp_header->multicast_ = IPAddress(mcast);
  eodmrp_header->prev_hop_addr_ = IPAddress(prev);
  eodmrp_header->sender_addr_ = IPAddress(my_addr);
  eodmrp_header->datasendinginterval_ = 0.0;
  eodmrp_header->refresh_interval_ = 0; 
  
  num_con++;
  debug_msg("Node generates Local Recovery : Total control pkt %d",num_con);
  // increase sequence number and the number of recovery tries
  seqno++;
  send_con_pkt++;
  output(2).push(q);
  
}


/*******************************************************************************************
 *                                                                                         *
 * Call generate join query function                                                       *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpAgent::run_JQ_timer(){
  generateJQ((IPAddress)memberTable->sender_mcast_addr());  
  return;
}


/*******************************************************************************************
 *                                                                                         *
 * Call recovery function and set timer                                                    *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpAgent::run_Data_timer(){
	// This is not receiver or forwarder so it doesn't need local recovery 	
	if(memberTable->is_receiver() != EODMRP_SUCCESS) {
		Data_timer_.schedule_after_sec(1);
		return;
	}
	
	// set mcast address and timer
	IPAddress mcast;
	Timestamp t_temp, t1_temp;
	t_temp.set(0, 0);		
	mcast=memberTable->receiver_mcast_addr();
	
	// No data has been transmitted yet
	if(last_pkt == t_temp){
		Data_timer_.schedule_after_sec(1);
		return;
	}
	
	// check packet loss
	t_temp.set_now();
	t_temp -= last_pkt;
	t1_temp.set(1,0);
	if(t1_temp < t_temp){
		if(send_con_pkt < 10){
			// send recovery packet			
			generateLR(mcast);
		}		
	}
	
	// set timer
	Data_timer_.schedule_after_sec(1);
  
  return;
}


/*******************************************************************************************
 *                                                                                         *
 * Temporary Forwarder goes back to Listener                                               *
 *                                                                                         *
 *******************************************************************************************/
void
EodmrpAgent::run_timeout_timer(){
	if(memberTable->returnmode(temp_mcast) == (uint8_t)TEMPFORWARDER_MODE){
		memberTable->updatemode(temp_mcast, LISTENER_MODE);	
	}
  return;
}

EXPORT_ELEMENT(EodmrpAgent)
