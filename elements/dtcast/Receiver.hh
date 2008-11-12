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
 * input[0]  IP packets with DTCAST payload from DtcastForwarder
 * output[0]  IP packets with DTCAST payload to DtcastForwarder
 * output[1]  DATA packets
 *
 * =a
 * DtcastForwarder, DtcastSource
 */
class DtcastReceiver : public Element
{
public:
	DtcastReceiver( )
			: _seq( 0 ) 
			, _lastRRorData( 0,0 )
			, _localRecoveryTimer( this )
	{ }

	const char *class_name() const { return "DtcastReceiver"; }
	const char *port_count() const { return "1/1-2"; }
	const char *processing() const { return "h/h"; }

	virtual int initialize( ErrorHandler *errH );
	int configure( Vector<String>&, ErrorHandler* );
	void push( int port, Packet *pkt );

protected:	
	void onDataPacket( DtcastDataPacket *pkt );
//	void onErDataPacket( DtcastDataPacket *pkt );
	void onRouteRequest( DtcastRRPacket *pkt );
	
	void onLocalRecovery( Timer *timer );
	
private:
	void run_timer( Timer *timer );
	void scheduleLocalRecovery( DtcastPacket *pkt );

private:
	node_t  _me;
	mcast_t _mcast;
	
	uint32_t _seq;
	Timestamp _lastRRorData; ///< last time we have seen RouteRequest or DataPacket
							 ///< if NOW()-_lastRRorData > THRESHOLD, then exit localRecovery state
	node_t   _lastSrc;		 ///< Source node_id from which we received last data or RT packet
	
	Timer     _localRecoveryTimer;
	
	enum{ FORWARDER, DATA };
};

CLICK_ENDDECLS
#endif
