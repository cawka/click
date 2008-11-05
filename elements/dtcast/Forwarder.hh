/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef FORWARDER_HH_DTCAST
#define FORWARDER_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

#include "Source.hh"
#include "Receiver.hh"

CLICK_DECLS

/*
 * =c DtcastForwarder( SOURCE : DtcastSource, DESTINATION : DtcastReceiver )
 *
 * =s DTCAST
 * DTCAST protocol forwarding functionality.
 * 
 * =d
 *
 * Input should be IP packets with protocol IP_PROTO_DTCAST (138).
 * All non DTCAST packets will be discarded.
 *
 * =a
 * DtcastSource, DtcastReceiver
 */
class DtcastForwarder : public Element
{
public:
	DtcastForwarder( );
	~DtcastForwarder( );

	const char *class_name() const { return "DtcastForwarder"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return "h/h"; }

	virtual int initialize( ErrorHandler *errH );
	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet* );

// Forwarding interface
protected:
	void onRouteRequest( DtcastRRPacket* );
	void onRouteReply( DtcastRTPacket* );
	void onData( DtcastDataPacket* );
	void onAck( DtcastAckPacket* );
	void onERData( DtcastDataPacket* );
	void onERAck( DtcastAckPacket* );
	
private:
	DtcastSource			*_source;
	DtcastReceiver			*_receiver;
	
	DtcastCacheTable		_cache;
	DtcastSRoutingTable		_source_routing;
	DtcastForwardingTable	_forwarding;
	
	Timer _refresher_source_routing;
	Timer _refresher_forwarding;
};

CLICK_ENDDECLS
#endif
