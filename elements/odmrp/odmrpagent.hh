#ifndef OdmrpAgent_HH
#define odmrpAgent_HH

#include "odmrp_defs.hh"
#include "odmrpmemtable.hh"
#include "odmrproutetable.hh"
#include <click/timer.hh>
#include <click/element.hh>
#include <click/timestamp.hh>

/*
=c
OdmrpAgent(OdmrpMemberTable, OdmrpRoutingTable, IPAddress)

=s
Odmrp - Ad Hoc multicast routing protocol

=d
Handle Odmrp control packets and multicasting data packets.
Generate Join Query, Join Reply, Local Recovery, and Refresh Request packets.

=e
CheckDupPkt[0] -> OdmrpAgent(OdmrpMemberTable, OdmrpRoutingTable, IPAddress);

=a
OdmrpMemberTable, OdmrpRoutingTable, CheckDupPkt
*/

class OdmrpAgent : public Element { 
public:
  
  OdmrpAgent();
  ~OdmrpAgent();

  OdmrpMemberTable *memberTable;
  OdmrpRoutingTable *routingTable;

  const char *class_name() const	{ return "OdmrpAgent"; }
  const char *port_count() const  	{ return "1/3"; }
  const char *processing() const	{ return "h/hhh"; }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  void push(int port, Packet *);
  void generateJR(IPAddress mcast, IPAddress src, IPAddress prev);
  void generateJQ(IPAddress mcast);
  //void generateLR(IPAddress mcast);

  void run_JQ_timer();
  void run_Forwarder_timer();
  //void run_timeout_timer();

private:
  IPAddress my_addr;
  IPAddress temp_mcast;
  int send_con_pkt;
  uint16_t seqno;
  Timestamp last_JR;
  int num_con;
  int received_JQ, received_JR;

  Timer JQ_timer_;
  Timer Forwarder_timer_;
  //Timer timeout_timer_;
  //unsigned short calculate_checksum(const unsigned char*, int);
};

#endif
