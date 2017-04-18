#ifndef _SVR_MESSAGE_HANDLER_H_
#define _SVR_MESSAGE_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "Message.h"
#include "PacketHandler.h"

using namespace std;
using namespace LocalSocketService;


class CSvrMessageHandler : public CPacketHandler
{
public:
	CSvrMessageHandler (void);
	virtual ~CSvrMessageHandler (void);

private:
	void onHandleRequest (CMessage *pMsg);
	void onHandleReply (CMessage *pMsg);
	void onHandleNotify (CMessage *pMsg);


};

#endif
