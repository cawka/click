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
	uint32_t _seq;
	Timestamp _actual_till;
	nodelist_t _unack_ids;
	unsigned char *_data;
	uint16_t _data_len;
	bool _epidemic;
	
	List_member<dtcast_message_t> link;
	
	dtcast_message_t( mcast_t mcast_id, node_t src_id, uint32_t seq,
			Timestamp actual_till, const nodelist_t &dst_ids,
			const unsigned char *data, uint16_t data_len,
			bool epidemic=false)
			: _mcast_id(mcast_id),_src_id(src_id),
			_seq(seq),_actual_till(actual_till),_unack_ids(dst_ids),
			_data_len(data_len),
			_epidemic(epidemic)
	{
		_data=new unsigned char[_data_len];
		memcpy( _data, data, _data_len );
	}
	
	~dtcast_message_t( )
	{
		delete _data;
	}
	
	bool operator==( const dtcast_message_t &tuple )
	{
		return	tuple._src_id ==_src_id && 
				tuple._mcast_id==_mcast_id &&
				tuple._seq==_seq;
	}
	void update( ) { }
	
	bool canPurge( Timestamp ref ) { return ref>_actual_till; };
};

class DtcastMessageQueue : public AgeTable<dtcast_message_t,MESSAGE_QUEUE_CHECK_PERIOD>
{
	
};

#endif
