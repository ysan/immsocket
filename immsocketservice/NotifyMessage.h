#ifndef _NOTIFY_MESSAGE_H_
#define _NOTIFY_MESSAGE_H_


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
#include "Message.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {


class CNotifyMessage : public CMessage
{
public:
	CNotifyMessage (CMessage *pMsg);
	CNotifyMessage (CImmSocketClient *pClient);
	virtual ~CNotifyMessage (void);

	bool send (uint8_t command);
	bool send (uint8_t command, uint8_t *pData, int size);
};

} // namespace ImmSocketService

#endif
