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

src :: DtcastSource(   1,  131 )
dst :: DtcastReceiver( 2,  131 )

fwd :: DtcastForwarder( 3, src, dst )

FromDevice( eth1 ) -> 
	class :: Classifier( 12/0800 21/8a,
						 12/0800,
						 12/fff0 ); //dum rule

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

class[2] -> dst
dst[0] -> DtcastPrint("dst0->fwd ")->Discard

dst[1] -> DtcastPrint("dst1->fwd ")-> fwd



//-> cache

//fwd ->
//	Discard

