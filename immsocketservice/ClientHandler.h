#ifndef _CLIENT_HANDLER_H_
#define _CLIENT_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "PacketHandler.h"
#include "ImmSocketServiceCommon.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {

class CClientHandler : public CImmSocketServer::IClientHandler
{
public:
	CClientHandler (void);
	virtual ~CClientHandler (void);

protected:
	CImmSocketClient* onAcceptClient (int fdClientSocket);

};

} // namespace ImmSocketService

#endif
