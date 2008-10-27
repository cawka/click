#ifndef ODMRPMEMTABLE_HH
#define ODMRPMEMTABLE_HH
CLICK_DECLS
#include "odmrp_defs.hh"
#include <click/element.hh>
#include <click/timestamp.hh>
#include <click/hashmap.hh>

/*
=c
ODMRPMemTable(MODE, MULTICAST IPAddress)

=s
ODMRP Member Table

=d
Store Odmrp member information such as multicast address, mode, packet arrived time,
last refresh packet arrived time, and refresh interval

=e
The member table is passed to Odmrp module as an argument

=a
OdmrpAgent, OdmrpRoutingTable, CheckDupPkt
*/


struct memberTable{
	IPAddress mcast_addr;	
	MODE mode;
	struct memberTable *next;
};

typedef memberTable memberTable;
	
class OdmrpMemberTable : public Element {
 public:

  OdmrpMemberTable();
  ~OdmrpMemberTable();

  const char *class_name() const	{ return "OdmrpMemberTable"; }
  const char *port_count() const	{ return PORTS_0_0; }
  //const char *processing() const	{ return "h/h"; }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  int addgroup(IPAddress mcast, MODE m);
  int removergroup(IPAddress mcast);
  int updatemode(IPAddress mcast, MODE m);  
  uint8_t returnmode(IPAddress mcast);
  int is_sender();
  int is_receiver();
  IPAddress sender_mcast_addr();
  IPAddress receiver_mcast_addr();
  void forwarder_expire();
  
private:
  //Vector<memberTable> memtable;
  memberTable *head;
  memberTable *tail;
};

CLICK_ENDDECLS
#endif
