/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "Cache.hh"
CLICK_DECLS

#error "This module should not be compiled. DEPRECATED"

Packet* DtcastCache::simple_action( Packet *pkt )
{
//	DtcastPacket *dpkt=DtcastPacket::make( pkt );
//	if( pkt==NULL ) { pkt->kill(); return NULL; }
//	
//	if( get(dpkt->dtcast()->_src,dpkt->dtcast()->_mcast,
//			dpkt->dtcast()->_type,dpkt->dtcast()->_seq) )
//	{
//		pkt->kill( );
//		return NULL;
//	}
	
	return pkt;
}


CLICK_ENDDECLS
//EXPORT_ELEMENT(DtcastCache)
