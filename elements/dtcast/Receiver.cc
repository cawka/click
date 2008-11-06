/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>

#include "Receiver.hh"
CLICK_DECLS



int DtcastReceiver::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"NODE",  cpkPositional, cpInteger, &_me,
			"MCAST", cpkPositional, cpInteger, &_mcast,
					cpEnd );
}


void DtcastReceiver::push( int, Packet *pkt )
{
	/**
	 * Just to make sure, that we have received DTCAST data packet
	 */
	DtcastPacket *dpkt=DtcastPacket::make( pkt );
	if( dpkt==NULL ) return;

	DtcastDataPacket *ddpkt=DtcastDataPacket::make( dpkt );
	if( ddpkt==NULL ) return;
	
	output( 0 ).push( ddpkt->data_payload() );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastReceiver)
