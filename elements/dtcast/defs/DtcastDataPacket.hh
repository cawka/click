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
	static DtcastDataPacket* make( IPAddress from, IPAddress to,
			uint32_t seq, 
			node_t src, age_t age, 
			const unsigned char *body, uint16_t body_len,
			bool epidemic=false )
	{
		DtcastDataPacket *pkt=static_cast<DtcastDataPacket*>( DtcastPacket::make(
				from, to, epidemic?DTCAST_ERDATA_TTL:DTCAST_DATA_TTL, 
				epidemic?DTCAST_TYPE_ERDATA:DTCAST_TYPE_DATA, seq, 
				sizeof(node_t)+sizeof(age_t)+body_len) );

		unsigned char *data=pkt->dtcast_payload( );
		memcpy( data,&src,sizeof(node_t) );
		uint16_t offset=sizeof(node_t);
		
		memcpy( data+offset,&age,sizeof(age_t) );
		offset+=sizeof(node_t);

		memcpy( data+offset,body,body_len );
		
		return pkt;
	}
	
	static DtcastRRPacket* make( Packet *orig )
	{
		DtcastPacket *dtcast=DtcastPacket::make( orig );
		/** 
		 *	@todo Start using DebugLogger
		 */
		if( dtcast==NULL ) return NULL;
		
		DtcastRRPacket *rr=static_cast<DtcastRRPacket*>( dtcast );
		if( rr->dtcast()->_length<(sizeof(node_t)+sizeof(age_t)) ) return NULL; /** @todo Add error printing */

		return rr;
	}
	
public: //non-static methods
	node_t src_id( ) { return *( (node_t*)dtcast_payload() ); }
	age_t  age( )    { return *( (age_t*)(dtcast_payload() )+sizeof(node_t)); }
	
	const unsigned char* body() { return dtcast_payload()+sizeof(node_t)+sizeof(age_t); }
	uint16_t body_len() { return dtcast()->_length-( sizeof(node_t)+sizeof(age_t) ); }
};

#endif
