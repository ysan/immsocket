#ifndef _SVR_CLIENT_HANDLER_H_
#define _SVR_CLIENT_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "LocalSocketServer.h"
#include "LocalSocketClient.h"
#include "PacketHandler.h"
#include "ClientHandler.h"

using namespace std;
using namespace LocalSocketService;


class CSvrClientHandler : public CClientHandler
{
public:
	CSvrClientHandler (void);
	virtual ~CSvrClientHandler (void);

private:
	CLocalSocketClient* onAcceptClient (int fdClientSocket);

};

#endif
