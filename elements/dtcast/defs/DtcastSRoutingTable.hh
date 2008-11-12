/**
 *	@file Definition of the DTCAST Source Routing Table
 */

#ifndef DEFS_DTCASTSROUTINGTABLE_HH_DTCAST
#define DEFS_DTCASTSROUTINGTABLE_HH_DTCAST

#include "AgeTable.hh"

struct dtcast_srouting_tuple_t : public age_tuple_t<ROUTE_REQUEST_MAXAGE>
{
	node_t	_src_id;
	node_t	_next_id;
//	List_member<dtcast_srouting_tuple_t> link;
	
	dtcast_srouting_tuple_t( node_t src_id, node_t next_id )
			: _src_id(src_id),_next_id(next_id)
	{ }
	
	void update( age_tuple_t<ROUTE_REQUEST_MAXAGE> &tuple )
	{
		_next_id=((dtcast_srouting_tuple_t&)tuple)._next_id;
		
		age_tuple_t<ROUTE_REQUEST_MAXAGE>::update( tuple );
	}
	
	bool isEqualSource( node_t src ) const
	{
		return src==_src_id;
	}
};

inline StringAccum& operator<<(StringAccum &os,const dtcast_srouting_tuple_t &t)
{
	return os << "src="  << t._src_id << ",next="  << t._next_id
			  << ",remains=" << (ROUTE_REQUEST_MAXAGE+(t._last_update-Timestamp::now()).sec()) << "sec";
}

struct srouting_key_t
{
	srouting_key_t( node_t src_id )
			: _src_id(src_id)
	{ }

	srouting_key_t( const dtcast_srouting_tuple_t &tuple )
			: _src_id(tuple._src_id)
	{ }
	
	hashcode_t hashcode( ) const
	{
		return _src_id;
	}

	bool operator==( const srouting_key_t &tuple ) const
	{
		return tuple._src_id==_src_id;
	}
	
	node_t _src_id;
};

class DtcastSRoutingTable : public AgeTable<srouting_key_t,dtcast_srouting_tuple_t,ROUTE_REQUEST_MAXAGE>
{
public:
	DtcastSRoutingTable() { _label="SROUTING"; _debug=false; }
};

#endif
