/**
 *	@file Definition of the DTCAST Forwarding Table
 */

#ifndef DEFS_DTCASTFORWARDINGTABLE_HH_DTCAST
#define DEFS_DTCASTFORWARDINGTABLE_HH_DTCAST

#include "AgeTable.hh"
#include "DtcastDstsTable.hh"

struct dtcast_fwd_tuple_t : public age_tuple_t<ROUTE_REPLY_MAXAGE>
{
	mcast_t _mcast_id;
	node_t	_src_id;
	DtcastDstsTable _dsts;
	
	dtcast_fwd_tuple_t( mcast_t mcast_id, node_t src_id )// , node_t dst_id, bool fw_flag )
			: _mcast_id(mcast_id),_src_id(src_id)//,_dst_id(dst_id),_fw_flag(fw_flag)
	{ 
	}

	bool canPurge( Timestamp )
	{
		_dsts.purgeOldRecords( NULL );
		return _dsts.size()==0;
	}
	
	bool needForward( ) const
	{
		for( DtcastDstsTable::const_iterator it=_dsts.begin(); it!=_dsts.end(); it++ ) if( it->second->_fw_flag ) return true;
		return false;
	}
	
	bool needLocalDelivery( ) const
	{
		for( DtcastDstsTable::const_iterator it=_dsts.begin(); it!=_dsts.end(); it++ ) if( !it->second->_fw_flag ) return true;
		return false;
	}
	
	nodelist_t local_dsts( ) const { return _dsts.local_dsts(); }
};

inline StringAccum& operator<<(StringAccum &os,const dtcast_fwd_tuple_t &t)
{
	return os << "mcast=" << t._mcast_id << ",src="  << t._src_id << "\n"
			  << t._dsts;
}

struct fwd_key_t
{
	fwd_key_t( const dtcast_fwd_tuple_t &tuple )
			: _mcast_id(tuple._mcast_id)
			, _src_id(tuple._src_id)
	{ }
	
	fwd_key_t( mcast_t mcast_id, node_t src_id )
			: _mcast_id(mcast_id),_src_id(src_id)
	{ }

	hashcode_t hashcode( ) const
	{
		return (_src_id^(_mcast_id<<16)) ^ (0xFFFF&_mcast_id);
	}

	bool operator==( const fwd_key_t &tuple ) const
	{
		return	tuple._src_id ==_src_id && 
				tuple._mcast_id==_mcast_id;
	}
	
	mcast_t _mcast_id;
	node_t	_src_id;
};

class DtcastForwardingTable : public AgeTable<fwd_key_t,dtcast_fwd_tuple_t,ROUTE_REPLY_MAXAGE>
{
public:
	DtcastForwardingTable( ) { _label="FORWARDING"; _debug=true; }
};

#endif
