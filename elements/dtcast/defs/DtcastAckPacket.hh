/**
 *	@file	DTCAST Data Acknowledgement packet definition
 */

#ifndef DEFS_DTCASTACKPACKET_HH_DTCAST
#define DEFS_DTCASTACKPACKET_HH_DTCAST

#include "DtcastPacket.hh"

/**
 *	DTCAST RouteRequest packet
 */
class DtcastAckPacket : public DtcastPacket
{
	static DtcastAckPacket* make( IPAddress from, IPAddress to,
			uint32_t seq, 
			node_t src, nodelist_t dsts, 
			bool epidemic=false )
	{
		DtcastAckPacket *pkt=static_cast<DtcastAckPacket*>( DtcastPacket::make(
				from, to, epidemic?DTCAST_ERACK_TTL:DTCAST_ACK_TTL, 
				epidemic?DTCAST_TYPE_ERACK:DTCAST_TYPE_ACK, seq, 
				sizeof(node_t)+dsts.size()*sizeof(node_t)) );

		unsigned char *data=pkt->dtcast_payload( );
		memcpy( data,&src,sizeof(node_t) );
		uint16_t offset=sizeof(node_t);
		for( nodelist_t::iterator i=dsts.begin(); i!=dsts.end(); i++ )
		{
			memcpy( data+offset,&(*i),sizeof(node_t) );
			offset+=sizeof(node_t);
		}
		return pkt;
	}
	
	static DtcastAckPacket* make( Packet *orig )
	{
		DtcastPacket *dtcast=DtcastPacket::make( orig );
		/** 
		 *	@todo Start using DebugLogger
		 */
		if( dtcast==NULL ) return NULL;
		
		DtcastAckPacket *rr=static_cast<DtcastAckPacket*>( dtcast );
		if( rr->dtcast()->_length<sizeof(node_t) ) return NULL; /** @todo Add error printing */

		return rr;
	}
	
public: //non-static methods
	node_t src_id( ) { return *( (node_t*)dtcast_payload() ); }
	
	nodelist_t dst_ids( )
	{
		nodelist_t ret;
		for( uint16_t offset=sizeof(node_t); offset<dtcast()->_length; offset+=sizeof(node_t) )
		{
			ret.push_back( *( (node_t*)(dtcast_payload()+offset) ) );
		}
		return ret;
	}
};

#endif
