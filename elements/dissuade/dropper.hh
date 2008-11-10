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

/**
Ports:
	- input 0: TCP packet
	- output 0: TCP packet downstream
	- output 1: TCP packet upstream
	- output 2: ARP packet downstream
	- output 3: ARP packet upstream
	- output 4: Dismissed legal TCP packets
*/

class Dropper : public Element
{
public:
	Dropper();
	~Dropper(){}
	
	const char *class_name() const { return "Dropper"; }
	const char *port_count() const { return "1/1-"; }
	const char *processing() const { return PUSH; }
	
	int configure( Vector<String>&, ErrorHandler* );
	//Packet* simple_action( Packet* );
	void push( int, Packet* );

protected:
	void processTcpDrop( Packet *pkt );
	void processTcpReset( Packet *pkt );
	
	void killPacket( Packet *pkt );

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
	
	static ErrorHandler *myLogger;
};

#define PROB_GRANUL		1000000000

int Dropper::rand( )
{
	return click_random() % PROB_GRANUL;
#ifndef CLICK_LINUXMODULE
	return rand()%PROB_GRANUL;
#else
	return random32()%PROB_GRANUL;
#endif
}

CLICK_ENDDECLS
#endif DROPPER_H
