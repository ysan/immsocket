#ifndef _CL_MESSAGE_HANDLER_H_
#define _CL_MESSAGE_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "Message.h"
#include "PacketHandler.h"

using namespace std;
using namespace LocalSocket;
using namespace LocalSocketService;


class CClMessageHandler : public CPacketHandler
{
public:
	CClMessageHandler (void);
	virtual ~CClMessageHandler (void);

private:
	void onHandleRequest (CMessage *pMsg);
	void onHandleReply (CMessage *pMsg);
	void onHandleNotify (CMessage *pMsg);


};

#endif
