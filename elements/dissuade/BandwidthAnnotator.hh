/**
	@file Declaration of the Dropper element
*/

#ifndef BANDWIDTH_ANNOTATOR_HH
#define BANDWIDTH_ANNOTATOR_HH

#include <click/element.hh>
#include <click/timer.hh>
CLICK_DECLS

class BandwidthAnnotator : public Element
{
public:
	BandwidthAnnotator( );
	
	virtual int initialize( ErrorHandler* );
	const char *class_name() const { return "BandwidthAnnotator"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return "h/h"; }
	
	int configure( Vector<String>&, ErrorHandler* );
	void push( int, Packet* );

protected:
	void run_timer( Timer * );
	
private:
	void updateBandwidth( uint32_t length );
	inline double a( double delta );
	
private:
	double _tau;
	
	double _bk;		///< Bandwidth measurement
	double _bk_1;	///< Previous bandwidth measurement
	double _b;		///< Bandwidth estimation
	double _b_1;	///< Previous bandwidth estimation
	
	Timestamp _tk_1;
	
	Timer _timer;
};

CLICK_ENDDECLS
#endif DROPPER_H
