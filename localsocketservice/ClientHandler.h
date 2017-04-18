#ifndef _CLIENT_HANDLER_H_
#define _CLIENT_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "LocalSocketServer.h"
#include "LocalSocketClient.h"
#include "PacketHandler.h"

using namespace std;

namespace LocalSocketService {

class CClientHandler : public CLocalSocketServer::IClientHandler
{
public:
	CClientHandler (void);
	virtual ~CClientHandler (void);

protected:
	CLocalSocketClient* onAcceptClient (int fdClientSocket);

};

} // namespace LocalSocketService

#endif
