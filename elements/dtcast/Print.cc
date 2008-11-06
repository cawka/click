/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "Print.hh"
CLICK_DECLS


static String TYPES[]={
"RR",
"RT",
"DATA",
"ACK",
"ERDATA",
"ERACK"
};
		
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
	
	ErrorHandler::default_handler()->message( "%s: type=%s,len=%d,seq=%d,src=%d,mcast=%d,from=%d",_label.c_str(),
			TYPES[dpkt->dtcast()->_type].c_str(),dpkt->dtcast()->_length,
			dpkt->dtcast()->_seq, dpkt->dtcast()->_src,
			dpkt->dtcast()->_mcast, dpkt->dtcast()->_from );
	return pkt;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastPrint)
