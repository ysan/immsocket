#ifndef _REPLY_MESSAGE_H_
#define _REPLY_MESSAGE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <arpa/inet.h>

#include <stdexcept>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Common.h"
#include "RequestMessage.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {


class CReplyMessage : public CMessage
{
public:
	CReplyMessage (CMessage *pMsg);
	CReplyMessage (CRequestMessage *pRequestMsg);
	virtual ~CReplyMessage (void);

	bool sendOK (void);
	bool sendOK (uint8_t *pReplyData, int size);
	bool sendNG (void);
	bool sendNG (uint8_t *pReplyData, int size);
};

} // namespace ImmSocketService

#endif
