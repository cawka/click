/**
 * @file Definition of the DtcastMcastTable
 */

#ifndef DTCASTMCASTTABLE_HH_DEFS
#define DTCASTMCASTTABLE_HH_DEFS

#include "AgeTable.hh"

struct mcast_tuple_t : public age_tuple_t<ROUTE_REQUEST_MAXAGE>
{
	mcast_t _mcast;

	mcast_tuple_t( dtcast_message_t &mcast ) : _mcast(mcast._mcast_id) { }
};

inline StringAccum& operator<<( StringAccum &os, const mcast_tuple_t &t )
{
	return os << "mcast=" << t._mcast
			  << ",remains=" << (ROUTE_REQUEST_TIME+(t._last_update-Timestamp::now()).sec()) << "sec";
}

struct mcast_key_t
{
	mcast_t _mcast;
	
	mcast_key_t( dtcast_message_t &mcast ) : _mcast(mcast._mcast_id) { }
	mcast_key_t( mcast_tuple_t &tuple ) : _mcast(tuple._mcast) { }
	
	operator mcast_t()    const { return _mcast; }	
	hashcode_t hashcode() const { return _mcast; }
};

class DtcastMcastTable : public AgeTable<mcast_key_t,mcast_tuple_t,ROUTE_REQUEST_MAXAGE>
{
public:
	DtcastMcastTable() { _label="MCASTS"; _debug=false; }
};

#endif
