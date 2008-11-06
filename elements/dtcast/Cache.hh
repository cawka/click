/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef CACHE_HH_DTCAST
#define CACHE_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

CLICK_DECLS

/*
 * =c DtcastCache
 *
 * =s DTCAST
 * DTCAST protocol packet caching
 * 
 * =d
 *
 * Input should be IP packets with protocol IP_PROTO_DTCAST (138).
 * All non DTCAST packets will be discarded.
 *
 * =a
 * DtcastForwarder
 */
class DtcastCache : public Element, public DtcastCacheTable
{
public:
	const char *class_name() const { return "DtcastCache"; }
	const char *port_count() const { return "-/1"; }
	const char *processing() const { return AGNOSTIC; }

	Packet* simple_action( Packet *pkt );
};

CLICK_ENDDECLS
#endif
