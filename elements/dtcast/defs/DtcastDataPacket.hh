/**
 *	@file	DTCAST Data packet definition
 */

#ifndef DEFS_DTCASTDATAPACKET_HH_DTCAST
#define DEFS_DTCASTDATAPACKET_HH_DTCAST

#include "DtcastPacket.hh"
#include "DtcastMessageQueue.hh"

/**
 *	DTCAST Data packet
 */
class DtcastDataPacket : public DtcastPacket
{
public:
	static DtcastDataPacket* make( node_t src,mcast_t mcast, node_t from,
			uint32_t seq, 
			age_t age, 
			const unsigned char *body, uint16_t body_len,
			bool epidemic=false )
	{
		DtcastDataPacket *pkt=static_cast<DtcastDataPacket*>( DtcastPacket::make(
				src,mcast,from, epidemic?DTCAST_ERDATA_TTL:DTCAST_DATA_TTL, 
				DTCAST_TYPE_DATA, epidemic?DTCAST_FLAG_EPIDEMIC:0, seq, 
				sizeof(age_t)+sizeof(uint16_t)+body_len) );

		unsigned char *data=pkt->dtcast_payload( );

		memcpy( data,&age,sizeof(age_t) );
		uint16_t offset=sizeof(age_t);

		*((uint16_t*)(data+offset))=body_len;
		offset+=sizeof(uint16_t);

		memcpy( data+offset,body,body_len );
		
		return pkt;
	}
	
	static DtcastDataPacket* make( dtcast_message_t &msg )
	{
		return make( msg._src_id ,msg._mcast_id,DTCAST_NODE_SELF,//msg._from,
				msg._seq,
				!msg._epidemic?msg._actual_till.sec():(Timestamp::now()+(msg._actual_till-Timestamp::now())/10).sec(),
				msg._data, msg._data_len,
				msg._epidemic
				);
	}
	
	static DtcastDataPacket* make( DtcastPacket *dtcast )
	{
		DtcastDataPacket *rr=static_cast<DtcastDataPacket*>( dtcast );
		
		if( rr->dtcast()->_type!=DTCAST_TYPE_DATA )
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: not DATA packet type" );
			rr->kill( );
			return NULL;
		}
		if( rr->dtcast()->_length<(sizeof(age_t)+sizeof(uint16_t)) ) 
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: incorrect DATA packet size" );
			rr->kill( );
			return NULL;
		}

		return rr;
	}
	
	Packet* data_payload( )
	{
		this->pull( sizeof(click_ip)+sizeof(dtcast_header_t)+1 );
		return this;
	}
	
public: //non-static methods
	age_t  age( )    { return *( (age_t*)dtcast_payload() ); }
	
	const unsigned char* body() { return dtcast_payload()+sizeof(age_t)+sizeof(uint16_t); }
	uint16_t body_len() { return *( (uint16_t*)(dtcast_payload()+sizeof(age_t)) ); }
};

#endif
