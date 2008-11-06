/**
 *	Definition of the FORWARDING NODE in DTCAST Network
 */

#include <click/config.h>

#include <click/confparse.hh>
#include <click/error.hh>
#include <click/element.hh>

#include "Forwarder.hh"
CLICK_DECLS

//void callbackHelper( Timer *timer, void *param )
//{
//	((Purger*)param)->purgeOldRecords( timer );
//};

DtcastForwarder::DtcastForwarder()
		: _me(DTCAST_NODE_SELF),_source(NULL), _receiver(NULL)
, _refresher_source_routing(this), _refresher_forwarding(this)
{
}

DtcastForwarder::~DtcastForwarder( )
{
}

int DtcastForwarder::initialize( ErrorHandler* )
{
	_refresher_source_routing.initialize( this );
	_refresher_forwarding.    initialize( this );

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
			pkt->kill( );
			break;
	}
}

void DtcastForwarder::onRouteRequest( DtcastRRPacket *pkt )
{
	if( pkt==NULL ) return;
	//ErrorHandler::default_handler( )->message( "DTCAST: we've got RouteRequest packet" );
	_source_routing.addOrUpdate( new dtcast_srouting_tuple_t(pkt->dtcast()->_src,pkt->dtcast()->_from) );
	
	_receiver->onRouteRequest( pkt );

	pkt->dtcast()->_from=_me;
	output( 0 ).push( pkt );
}

void DtcastForwarder::onRouteReply( DtcastRTPacket *pkt )
{
	if( pkt==NULL ) return;
	if( pkt->next_id()!=_me && pkt->dtcast()->_from!=DTCAST_NODE_SELF ) { pkt->kill(); return; }
	
	DtcastSRoutingTable::iterator sroute=find( _source_routing.begin(), _source_routing.end(), 
			&dtcast_srouting_tuple_t::isEqualSource, pkt->dtcast()->_src );

	/**
	 * If there is record in the Source Routing Table, than add/or update records in forwarding
	 * table for all nodes in the Route Reply packet
	 *
	 * If we actually have added record, try to immediately propagate RouteReply via early scheduling
	 * or forwarding table refreshment
	 */
	if( sroute!=_source_routing.end() ) 
	{
		nodelist_t nodes=pkt->dst_ids( );
		bool immediate=false;
		for( nodelist_t::iterator i=nodes.begin(); i!=nodes.end(); i++ )
		{
			bool isnew=_forwarding.addOrUpdate( 
					new dtcast_fwd_tuple_t(pkt->dtcast()->_mcast,pkt->dtcast()->_src,
						*i,pkt->dtcast()->_from!=DTCAST_NODE_SELF) );
			
			if( isnew ) immediate=true;
		}
		if( immediate ) _refresher_forwarding.schedule_now( );
	}
	
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


void DtcastForwarder::onRefreshSRouting( )
{
	
	
	_forwarding.purgeOldRecords( NULL );
}

void DtcastForwarder::onRefreshForwarding( )
{
	
	_source_routing.purgeOldRecords( NULL );
}

void DtcastForwarder::run_timer( Timer *timer )
{
	if( timer==&this->_refresher_forwarding )
		onRefreshForwarding( );
	else if( timer==&this->_refresher_source_routing )
		onRefreshSRouting( );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastForwarder)
