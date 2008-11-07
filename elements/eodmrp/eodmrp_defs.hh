/*
 * Eodmrp_defs.hh -- Define Eodmrp values and eodmrp packet header
 * Soon Y. Oh
 *
 * Copyright (c) 2006 University of California, Los Angeles
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#ifndef EODMRP_DEFS_HH
#define EODMRP_DEFS_HH

#include <click/ipaddress.hh>

// mode types
#define MODE uint8_t
#define SOURCE_MODE 1
#define FORRECEIVER_MODE 2
#define RECEIVER_MODE 3
#define FORWARDER_MODE 4
#define TEMPFORWARDER_MODE 5
#define LISTENER_MODE 6

// Eodmrp packet types
#define JQ 1
#define JR 2
#define LR 3
#define RR 4
#define DUMMY 5

// success and fail
#define EODMRP_SUCCESS 0
#define EODMRP_FAIL -1

/*******************************************************************************************
 *                                                                                         *
 * e-odmrp headers are below                                                               *
 *                                                                                         *
 *******************************************************************************************/
 
struct eodmrp_header{
  uint8_t type_;
  uint16_t seqno_;
  uint8_t hop_count_;
  IPAddress src_;
  IPAddress multicast_;
  IPAddress prev_hop_addr_;
  IPAddress sender_addr_;
  double datasendinginterval_;
  int refresh_interval_;
};

typedef eodmrp_header EodmrpHeader;

/*******************************************************************************************
 *                                                                                         *
 * IGMP protocol headers below                                                             *
 *                                                                                         *
 *******************************************************************************************/
/*
// IGMPv1 and IGMPv2 messages have to be supported by an IGMPv3 router
struct igmpv1andv2message {
  unsigned char type;
  unsigned char responsetime;
  unsigned short checksum;
  unsigned int group;
};  

// the query is used to detect other routers and the state of connected hosts
struct igmpv3querie {
  unsigned char type;
  unsigned char responsecode;
  unsigned short checksum;
  unsigned int group;
  unsigned char s_and_qrv;
  unsigned char qqic;
  unsigned short no_of_sources;
  unsigned int sources[1];
};

// see RFC 3376 for details
struct grouprecord {
  unsigned char type;
  unsigned char aux_data_len;
  unsigned short no_of_sources;
  unsigned int multicast_address;
  unsigned int sources[1];
};

struct igmpv3report {
  unsigned char type;
  unsigned char reserved;
  unsigned short checksum;
  unsigned short reserved_short;
  unsigned short no_of_grouprecords;
  struct grouprecord grouprecords[1];
};
*/

#endif
