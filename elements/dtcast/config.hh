/**
    @file   Constant definitions for DTCAST implementation
*/

#ifndef CONFIG_HH_DTCAST
#define CONFIG_HH_DTCAST


/**
 * Maximum age for tuples in cache
 */
#define CACHE_TIME_TO_LIVE		60
		

#define ROUTE_REQUEST_TIME      5  ///< Period of the route discovery while there is some data in message queue
#define ROUTE_REQUEST_MAXAGE	20 ///< == Source Routing Table record max age


#define ROUTE_REPLY_TIME        5  ///< Period of the route table retransmitting
#define ROUTE_REPLY_MAXAGE		20 ///< == Source Routing Table record max age


#define MESSAGE_QUEUE_CHECK_PERIOD	10 ///< Period for checking data messages actuality in queue

#define TIMEOUT_DATA_RETRANSMIT     5  ///< Data packet retransmit timeout (TIMEOUT 1) after epidemic data dissemination stage

#define LOCAL_RECOVERY_START	7	  ///< Timeout to start local recovery
#define LOCAL_RECOVERY_PERIOD   1	  ///< Period of LocalRecovery Route Requests
#define LOCAL_RECOVERY_END		20	  ///< Maximum timeout for local recovery stage (including recovery start timeout) 
									  ///< should be less or equal to #ROUTE_REQUEST_MAXAGE

/**
    
*/
#define TIMEOUT_ER_START        2	  ///< Timeout to start epidemic data delivery (TIMEOUT 2)
#define TIMEOUT_ER_RESEND		5	  ///< Timeout to epidemicly resend data packet
#define TIMEOUT_ER_STOP			10	  ///< Maximum time in seconds to perform epidemic data dissemination

/**
    Epidemic data transfer maximum TTL
    ER acks should have TTL=ER_TTL*2
*/
#define ER_TTL  2


#endif
