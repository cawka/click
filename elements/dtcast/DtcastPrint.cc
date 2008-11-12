/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "DtcastPrint.hh"
CLICK_DECLS


int DtcastPrint::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"LABEL",   cpkPositional, cpString, &_label,
					cpEnd );
}

Packet* DtcastPrint::simple_action( Packet *pkt )
{
	DtcastPacket *dpkt=DtcastPacket::make( pkt );
	if( dpkt==NULL ) return pkt;
	
	switch( dpkt->dtcast()->_type )
	{
		case DTCAST_TYPE_RR:
			onRouteRequest( DtcastRRPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_RT:
			onRouteReply( DtcastRTPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_DATA:
			onData( DtcastDataPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_ACK:
			onAck( DtcastAckPacket::make(dpkt) );
			break;
		default:
			ErrorHandler::default_handler()->fatal( "DTCAST: unknown packet type (%d)",dpkt->dtcast()->_type );
			pkt->kill( );
			break;
	}
	return pkt;
}

void DtcastPrint::onRouteRequest( DtcastRRPacket *pkt )
{
	StringAccum os;
	os << _label << ": RouteRequest : " 
			<< "seq="   << pkt->dtcast()->_seq 
			<< ",src="   << pkt->dtcast()->_src 
			<< ",mcast=" << pkt->dtcast()->_mcast
			<< ",from="  << pkt->dtcast()->_from;
	ErrorHandler::default_handler()->message( os.c_str() );	
}

void DtcastPrint::onRouteReply( DtcastRTPacket *pkt )
{
	StringAccum os;
	os << _label << ": RouteReply : " 
			<< "seq="   << pkt->dtcast()->_seq 
			<< ",src="   << pkt->dtcast()->_src 
			<< ",mcast=" << pkt->dtcast()->_mcast
			<< ",from="  << pkt->dtcast()->_from
			<< " ### "
			<< "next="  << pkt->next_id()
			<< ",dst={"  << pkt->dst_ids() << "}";

	ErrorHandler::default_handler()->message( os.c_str() );	
}

void DtcastPrint::onData( DtcastDataPacket *pkt )
{
	StringAccum os;
	os << _label << ": DATA" << (pkt->dtcast()->_flags&DTCAST_FLAG_EPIDEMIC?"(ER)":"") << " : " 
			<< "seq="   << pkt->dtcast()->_seq 
			<< ",src="   << pkt->dtcast()->_src 
			<< ",mcast=" << pkt->dtcast()->_mcast
			<< ",from="  << pkt->dtcast()->_from
			<< " ### "
			<< "age="   << pkt->age()-Timestamp::now().sec() << "sec"
			<< ",datalen="  << pkt->body_len();

	ErrorHandler::default_handler()->message( os.c_str() );		
}

void DtcastPrint::onAck( DtcastAckPacket *pkt )
{
	StringAccum os;
	os << _label << ": ACK" << (pkt->dtcast()->_flags&DTCAST_FLAG_EPIDEMIC?"(ER)":"") << " : "
			<< "seq="   << pkt->dtcast()->_seq 
			<< ",src="   << pkt->dtcast()->_src 
			<< ",mcast=" << pkt->dtcast()->_mcast
			<< ",from="  << pkt->dtcast()->_from
			<< " ### "
			<< "dst={"  << pkt->dst_ids() << "}";

	ErrorHandler::default_handler()->message( os.c_str() );		
}


CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastPrint)
