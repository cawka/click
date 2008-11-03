/**
 *	@file	DTCAST RouteRequest packet definition
 */

#ifndef DEFS_DTCASTRTPACKET_HH_DTCAST
#define DEFS_DTCASTRTPACKET_HH_DTCAST

#include "DtcastPacket.hh"

/**
 *	DTCAST RouteReply packet
 */
class DtcastRTPacket : public DtcastPacket
{
	static DtcastRTPacket* make( IPAddress from, IPAddress to,
			uint32_t seq, 
			node_t src, node_t next, nodelist_t dsts )
	{
		DtcastRTPacket *pkt=static_cast<DtcastRTPacket*>( DtcastPacket::make(
				from, to, DTCAST_RT_TTL, 
				DTCAST_TYPE_RT, seq, 
				sizeof(node_t)+dsts.size()*sizeof(node_t)) );

		unsigned char *data=pkt->dtcast_payload( );
		memcpy( data,&src,sizeof(node_t) );
		uint16_t offset=sizeof(node_t);
		
		memcpy( data+offset,&next,sizeof(node_t) );
		offset+=sizeof(node_t);
		
		for( nodelist_t::iterator i=dsts.begin(); i!=dsts.end(); i++ )
		{
			memcpy( data+offset,&(*i),sizeof(node_t) );
			offset+=sizeof(node_t);
		}
		return pkt;
	}
	
	static DtcastRTPacket* make( Packet *orig )
	{
		DtcastPacket *dtcast=DtcastPacket::make( orig );
		/** 
		 *	@todo Start using DebugLogger
		 */
		if( dtcast==NULL ) return NULL;
		
		DtcastRTPacket *rt=static_cast<DtcastRTPacket*>( dtcast );
		if( rt->dtcast()->_length<2*sizeof(node_t) ) return NULL; /** @todo Add error printing */

		return rt;
	}
public: //non-static methods
	node_t src_id( )  { return *( (node_t*)dtcast_payload() ); }
	node_t next_id( ) { return *( (node_t*)(dtcast_payload()+sizeof(node_t)) ); }
	
	nodelist_t dst_ids( )
	{
		nodelist_t ret;
		for( uint16_t offset=2*sizeof(node_t); offset<dtcast()->_length; offset+=sizeof(node_t) )
		{
			ret.push_back( *( (node_t*)(dtcast_payload()+offset) ) );
		}
		return ret;
	}
};

#endif
