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
	const char *processing() const { return "h/hl"; }

	int configure( Vector<String>&, ErrorHandler* );
	Packet* simple_action( Packet* );
	void push( int port, Packet* );
	Packet *pull( int port );

// Forwarding interface
protected:

private:
	DtcastSource			*_source;
	DtcastReceiver			*_receiver;
	
	DtcastCacheTable		_cache;
	DtcastSRoutingTable		_source_routing;
	DtcastForwardingTable	_forwarding;
};

CLICK_ENDDECLS
#endif
