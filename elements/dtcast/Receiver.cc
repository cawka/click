/**
 *	Definition of the Receiver NODE in DTCAST Network
 */

#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>

#include "Receiver.hh"
CLICK_DECLS

int DtcastReceiver::initialize( ErrorHandler * )
{
	_localRecoveryTimer.initialize( this );
	return 0;
}

int DtcastReceiver::configure( Vector<String> &conf, ErrorHandler *errH )
{
	return cp_va_kparse( conf,this,errH,
			"NODE",  cpkPositional, cpInteger, &_me,
			"MCAST", cpkPositional, cpInteger, &_mcast,
					cpEnd );
}

void DtcastReceiver::push( int, Packet *pkt )
{
	/**
	 * Just to make sure, that we have received DTCAST data packet
	 */
	DtcastPacket *dpkt=DtcastPacket::make( pkt );
	if( dpkt==NULL ) return;
	if( dpkt->dtcast()->_mcast!=_mcast ) return pkt->kill( ); //we are not interested in such data packet
	
	switch( dpkt->dtcast()->_type )
	{
		case DTCAST_TYPE_DATA:
			onDataPacket( DtcastDataPacket::make(dpkt) );
			break;
		case DTCAST_TYPE_RR:
			onRouteRequest( DtcastRRPacket::make(dpkt) );
			break;
		default:
			pkt->kill( );
			ErrorHandler::default_handler()->fatal( "DTCAST: DtcastReceiver has received not serviced packet type" );
			return;
	}
}

void DtcastReceiver::onDataPacket( DtcastDataPacket *pkt )
{
	if( pkt==NULL ) return; //will be fatal error	
	scheduleLocalRecovery( pkt );
	
	output( DATA ).push( pkt->data_payload() ); // Element should be connected to IPClassify
}

//void DtcastReceiver::onErDataPacket( DtcastDataPacket *pkt )
//{
//	if( pkt==NULL ) return; //will be fatal error
//	scheduleLocalRecovery( pkt );
//	
//	output( DATA ).push( pkt->data_payload() ); // Element should be connected to IPClassify
//	
//	output( FORWARDER ).push( DtcastAckPacket::make(pkt->dtcast()->_src,
//											  pkt->dtcast()->_mcast,
//											  DTCAST_NODE_SELF,
//											  pkt->dtcast()->_seq,
//											  nodelist_t().add(_me),
//											  true) );
//}

void DtcastReceiver::onRouteRequest( DtcastRRPacket *pkt )
{
	if( pkt==NULL ) return;
	scheduleLocalRecovery( pkt );

	// special DTCAST RouteReply packet. NEXT field is set to invalid value, DST field is set to SELF node,
	// which will indicate local delivery for forwarding table
	output( FORWARDER ).push( DtcastRTPacket::make(pkt->dtcast()->_src,pkt->dtcast()->_mcast,
				DTCAST_NODE_SELF,_seq++,DTCAST_NODE_SELF,nodelist_t().add( _me )) );
	pkt->kill( );
}

void DtcastReceiver::scheduleLocalRecovery( DtcastPacket *pkt )
{
	_lastSrc=pkt->dtcast()->_src;
	_lastRRorData=Timestamp::now( );
	_localRecoveryTimer.schedule_after_sec( LOCAL_RECOVERY_START );
}

void DtcastReceiver::onLocalRecovery( Timer *timer )
{
	output( FORWARDER ).push( DtcastRTPacket::make(_lastSrc,_mcast,
				DTCAST_NODE_SELF,_seq++,DTCAST_NODE_ALL,nodelist_t().add( _me )) );
	
	if( (Timestamp::now()-_lastRRorData)<LOCAL_RECOVERY_END )
		timer->reschedule_after_sec( LOCAL_RECOVERY_PERIOD );
}

void DtcastReceiver::run_timer( Timer *timer )
{
	if( timer==&_localRecoveryTimer )
	{
		onLocalRecovery( timer );
	}
}


CLICK_ENDDECLS
EXPORT_ELEMENT(DtcastReceiver)
