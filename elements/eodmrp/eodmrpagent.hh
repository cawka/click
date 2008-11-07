#ifndef EodmrpAgent_HH
#define EodmrpAgent_HH

#include "eodmrp_defs.hh"
#include "eodmrpmemtable.hh"
#include "eodmrproutetable.hh"
#include <click/timer.hh>
#include <click/element.hh>
#include <click/timestamp.hh>

/*
=c
EodmrpAgent(EodmrpMemberTable, EodmrpRoutingTable, IPAddress)

=s
Eodmrp - Ad Hoc multicast routing protocol

=d
Handle Eodmrp control packets and multicasting data packets.
Generate Join Query, Join Reply, Local Recovery, and Refresh Request packets.

=e
CheckDupPkt[0] -> EodmrpAgent(EodmrpMemberTable, EodmrpRoutingTable, IPAddress);

=a
EodmrpMemberTable, EodmrpRoutingTable, CheckDupPkt
*/

class EodmrpAgent : public Element { 
public:
  
  EodmrpAgent();
  ~EodmrpAgent();

  EodmrpMemberTable *memberTable;
  EodmrpRoutingTable *routingTable;

  const char *class_name() const	{ return "EodmrpAgent"; }
  const char *port_count() const  	{ return "1/3"; }
  const char *processing() const	{ return "h/hhh"; }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  void push(int port, Packet *);
  void generateJR(IPAddress mcast, IPAddress src, IPAddress prev);
  void generateJQ(IPAddress mcast);
  void generateLR(IPAddress mcast);

  void run_JQ_timer();
  void run_Data_timer();
  void run_timeout_timer();

private:
  IPAddress my_addr;
  IPAddress temp_mcast;
  int send_con_pkt;
  uint16_t seqno;
  Timestamp last_pkt;
  int num_con, received_JQ, received_JR, received_LR, received_RR;

  Timer JQ_timer_;
  Timer Data_timer_;
  Timer timeout_timer_;
  //unsigned short calculate_checksum(const unsigned char*, int);
};

#endif
