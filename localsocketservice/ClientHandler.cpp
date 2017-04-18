#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "LocalSocketServer.h"
#include "LocalSocketClient.h"
#include "Utils.h"
#include "ClientHandler.h"
#include "PacketHandler.h"


namespace LocalSocketService {

CClientHandler::CClientHandler (void)
{
}

CClientHandler::~CClientHandler (void)
{
}

CLocalSocketClient *CClientHandler::onAcceptClient (int fdClientSocket)
{
	CPacketHandler *pPacketHandler = new CPacketHandler();

	CLocalSocketClient *pClient = new CLocalSocketClient (fdClientSocket, pPacketHandler);
	pClient->startReceiver ();

	return pClient;
}

} // namespace LocalSocketService
