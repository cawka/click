/**
 * Definition of the age_tuple_t struct
 */

#ifndef DEFS_AGE_TUPLE_HH_DTCAST
#define DEFS_AGE_TUPLE_HH_DTCAST

#include <click/timestamp.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include "algorithm.hh"

template <const int MAXAGE>
struct age_tuple_t
{
	Timestamp	_last_update;
	
	age_tuple_t( ) { update(*this); }
	
	void update( age_tuple_t& )
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
class AgeTable : public Purger, public HashTable<tuple_t,tuple_t*>
{
public:
	/**
	 * Adds or updates record in the table
	 *
	 * @return true - added, false - updated //iterator of the added or update record
	 */
	bool addOrUpdate( tuple_t *tuple )
	{
		tuple_t *test=this->get( *tuple );//find( this->begin(), this->end(), *tuple );
		if( test )
		{
			test->update( *tuple );
			delete tuple;
			return false;
		}
		else
		{
//			push_back( tuple );
			this->set( *tuple, tuple );
			//return this->end();//(this->end()-1);
			return true;
		}
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
	typedef HashTable<tuple_t,tuple_t*> table_t;
	typedef typename table_t::iterator iterator;
//	table_t _table;
};


void callbackHelper( Timer *timer, void *param ); //defined in Forwarder.cc


#endif
