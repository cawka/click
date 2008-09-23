/**
	@file Declaration of the Dropper element
*/

#ifndef DROPPER_HH
#define DROPPER_HH

#include <click/element.hh>
CLICK_DECLS

#ifdef CLICK_LINUXMODULE
#include <linux/random.h>
#endif

class Dropper : public Element
{
public:
	Dropper();
	~Dropper(){}
	
	const char *class_name() const { return "Dropper"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return AGNOSTIC; }
	
	int configure( Vector<String>&, ErrorHandler* );
	Packet* simple_action( Packet* );

protected:
	Packet* processTcpDrop( Packet* );
	Packet* processTcpReset( Packet* );

private:
	inline int rand();

private:
// Configuration parameters, which specify probabilities of certain attacks
	//int _icmpFakeUnreach; 	///< Faking ICMP unreachability

	int _tcpDrop; 			///< Simple TCP packet drop
	
	int _tcpReset;			///< Reset TCP connection
	int _tcpLongReset;		///< Reset long lived TCP connections
	int _tcpShortReset;		///< Reset short lived TCP connections	
	
	int _tcpSlidingWnd;		///< Decreasing TCP sliding window
	int _tcpScaling;			///< Forge TCP window scaling header field
	int _tcpForgeAck;		///< Forging ACK number in the TCP packet(s)

	int _tcpSeqDrop;			///< ? sequence drop period ?
	/*
	?
	arp packet shuffling
	imitation of the congestion control - attacking particular congestion control algorithms
	?
	*/
};

int Dropper::rand( )
{
#ifndef CLICK_LINUXMODULE
	return rand()%100;
#else
	return random32()%100;
#endif
}

CLICK_ENDDECLS
#endif DROPPER_H
