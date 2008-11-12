AddressInfo(
  self 	  127.0.0.1 00:1f:5b:d1:65:e5,
  mcast	  224.1.1.1 00:1f:5b:d1:65:e5
);

elementclass UpdateIPHeader
{
	$src, $dst |

	input -> StoreIPAddress($src, 12)     //src
		  -> StoreIPAddress($dst, 16)		//dst
		  -> SetIPChecksum
		  -> CheckIPHeader
		  -> output;
}

src :: DtcastSource(   NODE 1,  MCAST 131, DST 1 2, AGE 3600 )
dst :: DtcastReceiver( NODE 1,  MCAST 131 )

fwd :: DtcastForwarder( NODE 1, ACTIVE_ACK true)

FromDevice( en1 ) -> 
       class :: Classifier( 12/0800 21/8a, //DTCAST protocol
                            12/0800 );

class[0] // DTCAST routine
	-> Strip(14) -> CheckIPHeader
	//-> DtcastPrint("mcast->fwd")
	-> fwd

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//
//
class[1] // DATA packets (start tunnel)
	-> Strip(14) -> CheckIPHeader
	-> IPClassifier(icmp dst 164.67.226.176) // just use only ICMP packets for data, discard all other packets
	-> IPPrint("DATA->src:", CONTENTS NONE)
	-> [1]src // Encapsulate IP packets into DTCAST
//
//
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
	
src
	//-> don't care about IP addreses
	//-> DtcastPrint("src->fwd  ") 
	-> fwd

fwd[0]
	-> DtcastPrint( "fwd->mcast" )
	-> UpdateIPHeader( 164.67.226.176, 164.67.226.176 )
	-> IPPrint("DATA->src:", CONTENTS NONE)
	-> EtherEncap(0x0800, self, mcast)
	-> Discard
	//-> Queue
	//-> ToDevice( en1 )

fwd[1] 
	-> DtcastPrint( "fwd->dst  " )
	-> dst

dst[1] // DATA packets (end tunnel)
//	-> IPPrint( "dst->DATA:",CONTENTS NONE )
	-> Discard

dst[0] 
	-> DtcastPrint( "dst1->fwd " )
	-> fwd

fwd[2]	
	-> DtcastPrint( "fwd->src" )
	-> [0]src
