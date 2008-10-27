#ifndef CHECK_DUPLICATED_PACKETS_HH 
#define CHECK_DUPLICATED_PACKETS_HH 

#include "odmrp_defs.hh"
#include <click/element.hh>
#include <click/glue.hh>
#include <click/packet_anno.hh>
#include <clicknet/ip.h>
#include <click/ipaddress.hh>

/*
=c
CheckDupPkt(IP Address)

=s
Check duplicated packets

=d
Check duplicated packets to drop duplicated packets. It uses source and multicast IP address
and IP sequence number to check duplicated mulitcast packets. For odmrp control packets,
it uses sender and multicast IP address and odmrp sequence number.

=e
CheckDupPkt[0] -> OdmrpAgent(OdmrpMemberTable, OdmrpRoutingTable, IPAddress);
CheckDupPkt[1] -> Discard

=a
OdmrpAgent, OdmrpMemberTable, OdmrpRoutingTable
*/

#define MSG_CACHE_SIZE 500
#define CON_CACHE_SIZE 50

struct msg_cache_slot{
  IPAddress src;
  IPAddress group;
  uint16_t pid;
};

struct con_cache_slot{
  IPAddress src;
  IPAddress group;
  uint16_t pid;
};

struct temp_pack{
	uint16_t seq_no;
	char buf[1008];
};
  
class CheckDupPkt : public Element { public:
  
  CheckDupPkt();
  ~CheckDupPkt();

  
  const char *class_name() const	{ return "CheckDupPkt"; }
  const char *port_count() const  	{ return "1/2"; }
  const char *processing() const	{ return "h/hh"; }


  int configure(Vector<String> &, ErrorHandler *);

  void push(int port, Packet *);
  
  int lookup(IPAddress s, IPAddress m, uint16_t id);
  void enterInCache(IPAddress s, IPAddress m, uint16_t id);
  int lookup_con(IPAddress s, IPAddress m, uint16_t id);
  void enterInCache_con(IPAddress s, IPAddress m, uint16_t id);
  
private:
  struct msg_cache_slot msg_cache[MSG_CACHE_SIZE];
  struct con_cache_slot con_cache[CON_CACHE_SIZE];
  int d_index, c_index;
  IPAddress my_addr;
  
};

#endif
