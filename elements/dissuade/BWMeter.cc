
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/straccum.hh>

#include "BWMeter.hh"

BWMeter::BWMeter( )
:_tau(0.1) //cut-off frequency 10Hz
,_bk(0)
,_bk_1(0)
,_b(0)
,_b_1(0)
,_tk_1(0,0)
,_timer( this )
,_length( 0 )
{
}

int BWMeter::initialize( ErrorHandler* )
{
	_timer.initialize( this );
	_timer.schedule_now();
	return 0;
}

int BWMeter::configure( Vector<String> &conf, ErrorHandler *errH )
{
	int freq=10;
	int ret=cp_va_kparse( conf,this,errH,
					"FREQ", cpkNormal, cpInteger,&freq,
					cpEnd );
	_tau=1/(double)freq;
	return ret;
}

inline double BWMeter::a( double delta )
{
	return (2*_tau/delta-1)/(2*_tau/delta+1);
}

inline void BWMeter::updateBandwidth( uint32_t length )
{
	Timestamp tk=Timestamp::now( );
	_bk=length/(tk-_tk_1).doubleval();
//	ErrorHandler::default_handler()->debug( "%dKiBit, len=%d, delta=%lf", (int)(_b/1024), length, (tk-_tk_1).doubleval() );

	_b=a((tk-_tk_1).doubleval())*_b_1 + (1-a((tk-_tk_1).doubleval()))/2*(_bk+_bk_1);
	StringAccum os;
	os << Timestamp::now() << "\t" << length << ", " << tk-_tk_1 
						   << "\t" << (long)((_b)/1024) << " Kbytes"
						   << "\t" << (long)((_bk)/1024) << " Kbytes";
	ErrorHandler::default_handler()->message( "BWMETER: %s", os.c_str() );
	
	_b_1=_b;
	_bk_1=_bk;
	_tk_1=tk;
}

void BWMeter::push( int, Packet *pkt )
{
	_length+=pkt->length( );
	
	output(0).push( pkt ); 
	//pkt cannot be used anymore
}

void BWMeter::run_timer( Timer * )
{
	updateBandwidth( _length );
	_length=0;

	StringAccum os;
//	os << Timestamp::now() << "\t" << _b;
	os << _count; _count=0;
	ErrorHandler::default_handler()->message( "BWMETER: %s", os.c_str() );

	_timer.reschedule_after_msec( _tau*1000 );
}


EXPORT_ELEMENT( BWMeter )
