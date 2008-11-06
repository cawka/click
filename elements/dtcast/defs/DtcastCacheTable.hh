/**
 *	@file	Definition of the CacheTable
 */

#ifndef DEFS_DTCASTCACHETABLE_HH_DTCAST
#define DEFS_DTCASTCACHETABLE_HH_DTCAST

#include <click/list.hh>
#include <click/algorithm.hh>

struct dtcast_cache_tuple_t
{
	node_t		_src_id;
	mcast_t		_mcast_id;
	uint16_t	_type;
	uint32_t	_seq;
	
	List_member<dtcast_cache_tuple_t> link;
	
	dtcast_cache_tuple_t( node_t src_id, mcast_t mcast_id, uint16_t type, uint32_t seq )
			: _src_id(src_id),_mcast_id(mcast_id),_type(type),_seq(seq)
	{
		
	}
	
	bool operator==( const dtcast_cache_tuple_t &tuple ) const
	{
		return	tuple._src_id  ==_src_id		&&
				tuple._mcast_id==_mcast_id		&&
				tuple._type    ==_type			&&
				tuple._seq     ==_seq;
	}
};


class DtcastCacheTable
{
public:
	bool hasTuple( node_t src_id, mcast_t mcast_id, uint16_t type, uint32_t seq )
	{
		dtcast_cache_tuple_t *tuple=new dtcast_cache_tuple_t( src_id,mcast_id,type,seq );
		
		bool found=findTuple( *tuple );
		if( found )
			delete tuple;
		else
		{
			if( _table.size()>=CACHE_TABLE_MAX_SIZE )
			{
				dtcast_cache_tuple_t *tmp=_table.front( );
				_table.pop_front( );
				delete tmp;
			}
			_table.push_back( tuple );
		}
		
		return found;
	}
	
	~DtcastCacheTable( )
	{
		purge<dtcast_cache_tuple_t>( _table );
	}
	
private:
	bool findTuple( const dtcast_cache_tuple_t &tuple )
	{
		/**
		 *	@todo Optimize cache searching
		 */
		return find( _table.begin(),_table.end(),tuple )!=_table.end( );
	}
	
private:
	typedef List<dtcast_cache_tuple_t,&dtcast_cache_tuple_t::link> list_t;
	list_t _table;
};

#endif
