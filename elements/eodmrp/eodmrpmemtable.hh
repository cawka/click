#ifndef EODMRPMEMTABLE_HH
#define EODMRPMEMTABLE_HH
CLICK_DECLS
#include "eodmrp_defs.hh"
#include <click/element.hh>
#include <click/timestamp.hh>
#include <click/hashmap.hh>

/*
=c
EODMRPMemTable(MODE, MULTICAST IPAddress)

=s
EODMRP Member Table

=d
Store Eodmrp member information such as multicast address, mode, packet arrived time,
last refresh packet arrived time, and refresh interval

=e
The member table is passed to Eodmrp module as an argument

=a
EodmrpAgent, EodmrpRoutingTable, CheckDupPkt
*/

// deifne valudes
#define MAXIMUM_REFRESH_INTERVAL 30
#define MINIMUM_REFRESH_INTERVAL 3
#define DEFAULT_PKT_ARRIVED_INTERVAL 0.1

struct memberTable{
	IPAddress mcast_addr;	
	MODE mode;
	double pkt_arrived_interval;
	Timestamp last_pkt_received;
	int refresh_interval;
	Timestamp last_refresh;
	struct memberTable *next;
};

typedef memberTable memberTable;
	
class EodmrpMemberTable : public Element {
 public:

  EodmrpMemberTable();
  ~EodmrpMemberTable();

  const char *class_name() const	{ return "EodmrpMemberTable"; }
  const char *port_count() const	{ return PORTS_0_0; }
  //const char *processing() const	{ return "h/h"; }

  int configure(Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);

  int addgroup(IPAddress mcast, MODE m, double interval);
  int removergroup(IPAddress mcast);
  int updatemode(IPAddress mcast, MODE m);
  int updatereceivedtime(IPAddress mcast);
  Timestamp returnreceivedtime(IPAddress mcast);
  uint8_t returnmode(IPAddress mcast);
  int is_sender();
  int is_receiver();
  IPAddress sender_mcast_addr();
  IPAddress receiver_mcast_addr();
  void update_refresh_interval(IPAddress mcast);
  void update_last_refresh(IPAddress mcast);
  int set_refresh_interval(IPAddress mcast, int t);
  int return_refresh_interval(IPAddress mcast);
  double return_pkt_arrived_interval(IPAddress mcast);
  
private:
  //Vector<memberTable> memtable;
  memberTable *head;
  memberTable *tail;
};

CLICK_ENDDECLS
#endif
