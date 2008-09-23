/**
	@file	
*/

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include <clicknet/ip.h>
#include <clicknet/tcp.h>
#include <clicknet/udp.h>

#include "dropper.hh"
CLICK_DECLS

#ifdef CLICK_LINUXMODULE
//MODULE_LICENSE("GPL");
#endif


Dropper::Dropper( )
: _tcpDrop( 0.0 )
, _tcpReset( 0.0 )
, _tcpLongReset( 0.0 )
, _tcpShortReset( 0.0 )

, _tcpSlidingWnd( 0.0 )
, _tcpScaling( 0.0 )
, _tcpForgeAck( 0.0 )

, _tcpSeqDrop( 0.0 )
{
	
}

int Dropper::configure( Vector<String> &conf, ErrorHandler *errH )
{
#ifndef CLICK_LINUXMODULE
	srand( time(NULL) );
#endif
	
	return cp_va_kparse( conf,this,errH,
					"TCP_DROP",  cpkPositional, cpInteger, &_tcpDrop,
					"TCP_RESET", cpkPositional, cpInteger, &_tcpReset,
					"TCP_LONG_RESET",  cpkPositional, cpInteger, &_tcpLongReset,
					"TCP_SHORT_RESET", cpkPositional, cpInteger, &_tcpShortReset,
					"TCP_SLIDING_WND", cpkPositional, cpInteger, &_tcpSlidingWnd,
					"TCP_SCALING",	   cpkPositional, cpInteger, &_tcpScaling,
					"TCP_FORGE_ACK",   cpkPositional, cpInteger, &_tcpForgeAck,
					"TCP_SEQ_DROP",	   cpkPositional, cpInteger, &_tcpSeqDrop,
					cpEnd );
}

Packet* Dropper::simple_action( Packet *pkt )
{
	if( rand()<(1-_tcpDrop) )
		return processTcpDrop( pkt );
	else if( rand()<(1-_tcpReset) )
		return processTcpReset( pkt );
	else
/// @todo	Realize all other attacks	
		return pkt;
}

Packet* Dropper::processTcpDrop( Packet *pkt )
{
	pkt->kill( );
	return NULL;
}

Packet* Dropper::processTcpReset( Packet *pkt )
{
	// Create packet with FIN flag set and same src/dst ip/port and sequence numbers
	WritablePacket *p=pkt->uniqueify();
	click_tcp *tcph=p->tcp_header();
	tcph->th_flags=TH_FIN;
	tcph->th_off=0;			//discard any additional header
	
	p->take( pkt->length()-16/*TCP header*/-8/*IP Header*/ ); // discard all usefull data
	
	pkt->kill();
	return p;
}


CLICK_ENDDECLS
//ELEMENT_MT_SAFE(Dropper)

EXPORT_ELEMENT(Dropper)
ELEMENT_REQUIRES(linuxmodule)
