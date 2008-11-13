ControlSocket( "TCP", 9999 )

src :: FromDevice( en1 )
//	-> Classifier( 12/0800 )
//	-> Strip(14) 
//	-> CheckIPHeader
//	-> IPClassifier( ip tcp dst 131.179.32.87 and port 80 )
//	-> IPPrint
//	-> BandwidthAnnotator
	-> Discard

