/**
    @file Definition of the FORWARDING NODE in DTCAST Network
*/

#ifndef RECEIVER_HH_DTCAST
#define RECEIVER_HH_DTCAST

#include <click/element.hh>
#include "dtcast.hh"

CLICK_DECLS

/*
 * =c DtcastReceiver( NODE : <integer>, MCAST : <integer> )
 *
 * =s DTCAST
 * DTCAST protocol receiver functionality.
 * 
 * =d
 *
 * Input should be IP packets with protocol IP_PROTO_DTCAST (138).
 * All non DTCAST packets will be discarded.
 *
 * Receiver has NODE identificator and designated to receive MCAST stream
 *
 * =a
 * DtcastForwarder, DtcastSource
 */
class DtcastReceiver : public Element
{
public:
//	DtcastForwarder( );
//	~DtcastForwarder( );

	const char *class_name() const { return "DtcastReceiver"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return "h/h"; }

	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet *pkt );

private:
	node_t  _me;
	mcast_t _mcast;
};

CLICK_ENDDECLS
#endif
