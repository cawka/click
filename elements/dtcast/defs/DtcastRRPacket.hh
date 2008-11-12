/**
 *	@file	DTCAST RouteRequest packet definition
 */

#ifndef DEFS_DTCASTRRPACKET_HH_DTCAST
#define DEFS_DTCASTRRPACKET_HH_DTCAST

#include "DtcastPacket.hh"
#include "DtcastSRoutingTable.hh"
#include <click/error.hh>

/**
 *	DTCAST RouteRequest packet
 */
class DtcastRRPacket : public DtcastPacket
{
public:
	static DtcastRRPacket* make( node_t src,mcast_t mcast, node_t from,
			uint32_t seq )
	{
		DtcastRRPacket *pkt=static_cast<DtcastRRPacket*>( DtcastPacket::make(
				src,mcast,from, DTCAST_RR_TTL, 
				DTCAST_TYPE_RR, 0,seq,0) );

		return pkt;
	}
	
	static DtcastRRPacket* make( DtcastPacket *dtcast )
	{
		DtcastRRPacket *rr=static_cast<DtcastRRPacket*>( dtcast );
		if( rr->dtcast()->_type!=DTCAST_TYPE_RR )
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: not RouteRequest packet type" );
			rr->kill( );
			return NULL;
		}
		if( rr->dtcast()->_length!=0 ) 
		{
			ErrorHandler::default_handler()->fatal( "DTCAST: incorrect RouteRequest packet size" );
			rr->kill( );
			return NULL;
		}

		return rr;
	}
	
public: //non-static methods
};

#endif
