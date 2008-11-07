/**
 *	@file Definition of the DTCAST Forwarding Table
 */

#ifndef DEFS_DTCASTDSTS_HH_DTCAST
#define DEFS_DTCASTDSTS_HH_DTCAST

#include "AgeTable.hh"

struct dtcast_dst_tuple_t : public age_tuple_t<ROUTE_REPLY_MAXAGE>
{
	node_t  _dst_id;
	bool    _fw_flag;
	
	dtcast_dst_tuple_t( node_t dst_id, bool fw_flag )
			: _dst_id(dst_id),_fw_flag(fw_flag)
	{ }
};

inline StringAccum& operator<<( StringAccum &os,const dtcast_dst_tuple_t &t )
{
	return os << ">>>>>> dst="  << t._dst_id  << ",flag="  << t._fw_flag
			  << ",remains=" << (ROUTE_REQUEST_MAXAGE+(t._last_update-Timestamp::now()).sec()) << "sec";
}

struct dst_key_t
{
	dst_key_t( const dtcast_dst_tuple_t &tuple )
			: _dst_id(tuple._dst_id)
	{ }
	
	dst_key_t( node_t node )
			: _dst_id(node)
	{ }

	hashcode_t hashcode( ) const
	{
		return _dst_id;
	}

	bool operator==( const dst_key_t &tuple ) const
	{
		return	tuple._dst_id ==_dst_id;
	}
	
	node_t  _dst_id;
};

class DtcastDstsTable : public AgeTable<dst_key_t,dtcast_dst_tuple_t,ROUTE_REPLY_MAXAGE>
{
public:
	DtcastDstsTable( ) { _label=" >>> DSTS"; _debug=false; }
	
};

#endif
