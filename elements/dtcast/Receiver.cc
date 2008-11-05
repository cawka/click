/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>

#include "Receiver.hh"
CLICK_DECLS



int DtcastReceiver::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
//			"DTCAST_SOURCE",   cpkPositional, cpElement, &_source,
//			"DTCAST_RECEIVER", cpkPositional, cpElement, &_receiver,
					cpEnd );
}


CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastReceiver)
