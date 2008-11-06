/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef SOURCE_HH_DTCAST
#define SOURCE_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

CLICK_DECLS

/*
 * =c DtcastSource( NODE : <integer> )
 *
 * =s DTCAST
 * DTCAST protocol source node functionality
 * 
 * =d
 *
 * Input should be any IP packets
 *
 * Source performs encapsulating IP packets into DTCAST protocol packets and
 * initiates data delivery
 *
 * =a
 * DtcastForwarder, DtcastReceiver
 */
class DtcastSource : public Element
{
public:
	DtcastSource( );
	
	const char *class_name() const { return "DtcastSource"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return "h/h"; }

	virtual int initialize( ErrorHandler* );
	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet *pkt );

// Forwarding interface
protected:
	void run_timer( Timer * );

private:
	node_t  _me;
	mcast_t _mcast;
	nodelist_t _dsts;
	
	Timestamp _age;
	
	uint32_t _seq_data;
	uint32_t _seq_rr;
	
	DtcastMessageQueue _queue;
	
	Timer _timer;
	Timestamp _lastRRSendBy;
};

CLICK_ENDDECLS
#endif
