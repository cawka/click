/**
 *	@file	DTCAST Data Acknowledgement packet definition
 */

#ifndef DEFS_DTCASTACKPACKET_HH_DTCAST
#define DEFS_DTCASTACKPACKET_HH_DTCAST

#include "DtcastPacket.hh"
#include <click/error.hh>

/**
 *	DTCAST RouteRequest packet
 */
class DtcastAckPacket : public DtcastPacket
{
public:
	static DtcastAckPacket* make( node_t src,mcast_t mcast, node_t from,
			uint32_t seq, 
			nodelist_t dsts, 
			bool epidemic=false )
	{
		if( dsts.size()==0 ) return NULL;
		
		DtcastAckPacket *pkt=static_cast<DtcastAckPacket*>( DtcastPacket::make(
				src,mcast,from, epidemic?DTCAST_ERACK_TTL:DTCAST_ACK_TTL, 
				epidemic?DTCAST_TYPE_ERACK:DTCAST_TYPE_ACK, seq, 
				dsts.size()*sizeof(node_t)) );

		unsigned char *data=pkt->dtcast_payload( );
		uint16_t offset=0;
		for( nodelist_t::iterator i=dsts.begin(); i!=dsts.end(); i++ )
		{
			memcpy( data+offset,&(*i),sizeof(node_t) );
			offset+=sizeof(node_t);
		}
		return pkt;
	}
	
	static DtcastAckPacket* make( DtcastPacket *dtcast )
	{
		DtcastAckPacket *rr=static_cast<DtcastAckPacket*>( dtcast );
		if( rr->dtcast()->_type!=DTCAST_TYPE_ERACK && 
			rr->dtcast()->_type!=DTCAST_TYPE_ACK )
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: not ACK packet type" );
			rr->kill( );
			return NULL;
		}
		if( rr->dtcast()->_length<sizeof(node_t) ) // at least one DST_ID is required
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: incorrect ACK packet size" );
			rr->kill( );
			return NULL;
		}

		if( rr->dtcast()->_length<sizeof(node_t) ) return NULL; /** @todo Add error printing */

		return rr;
	}
	
public: //non-static methods
	nodelist_t dst_ids( )
	{
		nodelist_t ret;
		for( uint16_t offset=0; offset<dtcast()->_length; offset+=sizeof(node_t) )
		{
			ret.push_back( *( (node_t*)(dtcast_payload()+offset) ) );
		}
		return ret;
	}
};

#endif
