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
	IPAddress	_mcast_ip;
	uint16_t	_type;
	uint32_t	_seq;
	
	List_member<dtcast_cache_tuple_t> link;
	
	dtcast_cache_tuple_t( node_t src_id, IPAddress mcast_ip, uint16_t type, uint32_t seq )
			: _src_id(src_id),_mcast_ip(mcast_ip),_type(type),_seq(seq)
	{
		
	}
	
	bool operator==( const dtcast_cache_tuple_t &tuple ) const
	{
		return	tuple._src_id  ==_src_id		&&
				tuple._mcast_ip==_mcast_ip		&&
				tuple._type    ==_type			&&
				tuple._seq     ==_seq;
	}
};


class DtcastCacheTable
{
public:
	bool hasTuple( node_t src_id, IPAddress mcast_ip, uint16_t type, uint32_t seq )
	{
		dtcast_cache_tuple_t *tuple=new dtcast_cache_tuple_t( src_id,mcast_ip,type,seq );
		
		bool found=findTuple( *tuple );
		if( found )
			delete tuple;
		else
		{
			if( _table.size()>=CACHE_TABLE_MAX_SIZE )
			{
				dtcast_cache_tuple_t *tmp=_table.front( );
				_table.pop_front( );
				delete tmp; // i'm not sure that is was safe to delete _table.fron() before pop_front()
							// example shows 3-stage purging process
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
	/**
	 *	@todo Optimize cache searching
	 */
	bool findTuple( const dtcast_cache_tuple_t &tuple )
	{
//		for( list_t::iterator i=_table.begin(); i!=_table.end(); i++ )
//		{
//			if( *i==tuple ) return true;
//		}
		return find( _table.begin(),_table.end(),tuple )!=_table.end( );
	}
	
private:
	typedef List<dtcast_cache_tuple_t,&dtcast_cache_tuple_t::link> list_t;
	list_t _table;
};

#endif
