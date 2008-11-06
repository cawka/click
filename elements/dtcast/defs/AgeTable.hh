/**
 * Definition of the age_tuple_t struct
 */

#ifndef DEFS_AGE_TUPLE_HH_DTCAST
#define DEFS_AGE_TUPLE_HH_DTCAST

#include <click/timestamp.hh>
#include <click/timer.hh>
#include <click/list.hh>
#include "algorithm.hh"

template <const int MAXAGE>
struct age_tuple_t
{
	Timestamp	_last_update;
	
	age_tuple_t( ) { update(); }
	
	void update( )
	{
		_last_update=Timestamp::now( );
	}
	
	bool canPurge( Timestamp ref )
	{
		return ref-_last_update>MAXAGE;
	}
};

class Purger
{
public:
	virtual void purgeOldRecords( Timer *timer )=0;
};

template<class tuple_t,const int MAXAGE>
class AgeTable : public Purger, public List<tuple_t,&tuple_t::link>
{
public:
	void addOrUpdate( tuple_t *tuple )
	{
		iterator test=find( this->begin(), this->end(), *tuple );
		if( test!=this->end() )
		{
			test->update( );
			delete tuple;
		}
		else
			push_back( tuple );
	}
	
	~AgeTable( )
	{
		purge<tuple_t>( *this );
	}
	
	virtual void purgeOldRecords( Timer *timer )
	{
		iterator i=find( this->begin(),this->end(),
								  &tuple_t::canPurge,Timestamp::now() );
		while( i!=this->end() )
		{
			i=this->erase( i );
			i=find( i,this->end(),
					&tuple_t::canPurge,Timestamp::now() );
		}
		if( timer ) timer->reschedule_after_sec( MAXAGE );
	}

//private:
	typedef List<tuple_t,&tuple_t::link> table_t;
	typedef typename table_t::iterator iterator;
//	table_t _table;
};


void callbackHelper( Timer *timer, void *param ); //defined in Forwarder.cc


#endif
