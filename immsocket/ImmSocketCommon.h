#ifndef _IMM_SOCKET_COMMON_H_
#define _IMM_SOCKET_COMMON_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include "Utils.h"

using namespace std;


namespace ImmSocket {

#define DEFAULT_IMMSOCKET_ENDPOINT_PATH		"./socket_endpoint"
#define DEFAULT_TCP_SERVER_PORT				((uint16_t)65000)
#define DEFAULT_TCP_SERVER_ADDR				"127.0.0.1"

#define RECEIVED_SIZE_MAX					(4096)
#define RECEIVE_PACKET_ONCE_TIMEOUT_MAX		(0x05265C00) // msec 24h

#define SOH									(0x01) // start of heading
#define EOT									(0x04) // end of transfer
#define ACK									(0x06)
#define NAK									(0x15)


typedef enum {
	EN_RECEIVE_STATE_STANDBY__WAIT_SOH,
	EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_0,
	EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_1,
	EN_RECEIVE_STATE_WORKING__CHECK_SIZE,
	EN_RECEIVE_STATE_WORKING__CHECK_DATA,
	EN_RECEIVE_STATE_WORKING__CHECK_EOT,
} EN_RECEIVE_STATE;



extern void setLogLevel (EN_LOG_LEVEL enLvl);
extern EN_LOG_LEVEL getLogLevel (void);

/**
 * log macro
 */
#ifndef _ANDROID_BUILD

// --- Information ---
#ifndef _LOG_ADD_FILE_INFO
#define _IMMSOCK_LOG_I(fmt, ...) {\
	CUtils::putsLogLW (stdout, getLogLevel(), EN_LOG_LEVEL_I, fmt, ##__VA_ARGS__);\
}
#else
#define _IMMSOCK_LOG_I(fmt, ...) {\
	CUtils::putsLog (stdout, getLogLevel(), EN_LOG_LEVEL_I, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);\
}
#endif

// --- Notice ---
#ifndef _LOG_ADD_FILE_INFO
#define _IMMSOCK_LOG_N(fmt, ...) {\
	CUtils::putsLogLW (stdout, getLogLevel(), EN_LOG_LEVEL_N, fmt, ##__VA_ARGS__);\
}
#else
#define _IMMSOCK_LOG_N(fmt, ...) {\
    CUtils::putsLog (stdout, getLogLevel(), EN_LOG_LEVEL_N, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);\
}
#endif

// --- Warning ---
#ifndef _LOG_ADD_FILE_INFO
#define _IMMSOCK_LOG_W(fmt, ...) {\
	CUtils::putsLogLW (stdout, getLogLevel(), EN_LOG_LEVEL_W, fmt, ##__VA_ARGS__);\
}
#else
#define _IMMSOCK_LOG_W(fmt, ...) {\
    CUtils::putsLog (stdout, getLogLevel(), EN_LOG_LEVEL_W, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);\
}
#endif

// --- Error ---
#ifndef _LOG_ADD_FILE_INFO
#define _IMMSOCK_LOG_E(fmt, ...) {\
	CUtils::putsLogLW (stdout, getLogLevel(), EN_LOG_LEVEL_E, fmt, ##__VA_ARGS__);\
}
#else
#define _IMMSOCK_LOG_E(fmt, ...) {\
    CUtils::putsLog (stdout, getLogLevel(), EN_LOG_LEVEL_E, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);\
}
#endif

// --- perror ---
#ifndef _LOG_ADD_FILE_INFO
#define _IMMSOCK_PERROR(fmt, ...) {\
	CUtils::putsLogLW (stdout, getLogLevel(), EN_LOG_LEVEL_PE, fmt, ##__VA_ARGS__);\
}
#else
#define _IMMSOCK_PERROR(fmt, ...) {\
	CUtils::putsLog (stdout, getLogLevel(), EN_LOG_LEVEL_PE, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__);\
}
#endif

#else // _ANDROID_BUILD

// --- Information ---
#define _IMMSOCK_LOG_I(fmt, ...) do {\
	__android_log_print (ANDROID_LOG_DEBUG, __func__, fmt, ##__VA_ARGS__); \
} while (0)

// --- Notice ---
#define _IMMSOCK_LOG_N(fmt, ...) do {\
	__android_log_print (ANDROID_LOG_INFO, __func__, fmt, ##__VA_ARGS__); \
} while (0)

// --- Warning ---
#define _IMMSOCK_LOG_W(fmt, ...) do {\
	__android_log_print (ANDROID_LOG_WARN, __func__, fmt, ##__VA_ARGS__); \
} while (0)

// --- Error ---
#define _IMMSOCK_LOG_E(fmt, ...) do {\
	__android_log_print (ANDROID_LOG_ERROR, __func__, fmt, ##__VA_ARGS__); \
} while (0)

// --- perror ---
#define _IMMSOCK_PERROR(fmt, ...) do {\
	__android_log_print (ANDROID_LOG_ERROR, __func__, fmt, ##__VA_ARGS__); \
	char szPerror[32]; \
	strerror_r(errno, szPerror, sizeof (szPerror)); \
	__android_log_print (ANDROID_LOG_ERROR, __func__, "%s", szPerror); \
} while (0)

#endif

} // namespace ImmSocket

#endif
