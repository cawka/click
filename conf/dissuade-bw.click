
src :: FromDevice( en1, SNIFFER true )
//RandomSource(100)
	-> Classifier( 12/0800 )
	-> Strip(14) 
	-> CheckIPHeader
//	-> IPClassifier( ip tcp dst 131.179.32.87 and port 80 )
//	-> IPPrint
	-> BandwidthAnnotator
	-> Discard

ControlSocket( "TCP", 9999, VERBOSE true )
