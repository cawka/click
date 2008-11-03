/**
 *	@file	DTCAST general packet definitions
 */

#ifndef DEFS_DTCASTPACKET_HH_DTCAST
#define DEFS_DTCASTPACKET_HH_DTCAST

#include <click/ipaddress.hh>
#include <click/packet.hh>


/**
 * DTCAST header
 */
struct dtcast_header_t
{
	uint16_t _type;
	uint16_t _length;
	uint32_t _seq;
};

/**
 *	DTCAST general packet
 */
class DtcastPacket : public WritablePacket
{
//default constructor is disabled in Packet class
public: //static methods
	static DtcastPacket* make( IPAddress src, IPAddress dst, uint8_t ttl,
			uint16_t type, uint32_t seq, 
			uint16_t length ) // length==payload_length
	{
		
		DtcastPacket *pkt=static_cast<DtcastPacket*>( Packet::make(sizeof(click_ip)+
																   sizeof(dtcast_header_t)+length) );
		dtcast_header_t header;
		header._type=type; 
		header._length=length;
		header._seq=seq;
		
		formatIPHeader( pkt, ttl,src,dst );
		memcpy( pkt->data()+sizeof(click_ip),&header,sizeof(header) );
		
		return pkt;
	}
	
	static void formatIPHeader( DtcastPacket *pkt, IPAddress src, IPAddress dst, uint8_t ttl )
	{
		click_ip* ip=reinterpret_cast<click_ip *>( pkt->data() );
		ip->ip_p=IP_PROTO_DTCAST;
		ip->ip_len = htons( pkt->length()-sizeof(*ip) );

		ip->ip_src=src.in_addr();
		ip->ip_dst=dst.in_addr();
		
		ip->ip_len = htons( pkt->length() );
		ip->ip_hl = sizeof(click_ip) >> 2;
		ip->ip_v = 4;
		ip->ip_ttl = ttl;
	}
	
	static DtcastPacket* make( Packet *orig )
	{
		if( orig->length()<sizeof(click_ip)+sizeof(dtcast_header_t) ) return NULL;
		DtcastPacket *pkt=static_cast<DtcastPacket*>( orig );
		
		if( pkt->ip_header()->ip_p!=IP_PROTO_DTCAST ) return NULL;
		if( pkt->dtcast()->_length!=pkt->length()-sizeof(click_ip)-sizeof(dtcast_header_t) ) return NULL;
		
		return pkt;
	}
	
	static Packet* finalizePacket( DtcastPacket *pkt )
	{
		click_ip* ip=reinterpret_cast<click_ip *>( pkt->data() );
		ip->ip_sum = click_in_cksum( (unsigned char *)ip, sizeof(*ip) );
		return pkt;
	}
	
public: //non-static methods
	unsigned char *dtcast_payload( ) { return data()+sizeof(click_ip)+sizeof(dtcast_header_t); }
	dtcast_header_t* dtcast( ) const { return reinterpret_cast<dtcast_header_t*>( data()+sizeof(click_ip) ); };
	
	IPAddress src_ip( ) { return IPAddress( ip_header()->ip_src );  }
	IPAddress dst_ip( ) { return IPAddress( ip_header()->ip_dst );  }
	uint8_t   ttl( ) { return ip_header()->ip_ttl; }
};


#endif
