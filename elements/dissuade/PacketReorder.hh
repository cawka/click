
#ifndef PACKETREORDER_HH_DISSUADE
#define PACKETREORDER_HH_DISSUADE

#include <click/element.hh>
#include <../elements/standard/simplequeue.hh>
#include <click/list.hh>

struct pkt_t
{
	Packet *_pkt;
	List_member<pkt_t> link;
	
	pkt_t( Packet *pkt ) : _pkt(pkt) { } 
	
	operator Packet*()
	{
		return _pkt;
	}
};

class PacketReorder : public SimpleQueue
{
public:
	PacketReorder();
	
	const char *class_name() const { return "PacketReorder"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return "h/l"; }
	
	int configure( Vector<String>&, ErrorHandler * );
	void push( int, Packet* );
	Packet* pull( );
	
private:
	void formOutput( );
	
private:
	uint32_t _probability;
	uint32_t _size;
	
	typedef Vector<Packet*> input_t;
	typedef List<pkt_t,&pkt_t::link> output_t;
	input_t _input_queue;
	output_t _output_queue;
	
};

#endif
