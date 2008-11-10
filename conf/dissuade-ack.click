FromDevice(eth0) 
	-> class :: Classifier( 12/0800, - )

class[0]
	-> Strip(14)
	-> CheckIPHeader
	-> ipclass :: IPClassifier( tcp, - )

ipclass[0] 
	-> TCPFake( ACK_PROB 0.001 )
	-> out

class[1]
	-> out

ipclass[1]
	-> out

out :: ToDevice(eth1)
