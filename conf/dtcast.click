//cache :: DtcastCache
AddressInfo(
//  self 	  131.179.192.175,
  self 	  127.0.0.1,
  mcast	  224.1.1.1
);

elementclass UpdateIPHeader
{
	$src, $dst |
	input -> StoreIPAddress($src,12)
		  -> StoreIPAddress($dst,16)
		  -> SetIPChecksum

//		  -> CheckIPHeader -> IPPrint("DTCAST (IP)", CONTENTS NONE)
		  -> output;
}

src :: DtcastSource( 2,131 )
dst :: DtcastReceiver

fwd :: DtcastForwarder( 2, src, dst )

FromDevice( eth1 ) -> 
	class :: Classifier( 12/0800 21/8a,
						 12/0800 );

class[0]
	-> DtcastPrint("mcast->fwd")
	-> fwd

class[1] 
	-> src // Encapsulate IP packets into DTCAST
	-> UpdateIPHeader(127.0.0.1, 255.255.255.255) 
	-> DtcastPrint("src->fwd  ") 
	-> fwd
//	-> Queue 
//	-> ToDevice( lo );
//	Strip(14) -> CheckIPHeader ->
//	cache -> fwd

fwd 
	-> DtcastPrint("fwd->mcast")
	-> Discard





//-> cache

//fwd ->
//	Discard
