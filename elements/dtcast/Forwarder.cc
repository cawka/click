/**
 *	Definition of the FORWARDING NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "Forwarder.hh"
CLICK_DECLS

DtcastForwarder::DtcastForwarder()
		: _source(NULL), _receiver(NULL)
{
}

DtcastForwarder::~DtcastForwarder( )
{
}

int DtcastForwarder::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"DTCAST_SOURCE",   cpkPositional, cpElement, &_source,
			"DTCAST_RECEIVER", cpkPositional, cpElement, &_receiver,
//					"TCP_DROP",  cpkPositional, cpInteger, &_tcpDrop,
//					"TCP_RESET", cpkPositional, cpInteger, &_tcpReset,
//					"TCP_LONG_RESET",  cpkPositional, cpInteger, &_tcpLongReset,
//					"TCP_SHORT_RESET", cpkPositional, cpInteger, &_tcpShortReset,
//					"TCP_SLIDING_WND", cpkPositional, cpInteger, &_tcpSlidingWnd,
//					"TCP_SCALING",	   cpkPositional, cpInteger, &_tcpScaling,
//					"TCP_FORGE_ACK",   cpkPositional, cpInteger, &_tcpForgeAck,
//					"TCP_SEQ_DROP",	   cpkPositional, cpInteger, &_tcpSeqDrop,
					cpEnd );
}

Packet* DtcastForwarder::simple_action( Packet *pkt )
{
	//pkt should containt valid IP packet with protocol field set to IP_PROTO_DTCAST
	DtcastPacket *dtcast=DtcastPacket::make( pkt );
	if( dtcast==NULL )
	{
		pkt->kill( );
		return NULL;
	}
	
	return NULL;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastForwarder)


		