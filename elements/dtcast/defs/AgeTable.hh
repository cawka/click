/**
 * Definition of the age_tuple_t struct
 */

#ifndef DEFS_AGE_TUPLE_HH_DTCAST
#define DEFS_AGE_TUPLE_HH_DTCAST

#include <click/timestamp.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/straccum.hh>
#include "algorithm.hh"

template <const int MAXAGE>
struct age_tuple_t
{
	Timestamp	_last_update;
	
	age_tuple_t( ) { update(*this); }
	
	void update( age_tuple_t& )
	{
//		ErrorHandler::default_handler()->debug( "BUGOGA: I'm updated" );
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
        virtual ~Purger( ) { };
};

template<class key_t, class tuple_t,const int MAXAGE>
class AgeTable : public Purger, public HashTable<key_t,tuple_t*>
{
public:
	/**
	 * Adds or updates record in the table
	 *
	 * @return true - added, false - updated //iterator of the added or update record
	 */
	bool addOrUpdate( tuple_t *tuple )
	{
		tuple_t *test=this->get( key_t(*tuple) );//find( this->begin(), this->end(), *tuple );
		if( test )
		{
			test->update( *tuple );
			delete tuple;
			return false;
		}
		else
		{
//			push_back( tuple );
			this->set( key_t(*tuple), tuple );
			//return this->end();//(this->end()-1);
			return true;
		}
	}
	
	virtual ~AgeTable( )
	{
		purge<tuple_t>( *this );
	}
	
	virtual void purgeOldRecords( Timer *timer )
	{
		iterator i=::find( this->begin(),this->end(),
				 &tuple_t::canPurge,Timestamp::now() );
		while( i!=this->end() )
		{
			i=this->erase( i );
			i=::find( i,this->end(),
					&tuple_t::canPurge,Timestamp::now() );
		}
		if( timer ) timer->reschedule_after_sec( MAXAGE );
		
		
		/**
		 * DEBUGGING
		 */
		if( !_debug ) return;
//		ErrorHandler::default_handler()->debug( "%s: <<<<<<<<",  _label.c_str());
		for( i=this->begin(); i!=this->end(); i++ )
		{
			StringAccum os; os << *(i->second);
			ErrorHandler::default_handler()->debug( "%s: %s", _label.c_str(),
				os.take_string().c_str() );
		}				
	}

//private:
	typedef HashTable<key_t,tuple_t*> table_t;
	typedef typename table_t::iterator iterator;
//	table_t _table;
	String _label;
	bool _debug;
};


//void callbackHelper( Timer *timer, void *param ); //defined in Forwarder.cc


template<class key_t, class tuple_t,const int MAXAGE>
inline StringAccum& operator<<(StringAccum &os,const AgeTable<key_t,tuple_t,MAXAGE> &table )
{
	for( typename AgeTable<key_t,tuple_t,MAXAGE>::const_iterator i=table.begin(); i!=table.end(); i++ )
	{
		if( i!=table.begin() ) os << "\n";
		os << *(i->second);
	}
	return os;
}

#endif
