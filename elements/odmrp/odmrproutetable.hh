#ifndef ODMRPROUTETABLE_HH
#define ODMRPROUTETABLE_HH
CLICK_DECLS
#include "odmrp_defs.hh"
#include <click/element.hh>
#include <click/vector.hh>

/*
=c
OdmrpRoutingTable()

=s
ODMRP Routing Table

=d
Store Odmrp routing information such as multicast, and previous hop IP addresses and hop count

=e
The routing table is passed to Odmrp module as an argument

=a
OdmrpAgent, OdmrpMemberTable, CheckDupPkt
*/


struct routingTable{
	IPAddress mcast_addr;
	IPAddress prevhop;	
	int hop_count;
	struct routingTable *next;
	};
	
typedef routingTable routingTable;

class OdmrpRoutingTable : public Element {
 public:
 
  OdmrpRoutingTable();
  ~OdmrpRoutingTable();

  const char *class_name() const	{ return "OdmrpRoutingTable"; }
  const char *port_count() const	{ return PORTS_0_0; }
  //const char *processing() const	{ return "h/h"; }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  int addroute(IPAddress mcast, IPAddress prev, int h);
  int removeroute(IPAddress mcast);
  int setprevhop(IPAddress mcast, IPAddress prev);
  int updateroute(IPAddress mcast, IPAddress prev,int h);
  int findhop(IPAddress mcast);
  IPAddress findprevhop(IPAddress mcast);
  int findRoute(IPAddress mcast);
  
private:
  routingTable *head;
  routingTable *tail;
};

CLICK_ENDDECLS
#endif
