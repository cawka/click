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
	: _me(DTCAST_NODE_SELF)
	, _activeAck( true )
	, _refresher_source_routing(callbackHelper, &_source_routing)
	, _refresher_forwarding(callbackHelper, &_forwarding)
	, _refresher_cache(callbackHelper, &_cache)
{
}

DtcastForwarder::~DtcastForwarder( )
{
}

int DtcastForwarder::initialize( ErrorHandler* )
{
	_refresher_source_routing.initialize( this );
	_refresher_forwarding.    initialize( this );
	_refresher_cache.		  initialize( this );

	_refresher_source_routing.schedule_now( );
	_refresher_forwarding.    schedule_now( );
	_refresher_cache.         schedule_now( );
	
	return 0;
}

int DtcastForwarder::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"NODE",	cpkPositional, cpInteger, &_me,
			"ACTIVE_ACK", cpkNormal | cpkMandatory, cpBool, &_activeAck,
					cpEnd );
}

void DtcastForwarder::push( int,Packet *pkt ) //we have only one input port
{
	//pkt should containt valid IP packet with protocol field set to IP_PROTO_DTCAST
	DtcastPacket *dtcast=DtcastPacket::make( pkt );
	if( dtcast==NULL ) return;
	if( _cache.receivedFromDifferentNode(*dtcast) ) 
	{
		if( !_activeAck && dtcast->dtcast()->_type==DTCAST_TYPE_DATA )
			onImplicitAck(  DtcastDataPacket::make(dtcast) );
		else
			pkt->kill(); 
		return; 
	}

	switch( dtcast->dtcast()->_type )
	{
		case DTCAST_TYPE_RR:
			onRouteRequest( DtcastRRPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_RT:
			onRouteReply( DtcastRTPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_DATA:
			if( dtcast->dtcast()->_flags&DTCAST_FLAG_EPIDEMIC )
				onERData( DtcastDataPacket::make(dtcast) );
			else
				onData( DtcastDataPacket::make(dtcast) );
			break;
		case DTCAST_TYPE_ACK:
			if( dtcast->dtcast()->_flags&DTCAST_FLAG_EPIDEMIC )
				onERAck( DtcastAckPacket::make(dtcast) );
			else
				onAck( DtcastAckPacket::make(dtcast) );
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
	_source_routing.addOrUpdate( new dtcast_srouting_tuple_t(pkt->dtcast()->_src,
			( pkt->dtcast()->_from==DTCAST_NODE_SELF?_me:pkt->dtcast()->_from )) );
	
	// forwarder node has no states. It simply maintain routing and forwarding
	// table and rebroadcasts all necessary packets
	output( RECEIVER ).push( pkt->clone() );

	broadcastPacket( pkt );
}

void DtcastForwarder::onRouteReply( DtcastRTPacket *pkt )
{
	if( pkt==NULL ) return;

	// allow if:
	//		 - _from   == DTCAST_NODE_SELF
	//		 - next_id == _me
	//       - next_id == DTCAST_NODE_ALL (local recovery mode)	
	if( pkt->next_id()      !=_me && 
		pkt->next_id()      !=DTCAST_NODE_ALL &&
		pkt->dtcast()->_from!=DTCAST_NODE_SELF ) 
	{ 
		return pkt->kill();
	}

	dtcast_srouting_tuple_t *sRoute=_source_routing.get( srouting_key_t(pkt->dtcast()->_src) );
	if( !sRoute ) return pkt->kill( );//hmmm, timeout or someone tries to do something wrong

	_forwarding.addOrUpdate( new dtcast_fwd_tuple_t(pkt->dtcast()->_mcast, pkt->dtcast()->_src) );

	dtcast_fwd_tuple_t *fwd=_forwarding.get( fwd_key_t(pkt->dtcast()->_mcast, pkt->dtcast()->_src) );
	assert( fwd!=NULL );

	nodelist_t nodes=pkt->dst_ids( );
	for( nodelist_t::iterator dst=nodes.begin(); dst!=nodes.end(); dst++ )
	{
		fwd->_dsts.addOrUpdate( new dtcast_dst_tuple_t(*dst,pkt->dtcast()->_from!=DTCAST_NODE_SELF) );
	}
	
	if( sRoute->_next_id==_me ) //source is local
	{
		output( SOURCE ).push( pkt );
	}
	else
	{
		/**
		 * Allow one-hop flood RouteReply packets (local recovery mode)
		 */
		if( !(pkt->next_id()==DTCAST_NODE_ALL && pkt->dtcast()->_from==DTCAST_NODE_SELF) )
			pkt->next_id( sRoute->_next_id );
		
		broadcastPacket( pkt );
	}
}

void DtcastForwarder::onDataLocal( DtcastDataPacket *pkt, dtcast_fwd_tuple_t *fwd )
{
	if( fwd->needLocalDelivery() && !(pkt->dtcast()->_flags&DTCAST_FLAG_DUPLICATE) ) //we already have delivered packet to local receiver
	{
		output( RECEIVER ).push( pkt->clone() );

		// send local ACKs to the source for local delivered 
		onAck( DtcastAckPacket::make( pkt->dtcast()->_src,
									  pkt->dtcast()->_mcast,_me,
									  pkt->dtcast()->_seq,
									  fwd->local_dsts(),  //all destinations for src/mcast reachable through this node
									  false) );
	}

	if( fwd->needForward() )
		broadcastPacket( pkt );
	else
		pkt->kill( );
}

void DtcastForwarder::onData( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return;
	dtcast_fwd_tuple_t *fwd=_forwarding.get( dtcast_fwd_tuple_t(pkt->dtcast()->_mcast, pkt->dtcast()->_src) );
	if( fwd==NULL ) // we are not on the delivery path (haven't reveiced any RouteReplies)
	{
		pkt->kill( );
		return;
	}
	if( pkt->dtcast()->_from==DTCAST_NODE_SELF ) return onDataLocal( pkt, fwd );
 
	if( _activeAck || pkt->dtcast()->_flags&DTCAST_FLAG_DUPLICATE )
	{ // broadcast ACK packet, confirming reception of data packet designated to {dsts}
	  //
	  // Send ACK when explicit ACKs are specified and if we have already received same
	  // DATA message from same source earlier unless we are receiving DATA from the own Source
		output(BROADCAST).push( DtcastAckPacket::make(pkt->dtcast()->_src,
											  pkt->dtcast()->_mcast,_me,
											  pkt->dtcast()->_seq,
											  fwd->_dsts,  //all destinations for src/mcast reachable through this node
											  false) );
	}
	//
	/*************/
	if( pkt->dtcast()->_flags&DTCAST_FLAG_DUPLICATE ) return pkt->kill( ); //if it is duplicate DATA then do not propagate this data message anywhere
	/*************/
	//
	output( SOURCE ).push( DtcastDataWithDstsPacket::make(pkt,fwd->_dsts) ); //save packet for waiting for delivery acknowledgement
	
	if( fwd->needLocalDelivery() ) 
	{
		output( RECEIVER ).push( pkt->clone() );
	}

	if( fwd->needForward() )
		broadcastPacket( pkt );
	else
		pkt->kill( );
}

void DtcastForwarder::onAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;

	output( SOURCE ).push( pkt ); //just forward to the source
}

void DtcastForwarder::onImplicitAck( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return;
	// not yet formalized what to do in this step
	
	pkt->kill( );
}

void DtcastForwarder::onERData( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return;
	// in the epidemic mode do not consult forwarding tables, just 
	// save packet if not from SELF, notify receiver and forward if TTL allows

	/** !!!
	 * Epidemic data packets are just rebroadcasted. No saving in the local Source
	 */
//	if( pkt->dtcast()->_from!=DTCAST_NODE_SELF ) //save in the source
//		output( SOURCE ).push( DtcastDataWithDstsPacket::make(pkt,nodelist_t()) ); // save epidemic dissemination data packet
	
	dtcast_cache_tuple_t *cache=_cache.get( cache_key_t(*pkt) );
	if( !cache ) output( RECEIVER ).push( pkt->clone( ) ); //notify receiver
	
	broadcastPacket( pkt ); //rebroadcast if TTL allows
}

/**
 * Message receiving acknowledgement 
 * 
 * In the epidemic mode ACK message can be generated only be appropriate Receiver
 */
void DtcastForwarder::onERAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;
	
	output( SOURCE ).push( pkt->clone() ); //notify SOURCE
	broadcastPacket( pkt );				  //rebroadcast packet if TTL allows
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastForwarder)
