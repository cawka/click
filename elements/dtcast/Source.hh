/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef SOURCE_HH_DTCAST
#define SOURCE_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

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
 * DtcastForwarder, DtcastReceiver
 */
class DtcastSource : public Element
{
public:
//	DtcastForwarder( );
//	~DtcastForwarder( );

	const char *class_name() const { return "DtcastSource"; }
	const char *port_count() const { return PORTS_0_0; }

	int configure( Vector<String>&, ErrorHandler* );
//	Packet* simple_action( Packet* );
//	void push( int port, Packet* );
//	Packet *pull( int port );

// Forwarding interface
protected:

private:
};

CLICK_ENDDECLS
#endif
