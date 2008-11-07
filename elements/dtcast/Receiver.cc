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
	
	switch( dpkt->dtcast()->_type )
	{
		case DTCAST_TYPE_DATA:
			onDataPacket( DtcastDataPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_RR:
			onRouteRequest( DtcastRRPacket::make(dpkt) );
			break;
		default:
			ErrorHandler::default_handler()->fatal( "DTCAST: DtcastReceiver has received not serviced packet type" );
			break;
	}
}

void DtcastReceiver::onDataPacket( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return; //will be fatal error
	
	output( DATA ).push( pkt->data_payload() ); // Element should be connected to IPClassify
}

void DtcastReceiver::onRouteRequest( DtcastRRPacket *pkt )
{
	if( pkt==NULL ) return;
	if( pkt->dtcast()->_mcast==_mcast ) //yes, we need this stream
	{
		// special DTCAST RouteReply packet. NEXT field is set to invalid value, DST field is set to SELF node,
		// which will indicate local delivery for forwarding table
		output( FORWARDER ).push( DtcastRTPacket::make(pkt->dtcast()->_src,pkt->dtcast()->_mcast,
					DTCAST_NODE_SELF,_seq++,DTCAST_NODE_ALL,nodelist_t().add( _me )) );
	}
	
	pkt->kill( );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastReceiver)
