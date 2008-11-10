
#ifndef TCPFORGER_HH_DISSUADE
#define TCPFORGER_HH_DISSUADE

#include <click/element.hh>

class TCPForger : public Element 
{
public:
	TCPForger( );
	
	const char *class_name() const { return "TCPForger"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return "h/h"; }
	
	int configure( Vector<String>&, ErrorHandler* );
	void push( int, Packet* );
	
private:
	uint32_t _ackProbability;
};

#endif
