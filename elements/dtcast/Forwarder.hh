/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef FORWARDER_HH_DTCAST
#define FORWARDER_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

CLICK_DECLS
        
class DtcastForwarder : public Element
{
public:
	DtcastForwarder( );
//	~DtcastForwarder(){}

	const char *class_name() const { return "DtcastForwarder"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	int configure( Vector<String>&, ErrorHandler* );
	Packet* simple_action( Packet* );

// Forwarding interface
	
protected:

};

CLICK_ENDDECLS
#endif
