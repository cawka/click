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
	List_member<dtcast_srouting_tuple_t> link;
	
	dtcast_srouting_tuple_t( node_t src_id, node_t next_id )
			: _src_id(src_id),_next_id(next_id)
	{ }
	
	bool operator==( const dtcast_srouting_tuple_t &tuple )
	{
		return	tuple._src_id ==_src_id && 
				tuple._next_id==_next_id;
	}
};

class DtcastSRoutingTable : public AgeTable<dtcast_srouting_tuple_t,ROUTE_REQUEST_MAXAGE>
{
};

#endif
