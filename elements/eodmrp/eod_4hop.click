//EODMRP click script

//enable eodmrp elements 
require(eodmrp);

// Ip address information
//AddressInfo(my_addr 131.179.136.46/24 00:08:74:3E:66:E1);
AddressInfo(my_addr 131.179.25.11/24 00:02:2D:3C:21:FA);

//define output
//out0 :: Queue(100) -> todevice0 :: ToDevice(eth0);
out1 :: Queue(100) -> todevice1 :: ToDevice(eth1);

// Eodmrp member and routing table
eodmemtb :: EodmrpMemberTable(1, 230.10.10.10);
eodroutb :: EodmrpRoutingTable();

eodagent :: EodmrpAgent(eodmemtb, eodroutb, 131.179.25.11);

rt :: RadixIPLookup(
	224.0.0.0/255.255.255.0 0,
	230.10.10.10/255.255.255.255 2,
	131.179.0.0/255.255.0.0 1,
	127.0.0.0/255.0.0.0 1,
	0.0.0.0/32 1);

checkd :: CheckDupPkt(131.179.25.11);
etherclaasify :: Classifier(6/00022D74F980, 6/00022D2E3B1F,-);
	
//mcc :: IPClassifier(224.0.0.0/4 and ip proto udp, ip proto igmp, -);

FromDevice(eth1)->etherclaasify;
etherclaasify[0]->Discard;
etherclaasify[1]->Discard;
etherclaasify[2]
		->HostEtherFilter(00:02:2D:3C:21:FA, DROP_OWN true, DROP_OTHER false)
		->Strip(14)
		->Align(4,0)
		->ip_check::CheckIPHeader(CHECKSUM true, DETAILS true)
		->checkd;

ip_check[1] -> Print("Bad IP") ->Discard;

checkd[1] -> Discard;
checkd[0] -> dt::DecIPTTL() -> eodagent;
dt[1] -> Discard;

eodagent[0] -> Discard;
eodagent[1]	-> EtherEncap(0x800,00:02:2D:3C:21:FA, ff:ff:ff:ff:ff:ff)
						-> rt;
eodagent[2] -> UDPIPEncap(131.179.25.11, 999, 224.0.0.13, 998)
						-> EtherEncap(0x800,00:02:2D:3C:21:FA, ff:ff:ff:ff:ff:ff) 
						-> out1;						

rt[0] -> out1;
rt[1] -> out1;
rt[2] -> out1;
