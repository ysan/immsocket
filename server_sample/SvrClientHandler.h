#ifndef _SVR_CLIENT_HANDLER_H_
#define _SVR_CLIENT_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "PacketHandler.h"

using namespace std;
using namespace ImmSocket;
using namespace ImmSocketService;


class CSvrClientHandler : public CImmSocketServer::IClientHandler
{
public:
	CSvrClientHandler (void);
	virtual ~CSvrClientHandler (void);

private:
	CImmSocketClient* onAcceptClient (int fdClientSocket);

};

#endif
