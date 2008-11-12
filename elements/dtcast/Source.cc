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
		, _seq_data(0) ///< @todo maybe change sequence number initialization for random value
		, _seq_rr(0)
		, _timer( this )
{
}

int DtcastSource::initialize( ErrorHandler* )
{
	_timer.initialize( this );
	_timer.schedule_now( );
	return 0;
}

#if !CLICK_TOOL
#define CP_CONTEXT , const Element *
#else
#define CP_CONTEXT
#endif

static void cp_nodelist_parse( cp_value *v, const String &str, ErrorHandler *errH, 
		const char * CP_CONTEXT )
{
	nodelist_t *list=(nodelist_t*)v->store;
	
	Vector<String> dsts;
	cp_spacevec( str, dsts );
	if( dsts.size()==0 ) 
	{
		errH->fatal( "Destinations argument should contain some values" );
		return;
	}
	
	for( Vector<String>::iterator i=dsts.begin(); i!=dsts.end(); i++ )
	{
		uint32_t tmp=atoi( (*i).c_str() );
		if( tmp>0 ) 
		{
			list->push_back( tmp );
		}
		else
		{
			errH->fatal( "Destinations argument should contain only integer values > 0" );
			return;
		}
	}
}

static void do_nothing( cp_value *	CP_CONTEXT ) { }

#define cpNodelist	"nodelist"

int DtcastSource::configure( Vector<String> &conf, ErrorHandler *errH )
{
	cp_register_argtype( cpNodelist, "Nodelist", 0, cp_nodelist_parse, do_nothing, NULL );
	
	Vector<String> dsts;
	return cp_va_kparse( conf,this,errH,
			"NODE",  cpkPositional, cpInteger, &_me,
			"MCAST", cpkNormal, cpInteger, &_mcast,
			"DST",   cpkNormal, cpNodelist, &_dsts,
			"AGE",   cpkNormal, cpTimestamp,&_age,
					 cpEnd );
}

void DtcastSource::push( int port, Packet *pkt )
{
	if( port==DATA )
	{
		onInputData( pkt );
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
			onAck( DtcastAckPacket::make(dpkt) );
			break;
		default:
			ErrorHandler::default_handler()->fatal( "DTCAST: DtcastSource has received not serviced packet type" );
			break;
		}
	}
}

void DtcastSource::onInputData( Packet *pkt )
{
	_queue.addOrUpdate( new dtcast_message_t(_me,_mcast,DTCAST_NODE_SELF,_seq_data++,
			Timestamp::now()+_age,
			_dsts,
			pkt->data(),pkt->length(),
			false) );
	_timer.schedule_now( );	
	pkt->kill( );
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
		_queue.addOrUpdate( new dtcast_message_t(
					_me,pkt->dtcast()->_mcast,
				    DTCAST_NODE_SELF,
					pkt->dtcast()->_seq,
					pkt->age(),
					pkt->dsts(),
					pkt->body(),pkt->body_len(),
					pkt->dtcast()->_flags&DTCAST_FLAG_EPIDEMIC
			) );
	}
	pkt->kill( );
}

void DtcastSource::onAck( DtcastAckPacket *pkt )
{
	if( pkt==NULL ) return;
	
	dtcast_message_t *msg=_queue.get( msg_key_t(pkt->dtcast()->_src,pkt->dtcast()->_mcast,pkt->dtcast()->_seq) );
	if( msg!=NULL ) //otherwise hmm... ack received, but there is no message...
	{
		msg->_unack_ids-=pkt->dst_ids( ); //acknowledge receiving
		// if there is no unacknowledged destinations, message will be removed from queue in
		// the next timer cycle
	}
	
	pkt->kill( );
}

void DtcastSource::run_timer( Timer *timer )
{
	_queue.purgeOldRecords( NULL );
	_mcasts.purgeOldRecords( NULL );
	if( !_queue.empty() )
	{
		for( DtcastMessageQueue::iterator i=_queue.begin(); i!=_queue.end(); i++ )
		{
			mcast_tuple_t *mcast=_mcasts.get( mcast_key_t(*i->second) );
			if( !mcast || Timestamp::now()-mcast->_last_update>ROUTE_REQUEST_TIME )
			{
				output( 0 ).push( DtcastRRPacket::make(_me,i->second->_mcast_id,DTCAST_NODE_SELF,_seq_rr++) );
				_mcasts.addOrUpdate( new mcast_tuple_t(*i->second) );
			}
			
			bool broadcast=false;
			if( !i->second->_isFirstTry && 
				i->second->_firstTryTime+TIMEOUT_ER_START<=Timestamp::now() && //EPIDEMIC ROUTING TIMEOUT
				i->second->_firstTryTime+TIMEOUT_ER_STOP >=Timestamp::now()	) //switch on epidemic OR update epidemic
			{
				if( !i->second->_epidemic || 
					(i->second->_epidemic && i->second->_lastTryTime+TIMEOUT_ER_RESEND<Timestamp::now()) )
				{
					i->second->_epidemic=true;
					broadcast=true;
				}
			}
			if( i->second->_epidemic &&
				i->second->_firstTryTime+TIMEOUT_ER_STOP<Timestamp::now() ) // switch off epidemic
			{
				i->second->_epidemic=false;
			}
			
			if( !i->second->_epidemic && 
				i->second->_lastTryTime+TIMEOUT_DATA_RETRANSMIT<Timestamp::now() )
			{
				broadcast=true;
			}

			if( i->second->_isFirstTry ) 
			{ 
				i->second->_firstTryTime=Timestamp::now(); 
				i->second->_isFirstTry=false;
				broadcast=true; 
			}
			
			if( broadcast ) // finally if we need broadcast packet
			{
				i->second->_lastTryTime=Timestamp::now();
				output( 0 ).push( DtcastDataPacket::make(*(i->second)) );
			}
		}
	}
	
	timer->reschedule_after_sec( 1 );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastSource)
