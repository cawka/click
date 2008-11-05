src :: DtcastSource
dst :: DtcastReceiver

fwd :: DtcastForwarder
//( src, dst )

FromDevice( eth1 ) ->
	Classifier( 12/0800 ) ->
	Strip(14) -> CheckIPHeader ->

	fwd -> Discard
