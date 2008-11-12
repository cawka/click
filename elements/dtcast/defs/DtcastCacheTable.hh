/**
 *	@file	Definition of the CacheTable
 */

#ifndef DEFS_DTCASTCACHETABLE_HH_DTCAST
#define DEFS_DTCASTCACHETABLE_HH_DTCAST

#include <click/list.hh>
#include <click/algorithm.hh>

#include "DtcastPacket.hh"

struct dtcast_cache_tuple_t : public age_tuple_t<CACHE_TIME_TO_LIVE>
{
	node_t		_src_id;
	mcast_t		_mcast_id;
	node_t		_from_id;
	uint16_t	_type;
	uint32_t	_seq;
	
	dtcast_cache_tuple_t( node_t src_id, mcast_t mcast_id, node_t from_id, 
						  uint16_t type, uint32_t seq )
			:_src_id(src_id)
			,_mcast_id(mcast_id)
			,_from_id(from_id)
			,_type(type)
			,_seq(seq)
	{
	}

	dtcast_cache_tuple_t( const DtcastPacket &pkt )
			:_src_id(pkt.dtcast()->_src)
			,_mcast_id(pkt.dtcast()->_mcast)
			,_from_id(pkt.dtcast()->_from)
			,_type(pkt.dtcast()->_type)
			,_seq(pkt.dtcast()->_seq)
	{ }
};

inline StringAccum& operator<<( StringAccum &os,const dtcast_cache_tuple_t &t )
{
	return os << "type=" << t._type 
			  << ",seq=" << t._seq << ",mcast=" << t._mcast_id << ",src="  << t._src_id
			  << ",remains=" << (ROUTE_REQUEST_MAXAGE+(t._last_update-Timestamp::now()).sec()) << "sec";
}

struct cache_key_t
{
	cache_key_t( const dtcast_cache_tuple_t &tuple )
			:_src_id(tuple._src_id)
			,_mcast_id(tuple._mcast_id)
			,_type(tuple._type)
//			,_type(tuple._type==DTCAST_TYPE_ERDATA?DTCAST_TYPE_DATA:tuple._type)
			,_seq(tuple._seq)
	{ }
	
	cache_key_t( node_t src_id, mcast_t mcast_id, uint16_t type, uint32_t seq )
			:_src_id(src_id)
			,_mcast_id(mcast_id)
			,_type(type)
//			,_type(type==DTCAST_TYPE_ERDATA?DTCAST_TYPE_DATA:type)
			,_seq(seq)
	{ }
	
	cache_key_t( DtcastPacket &pkt )
			:_src_id(pkt.dtcast()->_src)
			,_mcast_id(pkt.dtcast()->_mcast)
			,_type(pkt.dtcast()->_type)
//			,_type(pkt.dtcast()->_type==DTCAST_TYPE_ERDATA?DTCAST_TYPE_DATA:pkt.dtcast()->_type)
			,_seq(pkt.dtcast()->_seq)
	{ }

	hashcode_t hashcode( ) const
	{
		return ((_src_id<<8)^(_mcast_id<<16)) ^ (0xFFFF&_mcast_id) ^ _seq ^ (_type<<24);
	}

	bool operator==( const cache_key_t &tuple ) const
	{
		return	tuple._src_id  ==_src_id		&&
				tuple._mcast_id==_mcast_id		&&
				tuple._type    ==_type			&&
//				(tuple._type==DTCAST_TYPE_ERDATA?DTCAST_TYPE_DATA:tuple._type)==_type&&
				tuple._seq     ==_seq;
	}
	
	node_t		_src_id;
	mcast_t		_mcast_id;
	uint16_t	_type;
	uint32_t	_seq;
};


class DtcastCacheTable : public AgeTable<cache_key_t,dtcast_cache_tuple_t,CACHE_TIME_TO_LIVE>
{
public:
	DtcastCacheTable(){ _label="CACHE", _debug=false; }
	
	bool receivedFromDifferentNode( DtcastPacket &pkt )
	{
		dtcast_cache_tuple_t *t=get( cache_key_t(pkt) );
		if( !t ) 
			this->addOrUpdate( new dtcast_cache_tuple_t(pkt) );
		else
			pkt.dtcast()->_flags|=DTCAST_FLAG_DUPLICATE;
		return t && t->_from_id!=pkt.dtcast()->_from;
	}
};

#endif
