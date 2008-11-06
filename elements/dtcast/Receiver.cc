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


void DtcastReceiver::onRouteRequest( DtcastRRPacket *pkt )
{
	if( pkt==NULL ) return;
	if( pkt->dtcast()->_mcast==_mcast ) //yes, we need this stream
	{
		// special DTCAST RouteReply packet. NEXT field is set to invalid value, DST field is set to SELF node,
		// which will indicate local delivery for forwarding table
		output( 0 ).push( DtcastRTPacket::make(pkt->dtcast()->_src,pkt->dtcast()->_mcast,DTCAST_NODE_SELF,
					_seq++,DTCAST_NODE_ALL,nodelist_t().add( _me )) );
	}
	
	//do not kill packet, because Forwarder will resend it further
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastReceiver)
