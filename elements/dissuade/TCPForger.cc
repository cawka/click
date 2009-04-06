
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/glue.hh>

#include "TCPForger.hh"
#include "clicknet/tcp.h"

TCPForger::TCPForger( )
		: _ackProbability( CLICK_RAND_MAX )
{
	
}

int TCPForger::configure( Vector<String> &conf, ErrorHandler *errH )
{
	double prob;
	return cp_va_kparse( conf,this,errH,
					"ACK_PROB",  cpkPositional, cpDouble, &prob,
					cpEnd );
	_ackProbability=prob * CLICK_RAND_MAX;
}

// Input should be IP TCP packets
void TCPForger::push( int, Packet *pkt )
{
	const click_tcp *ip=pkt->tcp_header( );
	if( ip->th_flags&TH_ACK && click_random()>_ackProbability )
	{
		output(0).push( pkt->clone() );
		output(0).push( pkt->clone() );
		output(0).push( pkt->clone() );
		output(0).push( pkt );
	}
}

EXPORT_ELEMENT(TCPForger)
