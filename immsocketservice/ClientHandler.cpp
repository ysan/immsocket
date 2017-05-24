#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "ClientHandler.h"
#include "PacketHandler.h"


namespace ImmSocketService {

CClientHandler::CClientHandler (void)
{
}

CClientHandler::~CClientHandler (void)
{
}

CImmSocketClient *CClientHandler::onAcceptClient (int fdClientSocket)
{
	CPacketHandler *pPacketHandler = new CPacketHandler();

	CImmSocketClient *pClient = new CImmSocketClient (fdClientSocket, pPacketHandler);
	pClient->startReceiver ();

	return pClient;
}

} // namespace ImmSocketService
