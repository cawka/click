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
 * input[0]:   IP packets with DTCAST protocol payload
 *
 * output[0]:  IP packets designated for broadcasting
 * output[1]:  IP packets designated to local delivery (to DtcastReceiver)
 * output[2]:  IP packets designated to locel source (DtcastSource)
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
	const char *port_count() const { return "1/3"; }
	const char *processing() const { return "h/h"; }

	virtual int initialize( ErrorHandler *errH );
	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet* );

protected:
	void onRouteRequest( DtcastRRPacket* );
	void onRouteReply( DtcastRTPacket* );
	void onData( DtcastDataPacket* );
	void onDataLocal( DtcastDataPacket *pkt, dtcast_fwd_tuple_t *fwd );
	void onAck( DtcastAckPacket* );
	void onImplicitAck( DtcastDataPacket* );
	void onERData( DtcastDataPacket* );
	void onERAck( DtcastAckPacket* );

	/**
	 * If IP header TTL field is greater than 0, then decrement TTL, set dtcast::_from field to _me and
	 * broadcast packet on BROADCAST port
	 *
	 * @param pkt   Packet to broadcast
	 */
	inline void broadcastPacket( DtcastPacket *pkt );
//	void onRefreshSRouting( Timer *timer );
//	void onRefreshForwarding( Timer *timer );

protected:
//	void run_timer( Timer * );
	
	enum {BROADCAST, RECEIVER, SOURCE};

private:
	node_t	_me;
	
	DtcastCacheTable		_cache;
	DtcastSRoutingTable		_source_routing;
	DtcastForwardingTable	_forwarding;
	
	bool					_activeAck; ///< if true, explicit ACK messages are used
										///< on receiving duplicate DATA we always
										///< use explicit ACKs
	
	Timer _refresher_source_routing;
	Timer _refresher_forwarding;
	Timer _refresher_cache;
};

void DtcastForwarder::broadcastPacket( DtcastPacket *pkt )
{
	if( pkt->ip_header()->ip_ttl>0 )
	{
		pkt->ip_header()->ip_ttl--;
		pkt->dtcast()->_from=_me;
		output( BROADCAST ).push( pkt );
	}
}

CLICK_ENDDECLS
#endif
