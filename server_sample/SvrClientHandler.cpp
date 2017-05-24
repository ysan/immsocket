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
#include "SvrClientHandler.h"
#include "SvrMessageHandler.h"


CSvrClientHandler::CSvrClientHandler (void)
{
}

CSvrClientHandler::~CSvrClientHandler (void)
{
}

CImmSocketClient *CSvrClientHandler::onAcceptClient (int fdClientSocket)
{
	_UTL_LOG_N ("%s\n", __PRETTY_FUNCTION__);

	CSvrMessageHandler *pHandler = new CSvrMessageHandler();

	CImmSocketClient *pClient = new CImmSocketClient (fdClientSocket, pHandler);
#ifdef _DEBUG_TCP
	pClient->setTcpSocket ();
#endif
	pClient->startReceiver ();


	return pClient;
}
