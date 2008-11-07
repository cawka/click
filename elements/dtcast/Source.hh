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
 * input[0]  DATA packets
 * input[1]  IP packets with DTCAST payload from DtcastForwarder
 * output[0] IP packets with DTCAST payload to DtcastForwarder
 *
 * =a
 * DtcastForwarder, DtcastReceiver
 */
class DtcastSource : public Element
{
public:
	DtcastSource( );
	
	const char *class_name() const { return "DtcastSource"; }
	const char *port_count() const { return "2/1"; }
	const char *processing() const { return "h/h"; }

	virtual int initialize( ErrorHandler* );
	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet *pkt );
	

// Forwarding interface
protected:
	void run_timer( Timer * );

	void onRouteReply( DtcastRTPacket *pkt );
	void onData( DtcastDataWithDstsPacket *pkt );
	void onAck( DtcastAckPacket *pkt );
	
private:
	node_t  _me;
	mcast_t _mcast;
	nodelist_t _dsts;
	
	Timestamp _age;
	
	uint32_t _seq_data;
	uint32_t _seq_rr;
	
	DtcastMessageQueue _queue;
	DtcastMessageQueue _fwd_queue; ///< I'm not sure why I have divided queues
	
	Timer _timer;
	Timestamp _lastRRSendBy;

	enum {DATA,FORWARDER};
};

CLICK_ENDDECLS
#endif
