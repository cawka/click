/**
 *	@file	DTCAST RouteRequest packet definition
 */

#ifndef DEFS_DTCASTRRPACKET_HH_DTCAST
#define DEFS_DTCASTRRPACKET_HH_DTCAST

#include "DtcastPacket.hh"

/**
 *	DTCAST RouteRequest packet
 */
class DtcastRRPacket : public DtcastPacket
{
	static DtcastRRPacket* make( IPAddress from, IPAddress to,
			uint32_t seq, 
			node_t src, nodelist_t dsts )
	{
		DtcastRRPacket *pkt=static_cast<DtcastRRPacket*>( DtcastPacket::make(
				from, to, DTCAST_RR_TTL, 
				DTCAST_TYPE_RR, seq, 
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
	
	static DtcastRRPacket* make( Packet *orig )
	{
		DtcastPacket *dtcast=DtcastPacket::make( orig );
		/** 
		 *	@todo Start using DebugLogger
		 */
		if( dtcast==NULL ) return NULL;
		
		DtcastRRPacket *rr=static_cast<DtcastRRPacket*>( dtcast );
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
