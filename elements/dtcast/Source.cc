/**
 *	Definition of the Source NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/vector.hh>
#include <click/timer.hh>
#include <click/element.hh>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timestamp.hh>
#include <click/timer.hh>
#include <click/timer.hh>

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


void DtcastSource::push( int, Packet *pkt )
{
	_queue.addOrUpdate( new dtcast_message_t(_me,_mcast,_me,_seq_data++,
			Timestamp::now()+_age,
			_dsts,
			pkt->data(),pkt->length(),
			false) );
	_timer.schedule_now( );	
	pkt->kill( );
}

void DtcastSource::run_timer( Timer *timer )
{
	_queue.purgeOldRecords( NULL );
	if( !_queue.empty() )
	{
		if( Timestamp::now()-_lastRRSendBy>ROUTE_REQUEST_TIME )
		{
			output( 0 ).push( DtcastRRPacket::make(_me,_mcast,_me,_seq_rr++) );
			_lastRRSendBy=Timestamp::now();
		}
		
//		for( DtcastMessageQueue::iterator i=_queue.begin(); i!=_queue.end(); i++ )
//		{
//			output( 0 ).push( DtcastDataPacket::make(*i) );
//		}
	}
	
	timer->reschedule_after_sec( ROUTE_REQUEST_TIME );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastSource)
