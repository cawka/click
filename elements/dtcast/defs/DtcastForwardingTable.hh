/**
 *	@file Definition of the DTCAST Forwarding Table
 */

#ifndef DEFS_DTCASTFORWARDINGTABLE_HH_DTCAST
#define DEFS_DTCASTFORWARDINGTABLE_HH_DTCAST

#include "AgeTable.hh"

struct dtcast_fwd_tuple_t : public age_tuple_t<ROUTE_REPLY_MAXAGE>
{
	mcast_t _mcast_id;
	node_t	_src_id;
	node_t  _dst_id;
	bool    _fw_flag;
//	List_member<dtcast_fwd_tuple_t> link;
	
	dtcast_fwd_tuple_t( mcast_t mcast_id, node_t src_id, node_t dst_id, bool fw_flag )
			: _mcast_id(mcast_id),_src_id(src_id),_dst_id(dst_id),_fw_flag(fw_flag)
	{ }
	
	bool operator==( const dtcast_fwd_tuple_t &tuple ) const
	{
		return	tuple._src_id ==_src_id && 
				tuple._mcast_id==_mcast_id &&
				tuple._dst_id==_dst_id;
	}
	
	hashcode_t hashcode( ) const
	{
		return (_src_id^(_mcast_id<<16)) ^ (0xFFFF&_mcast_id);
	}
};

class DtcastForwardingTable : public AgeTable<dtcast_fwd_tuple_t,ROUTE_REPLY_MAXAGE>
{
	
};

#endif
