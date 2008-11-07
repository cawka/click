/**
 *	@file Definition of the DTCAST Data Message Queue
 */

#ifndef DEFS_DTCASTMESSAGEQUEUE_HH_DTCAST
#define DEFS_DTCASTMESSAGEQUEUE_HH_DTCAST

#include "AgeTable.hh"

/**
 * Data Message structure
 *
 * There is an assumption, that we have enough memory to store all 
 * data messages in memory and that we use UserMode click interface
 */
struct dtcast_message_t
{
	mcast_t _mcast_id;
	node_t	_src_id;
	node_t  _from;
	uint32_t _seq;
	Timestamp _actual_till;
	nodelist_t _unack_ids;
	unsigned char *_data;
	uint16_t _data_len;
	bool _epidemic;
	
//	List_member<dtcast_message_t> link;
	
	dtcast_message_t( 
						node_t src_id, 
						mcast_t mcast_id, 
						node_t from,
						uint32_t seq,
						Timestamp actual_till, 
						const nodelist_t &dst_ids,
						const unsigned char *data, 
						uint16_t data_len,
						bool epidemic=false )
			: _mcast_id(mcast_id)
			,_src_id(src_id)
			,_from(from)
			,_seq(seq)
			,_actual_till(actual_till)
			,_unack_ids(dst_ids)
			,_data_len(data_len)
			,_epidemic(epidemic)
	{
		_data=new unsigned char[_data_len];
		memcpy( _data, data, _data_len );
	}
	
	~dtcast_message_t( )
	{
		delete _data;
	}
	
	void update( dtcast_message_t& ) { }
	
	bool canPurge( Timestamp ref ) { return ref>_actual_till; };
};

inline StringAccum& operator<<(StringAccum &os,const dtcast_message_t &t)
{
	return os << "seq=" << t._seq << ",mcast=" << t._mcast_id << ",src="  << t._src_id
			  << ",valid="  << (t._actual_till - Timestamp::now()).sec() << "sec";
}


struct msg_key_t
{
	msg_key_t( const dtcast_message_t &tuple )
			:
			_src_id(tuple._src_id)
			,_mcast_id(tuple._mcast_id)
			,_seq(tuple._seq)
	{
	}

	bool operator==( const msg_key_t &tuple ) const
	{
		return	tuple._src_id ==_src_id && 
				tuple._mcast_id==_mcast_id &&
				tuple._seq==_seq;
	}

	hashcode_t hashcode( ) const
	{
		return ((_src_id<<8)^(_mcast_id<<16)) ^ (0xFFFF&_mcast_id) ^ _seq;
	}

	node_t	_src_id;
	mcast_t _mcast_id;
	uint32_t _seq;
};


class DtcastMessageQueue : public AgeTable<msg_key_t,dtcast_message_t,MESSAGE_QUEUE_CHECK_PERIOD>
{
public:
	DtcastMessageQueue() { _label="MQUEUE"; _debug=false; }
	
};

#endif
