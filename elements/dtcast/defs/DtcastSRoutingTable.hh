/**
 *	@file Definition of the DTCAST Source Routing Table
 */

#ifndef DEFS_DTCASTSROUTINGTABLE_HH_DTCAST
#define DEFS_DTCASTSROUTINGTABLE_HH_DTCAST

#include <click/list.hh>

struct dtcast_srouting_tuple_t
{
	node_t	_src_id;
	node_t	_next_id;
	age_t	_last_update;
	
	List_member<dtcast_srouting_tuple_t> link;
	
	dtcast_srouting_tuple_t( node_t src_id, node_t next_id ) //, age_t last_update )
			: _src_id(src_id),_next_id(next_id)//,_last_update(last_update)
	{
		update( );
	}
	
	bool operator==( const dtcast_srouting_tuple_t &tuple )
	{
		return	tuple._src_id ==_src_id && 
				tuple._next_id==_next_id;
	}
	
	void update( )
	{
		/// @todo update age field !!!
	}	
};

class DtcastSroutingTable
{
public:
	void addOrUpdate( node_t src_id, node_t next_id )
	{
		dtcast_srouting_tuple_t *tuple=new dtcast_srouting_tuple_t( src_id,next_id );
		table_t::iterator test=find( _table.begin(), _table.end(), *tuple );
		if( test!=_table.end() )
		{
			test->update( );
			delete tuple;
		}
		else
			_table.push_back( tuple );
	}
	
	~DtcastSroutingTable( )
	{
		purge<dtcast_srouting_tuple_t>( _table );
	}

private:
	typedef List<dtcast_srouting_tuple_t,&dtcast_srouting_tuple_t::link> table_t;
	table_t _table;
};

#endif
