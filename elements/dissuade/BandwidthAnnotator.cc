
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "BandwidthAnnotator.hh"

BandwidthAnnotator::BandwidthAnnotator( )
:_tau(0.1) //cut-off frequency 10Hz
,_bk(0)
,_bk_1(0)
,_b(0)
,_b_1(0)
,_tk_1(0,0)
,_timer( this )
{
}

int BandwidthAnnotator::initialize( ErrorHandler* )
{
	_timer.initialize( this );
	_timer.schedule_now();
	return 0;
}

int BandwidthAnnotator::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
					cpEnd );
}

inline double BandwidthAnnotator::a( double delta )
{
	return (2*_tau/delta-1)/(2*_tau/delta+1);
}

inline void BandwidthAnnotator::updateBandwidth( uint32_t length )
{
	Timestamp tk=Timestamp::now( );
	_bk=length/(tk-_tk_1).doubleval();
//	ErrorHandler::default_handler()->debug( "%dKiBit, len=%d, delta=%lf", (int)(_b/1024), length, (tk-_tk_1).doubleval() );

	_b=a((tk-_tk_1).doubleval())*_b_1 + (1-a((tk-_tk_1).doubleval()))/2*(_bk+_bk_1);
	
	_b_1=_b;
	_bk_1=_bk;
	_tk_1=tk;
}

void BandwidthAnnotator::push( int, Packet *pkt )
{
	updateBandwidth( pkt->length() );
	output(0).push( pkt );
	
//	ErrorHandler::default_handler()->debug( "length=%d, Estimate=%d", pkt->length(), (int)_bk );
}

void BandwidthAnnotator::run_timer( Timer * )
{
	updateBandwidth( 0 );

	_timer.reschedule_after_msec( _tau*1000 );
}


EXPORT_ELEMENT( BandwidthAnnotator )
		