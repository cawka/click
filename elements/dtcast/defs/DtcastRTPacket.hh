/**
 *	@file	DTCAST RouteRequest packet definition
 */

#ifndef DEFS_DTCASTRTPACKET_HH_DTCAST
#define DEFS_DTCASTRTPACKET_HH_DTCAST

#include "DtcastPacket.hh"
#include <click/error.hh>

/**
 *	DTCAST RouteReply packet
 */
class DtcastRTPacket : public DtcastPacket
{
public:
	static DtcastRTPacket* make( node_t src,mcast_t mcast,
			uint32_t seq, 
			node_t next, nodelist_t dsts )
	{
		DtcastRTPacket *pkt=static_cast<DtcastRTPacket*>( DtcastPacket::make(
				src,mcast, DTCAST_RT_TTL, 
				DTCAST_TYPE_RT, seq, 
				sizeof(node_t)+dsts.size()*sizeof(node_t)) );

		unsigned char *data=pkt->dtcast_payload( );
		
		memcpy( data,&next,sizeof(node_t) );
		uint16_t offset=sizeof(node_t);
		
		for( nodelist_t::iterator i=dsts.begin(); i!=dsts.end(); i++ )
		{
			memcpy( data+offset,&(*i),sizeof(node_t) );
			offset+=sizeof(node_t);
		}
		return pkt;
	}
	
	static DtcastRTPacket* make( DtcastPacket *dtcast )
	{
		DtcastRTPacket *rt=static_cast<DtcastRTPacket*>( dtcast );
		if( rt->dtcast()->_type!=DTCAST_TYPE_RT )
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: not RouteReply packet type" );
			rt->kill( );
			return NULL;
		}

		if( rt->dtcast()->_length<sizeof(node_t) ) 
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: incorrect RouteReply packet size" );
			rt->kill( );
			return NULL;
		}

		return rt;
	}
public: //non-static methods
	node_t next_id( ) { return *( (node_t*)dtcast_payload() ); }
	
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
