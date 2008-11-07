/**
 *	Definition of the Source NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>

#include "Source.hh"

CLICK_DECLS

DtcastSource::DtcastSource( )
		: _me( DTCAST_NODE_SELF )
		, _mcast( DTCAST_MCAST_NULL ) 
		, _age( Timestamp::Timestamp(3600,0) ) ///< default age = 3600 seconds
		, _seq_data(0) /// @todo change sequence number initialization for random value
		, _seq_rr(0)
		, _timer( this )
		, _lastRRSendBy( 0,0 )
{
}

int DtcastSource::initialize( ErrorHandler* )
{
	_timer.initialize( this );
	_timer.schedule_now( );
	return 0;
}

int DtcastSource::configure( Vector<String> &conf, ErrorHandler *errH )
{
	Vector<String> dsts;
	return cp_va_kparse( conf,this,errH,
			"NODE",  cpkPositional, cpInteger, &_me,
			"MCAST", cpkPositional, cpInteger, &_mcast,
			"AGE",   cpkNormal,     cpTimestamp, &_age,
			"DST",   cpkNormal,		cpArguments, &dsts,
					 cpEnd );
	for( Vector<String>::iterator i=dsts.begin(); i!=dsts.end(); i++ )
	{
		uint32_t tmp;
		if( (tmp=atoi((*i).c_str())) ) _dsts.push_back( tmp );
	}
}


void DtcastSource::push( int port, Packet *pkt )
{
	if( port==DATA )
	{
		_queue.addOrUpdate( new dtcast_message_t(_me,_mcast,_me,_seq_data++,
				Timestamp::now()+_age,
				_dsts,
				pkt->data(),pkt->length(),
				false) );
		_timer.schedule_now( );	
		pkt->kill( );
	}
	else if( port==FORWARDER )
	{
		DtcastPacket *dpkt=DtcastPacket::make( pkt );
		if( dpkt==NULL ) return;
		
		switch( dpkt->dtcast()->_type )
		{
		case DTCAST_TYPE_DATA:
			onData( DtcastDataWithDstsPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_RT:
			onRouteReply( DtcastRTPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_ACK:
		case DTCAST_TYPE_ERACK: //it doesn't matter for Source node whether it is normal or epidemic routing ACK
			onAck( DtcastAckPacket::make(dpkt) );
			break;
		default:
			ErrorHandler::default_handler()->fatal( "DTCAST: DtcastSource has received not serviced packet type" );
			break;
		}
	}
}

void DtcastSource::run_timer( Timer *timer )
{
	_queue.purgeOldRecords( NULL );
	if( !_queue.empty() )
	{
		if( Timestamp::now()-_lastRRSendBy>ROUTE_REQUEST_TIME )
		{
			output( 0 ).push( DtcastRRPacket::make(_me,_mcast,DTCAST_NODE_SELF,_seq_rr++) );
			_lastRRSendBy=Timestamp::now();
		}
		
//		for( DtcastMessageQueue::iterator i=_queue.begin(); i!=_queue.end(); i++ )
//		{
//			output( 0 ).push( DtcastDataPacket::make(*i) );
//		}
	}
	
	timer->reschedule_after_sec( ROUTE_REQUEST_TIME );
}

void DtcastSource::onRouteReply( DtcastRTPacket *pkt )
{
	if( pkt==NULL ) return;
	/// maybe do something usefull
	pkt->kill( );
}

void DtcastSource::onData( DtcastDataWithDstsPacket *pkt )
{
	if( pkt==NULL ) return;
	if( pkt->dsts().size()!=0 )
	{
		_fwd_queue.addOrUpdate( new dtcast_message_t(
					pkt->dtcast()->_src,pkt->dtcast()->_mcast,_me,pkt->dtcast()->_seq,
					pkt->age(),
					pkt->dsts(),
					pkt->body(),pkt->body_len(),
					pkt->dtcast()->_type==DTCAST_TYPE_ERDATA
			) );
	}
	pkt->kill( );
}

void DtcastSource::onAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;
	DtcastMessageQueue &queue=_queue;
	if( pkt->dtcast()->_src!=_me ) queue=_fwd_queue;
	
	dtcast_message_t *msg=queue.get( msg_key_t(pkt->dtcast()->_src,pkt->dtcast()->_mcast,pkt->dtcast()->_seq) );
	if( msg!=NULL ) //otherwise hmm... ack received, but there is no message...
	{
		msg->_unack_ids-=pkt->dst_ids( ); //acknowledge receiving
		if( msg->_unack_ids.size()==0 ) queue.erase( msg_key_t(pkt->dtcast()->_src,pkt->dtcast()->_mcast,pkt->dtcast()->_seq) );
	}
	
	pkt->kill( );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastSource)
