/**
    @file   Constant definitions for DTCAST implementation
*/

#ifndef CONFIG_HH_DTCAST
#define CONFIG_HH_DTCAST


/**
 * Maximum age for tuples in cache
 */
#define CACHE_TIME_TO_LIVE		60
		
/**
    Default age of the data messages
*/
#define MESSAGE_DEFAULT_AGE     3600    /* 1hour */


/**
 *	Period of the route discovery while there is some data in message queue
*/
#define ROUTE_REQUEST_TIME      5      
#define ROUTE_REQUEST_MAXAGE	10 ///< == Source Routing Table record max age

/**
    Period of the route table retransmitting
*/
#define ROUTE_REPLY_TIME        5
#define ROUTE_REPLY_MAXAGE		10 ///< == Source Routing Table record max age


#define MESSAGE_QUEUE_CHECK_PERIOD	10 ///< Perdio for checking data messages actuality in queue
/**
    Data packet retransmit timeout (TIMEOUT 1)
*/
#define TIMEOUT_1_DATA_RETRANSMIT   5

/**
    Maximum retransmission count
*/
#define RETRANSMISSION_COUNT        3


/**
    Timeout to start epidemic data delivery (TIMEOUT 2)
*/
#define TIMEOUT_2_ER_START      20


/**
    Epidemic data transfer maximum TTL
    ER acks should have TTL=ER_TTL*2
*/
#define ER_TTL  2

/**
    Timeout to become temporary source node for data message (TIMEOUT 3)
*/
#define TIMEOUT_3_NEWSOURCE     60

/**
 *	CacheTable maximum size
 */
#define CACHE_TABLE_MAX_SIZE	1024

#endif
