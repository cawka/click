/**
 *	@file Basic definitions for DTCAST
 */

#ifndef DTCAST_HH_DTCAST
#define DTCAST_HH_DTCAST

/**
 *  Protocol number for DTCAST
*/
#define IP_PROTO_DTCAST     138

/**
 * DTCAST packet types
 */
#define DTCAST_TYPE_RR		0
#define DTCAST_TYPE_RT		1
#define DTCAST_TYPE_DATA	2
#define DTCAST_TYPE_ACK		3
#define DTCAST_TYPE_ERDATA	4
#define DTCAST_TYPE_ERACK	5

#define DTCAST_RR_TTL	255
#define DTCAST_RT_TTL	255
#define DTCAST_DATA_TTL	1
#define DTCAST_ACK_TTL  1
#define DTCAST_ERDATA_TTL 2
#define DTCAST_ERACK_TTL  4

#include <click/packet.hh>
#include <click/vector.hh>

typedef uint32_t node_t;
typedef uint32_t age_t;
typedef Vector<uint32_t> nodelist_t;

#include "defs/DtcastPacket.hh"
#include "defs/DtcastRRPacket.hh"
#include "defs/DtcastRTPacket.hh"
#include "defs/DtcastDataPacket.hh"
#include "defs/DtcastAckPacket.hh"


#endif
