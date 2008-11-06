/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef PRINT_HH_DTCAST
#define PRINT_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

CLICK_DECLS

/*
 * =c DtcastPrint( LABEL : <string> )
 *
 * =s DTCAST
 * Print DTCAST protocol packets in human readable format with LABEL prefix
 * 
 * =d
 *
 * Input should be IP packets with protocol IP_PROTO_DTCAST (138).
 * All non DTCAST packets will be discarded.
 *
 * =a
 * DtcastForwarder
 */
class DtcastPrint : public Element
{
public:
	const char *class_name() const { return "DtcastPrint"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }

	int configure( Vector<String>&, ErrorHandler* );
	Packet* simple_action( Packet* );
	
private:
	String _label;
};

CLICK_ENDDECLS
#endif
