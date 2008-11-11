
#include <click/config.h>
#include <click/confparse.hh>

#include "PacketReorder.hh"
#include "click/error.hh"

PacketReorder::PacketReorder( )
		: _probability( CLICK_RAND_MAX )
		, _size( 10 )
{
}

int PacketReorder::configure( Vector<String> &conf, ErrorHandler *errH )
{
	double prob;
	return cp_va_kparse( conf,this,errH,
					"SIZE",  cpkPositional, cpInteger, &_size,
					"PROB",  cpkPositional, cpDouble, &_probability,
					cpEnd );
	_probability=prob * CLICK_RAND_MAX;	
}

void PacketReorder::formOutput( )
{
	for( input_t::iterator i=_input_queue.begin(); i!=_input_queue.end(); i++ )
	{
		if( click_random()>_probability ) 
			_output_queue.push_front( new pkt_t(*i) );
		else
			_output_queue.push_back( new pkt_t(*i) );
	}
	_input_queue.clear( );
}

void PacketReorder::push( int, Packet *pkt )
{
	if( _input_queue.size()==(int)_size ) 
		if( _output_queue.size()!=0 )
		{ 
			pkt->kill(); 
			return; 
		}
		else
			formOutput( );
	
	_input_queue.push_back( pkt );
}

Packet* PacketReorder::pull( )
{
	pkt_t *pkt=_output_queue.back( );
	Packet *ret=0;
	if( pkt ) 
	{
		_output_queue.pop_back();
		ret=*pkt;
		delete pkt;
	}
	return ret;
}


EXPORT_ELEMENT(PacketReorder)
