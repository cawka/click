//cache :: DtcastCache
AddressInfo(
//  self 	  131.179.192.175,
  self 	  127.0.0.1,
  mcast	  224.1.1.1
);

elementclass UpdateIPHeader
{
	$src, $dst |
	input -> StoreIPAddress($src,12)     //src
		  -> StoreIPAddress($dst,16) //dst
		  -> SetIPChecksum
		  -> CheckIPHeader
		  -> output;
}

src :: DtcastSource(   NODE 1,  MCAST 131, AGE 3600, DST 2 3 ) //node 3 is somewhere in the network
dst :: DtcastReceiver( NODE 2,  MCAST 131 )

fwd :: DtcastForwarder( NODE 3, ACTIVE_ACK true)

FromDevice( en1 ) -> 
        class :: Classifier( 12/0800 21/8a, //DTCAST protocol
                             12/0800 );

class[0] // DTCAST routine
	-> Strip(14) -> CheckIPHeader
	-> DtcastPrint("mcast->fwd")
	-> fwd

class[1] // DATA packets (start tunnel)
	-> Strip(14) -> CheckIPHeader
	-> IPClassifier(icmp) // just use only ICMP packets for data, discard all other packets
	-> IPPrint("DATA->src:", CONTENTS NONE)
	-> src // Encapsulate IP packets into DTCAST
	-> UpdateIPHeader(127.0.0.1, 255.255.255.255) 
	-> DtcastPrint("src->fwd  ") 
	-> fwd

fwd[0]
	-> DtcastPrint( "fwd->mcast" )
	-> Discard

fwd[1] 
	-> DtcastPrint( "fwd->dst  " )
	-> dst

dst[0] // DATA packets (end tunnel)
	-> IPPrint( "dst->DATA:",CONTENTS NONE )
	-> Discard

dst[1] 
	-> DtcastPrint( "dst1->fwd " )
	-> fwd

fwd[2]	
	-> DtcastPrint( "fwd->src" )
	-> [1]src


