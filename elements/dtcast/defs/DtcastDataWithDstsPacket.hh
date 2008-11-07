/**
 *	@file	DTCAST Data packet definition
 */

#ifndef DEFS_DTCASTDATAWITHDSTSPACKET_HH_DTCAST
#define DEFS_DTCASTDATAWITHDSTSPACKET_HH_DTCAST

#include "DtcastPacket.hh"
#include "DtcastMessageQueue.hh"

/**
 *	DTCAST Data packet
 */
class DtcastDataWithDstsPacket : public DtcastDataPacket
{ //packets are compatible
public:
	static DtcastDataWithDstsPacket* make( node_t src,mcast_t mcast, node_t from,
			uint32_t seq, 
			age_t age, 
			const unsigned char *body, uint16_t body_len,
			bool epidemic,
			const nodelist_t &dsts
			)
	{
		DtcastDataWithDstsPacket *pkt=static_cast<DtcastDataWithDstsPacket*>( DtcastPacket::make(
				src,mcast,from, epidemic?DTCAST_ERDATA_TTL:DTCAST_DATA_TTL, 
				epidemic?DTCAST_TYPE_ERDATA:DTCAST_TYPE_DATA, seq, 
				sizeof(node_t)+sizeof(age_t)+sizeof(uint16_t)+body_len+dsts.size()*sizeof(node_t)) );

		unsigned char *data=pkt->dtcast_payload( );

		memcpy( data,&age,sizeof(age_t) );
		uint16_t offset=sizeof(node_t);

		*((uint16_t*)(data+offset))=body_len;
		offset+=sizeof(uint16_t);
		
		memcpy( data+offset,body,body_len );
		offset+=body_len;
		
		for( nodelist_t::const_iterator i=dsts.begin(); i!=dsts.end(); i++ )
		{
			memcpy( data+offset,&(*i),sizeof(node_t) );
			offset+=sizeof(node_t);
		}
		return pkt;
	}
	
	static DtcastDataWithDstsPacket* make( DtcastPacket *dtcast )
	{
		return static_cast<DtcastDataWithDstsPacket*>( DtcastDataPacket::make(dtcast) );
	}
	
	static DtcastDataWithDstsPacket* make( DtcastDataPacket *d, const nodelist_t &dsts )
	{
		return make( d->dtcast()->_src, d->dtcast()->_mcast, d->dtcast()->_from,
				d->dtcast()->_seq,
				d->age(),
				d->body(), d->body_len(),
				d->dtcast()->_type==DTCAST_TYPE_ERDATA,
				dsts
				);
	}
	
	
public: //non-static methods
	nodelist_t dsts( ) //i hope, that it will work
	{
		nodelist_t ret;
		for( uint16_t offset=sizeof(age_t)+sizeof(uint16_t)+body_len(); 
			 offset<dtcast()->_length; offset+=sizeof(node_t) )
		{
			ret.push_back( *( (node_t*)(dtcast_payload()+offset) ) );
		}
		return ret;
	}
};

#endif
