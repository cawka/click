/**
 *	@file	DTCAST Data packet definition
 */

#ifndef DEFS_DTCASTDATAPACKET_HH_DTCAST
#define DEFS_DTCASTDATAPACKET_HH_DTCAST

#include "DtcastPacket.hh"

/**
 *	DTCAST Data packet
 */
class DtcastDataPacket : public DtcastPacket
{
	static DtcastDataPacket* make( node_t src,mcast_t mcast,
			uint32_t seq, 
			age_t age, 
			const unsigned char *body, uint16_t body_len,
			bool epidemic=false )
	{
		DtcastDataPacket *pkt=static_cast<DtcastDataPacket*>( DtcastPacket::make(
				src,mcast, epidemic?DTCAST_ERDATA_TTL:DTCAST_DATA_TTL, 
				epidemic?DTCAST_TYPE_ERDATA:DTCAST_TYPE_DATA, seq, 
				sizeof(node_t)+sizeof(age_t)+body_len) );

		unsigned char *data=pkt->dtcast_payload( );
		
		memcpy( data,&age,sizeof(age_t) );
		uint16_t offset=sizeof(node_t);

		memcpy( data+offset,body,body_len );
		
		return pkt;
	}
	
	static DtcastRRPacket* make( DtcastPacket *dtcast )
	{
		DtcastRRPacket *rr=static_cast<DtcastRRPacket*>( dtcast );
		
		if( rr->dtcast()->_type!=DTCAST_TYPE_ERDATA && 
			rr->dtcast()->_type!=DTCAST_TYPE_DATA )
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: not DATA packet type" );
			rr->kill( );
			return NULL;
		}
		if( rr->dtcast()->_length<sizeof(age_t) ) 
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: incorrect DATA packet size" );
			rr->kill( );
			return NULL;
		}

		return rr;
	}
	
public: //non-static methods
	age_t  age( )    { return *( (age_t*)dtcast_payload() ); }
	
	const unsigned char* body() { return dtcast_payload()+sizeof(age_t); }
	uint16_t body_len() { return dtcast()->_length-( sizeof(age_t) ); }
};

#endif
