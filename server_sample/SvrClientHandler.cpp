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
#include "SvrClientHandler.h"
#include "SvrMessageHandler.h"


CSvrClientHandler::CSvrClientHandler (void)
{
}

CSvrClientHandler::~CSvrClientHandler (void)
{
}

CLocalSocketClient *CSvrClientHandler::onAcceptClient (int fdClientSocket)
{
	CSvrMessageHandler *pHandler = new CSvrMessageHandler();

	CLocalSocketClient *pClient = new CLocalSocketClient (fdClientSocket, pHandler);
#ifdef _DEBUG_TCP
	pClient->setTcpSocket ();
#endif
	pClient->startReceiver ();


	return pClient;
}
