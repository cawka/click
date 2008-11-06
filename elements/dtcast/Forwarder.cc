/**
 *	Definition of the FORWARDING NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/element.hh>

#include "Forwarder.hh"
CLICK_DECLS

void callbackHelper( Timer *timer, void *param )
{
	((Purger*)param)->purgeOldRecords( timer );
};


DtcastForwarder::DtcastForwarder()
		: _me(DTCAST_NODE_SELF),_source(NULL), _receiver(NULL)
{
}

DtcastForwarder::~DtcastForwarder( )
{
}

int DtcastForwarder::initialize( ErrorHandler* )
{
	_refresher_source_routing.assign( callbackHelper, &_source_routing );
	_refresher_forwarding.    assign( callbackHelper, &_forwarding );
	
	_refresher_source_routing.initialize( this->router() );
	_refresher_forwarding.    initialize( this->router() );

	_refresher_source_routing.schedule_now( );
	_refresher_forwarding.    schedule_now( );
	
	return 0;
}

int DtcastForwarder::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"NODE",		cpkPositional, cpInteger, &_me,
			"SOURCE",   cpkPositional, cpElement, &_source,
			"RECEIVER", cpkPositional, cpElement, &_receiver,
					cpEnd );
}

void DtcastForwarder::push( int,Packet *pkt ) //we have only one input port
{
	//pkt should containt valid IP packet with protocol field set to IP_PROTO_DTCAST
	DtcastPacket *dtcast=DtcastPacket::make( pkt );
	if( dtcast==NULL ) { pkt->kill( ); return; }
	
//	ErrorHandler::default_handler()->debug( "DTCAST: valid packet (type=%d,len=%d)",
//			dtcast->dtcast()->_type, dtcast->dtcast()->_length );
	
	switch( dtcast->dtcast()->_type )
	{
		case DTCAST_TYPE_RR:
			onRouteRequest( DtcastRRPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_RT:
			onRouteReply( DtcastRTPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_DATA:
			onData( DtcastDataPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_ACK:
			onAck( DtcastAckPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_ERDATA:
			onERData( DtcastDataPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_ERACK:
			onERAck( DtcastAckPacket::make(dtcast) );
			break;
		default:
			ErrorHandler::default_handler()->fatal( "DTCAST: unknown packet type (%d)",dtcast->dtcast()->_type );
			break;
	}
}

void DtcastForwarder::onRouteRequest( DtcastRRPacket *pkt )
{
	if( pkt==NULL ) return;
	//ErrorHandler::default_handler( )->message( "DTCAST: we've got RouteRequest packet" );
	_source_routing.addOrUpdate( new dtcast_srouting_tuple_t(pkt->dtcast()->_src,pkt->dtcast()->_from) );
	
/**
 * @todo Do something to the DtcastReceiver
 */
	
	pkt->dtcast()->_from=_me;
	output( 0 ).push( pkt );
}

void DtcastForwarder::onRouteReply( DtcastRTPacket *pkt )
{
	if( pkt==NULL ) return;
	
	pkt->kill( );
}

void DtcastForwarder::onData( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return;
	
	
	pkt->kill( );
}

void DtcastForwarder::onAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;
	
	pkt->kill( );
}

void DtcastForwarder::onERData( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return;
	
	pkt->kill( );
}

void DtcastForwarder::onERAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;
	
	pkt->kill( );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastForwarder)


		