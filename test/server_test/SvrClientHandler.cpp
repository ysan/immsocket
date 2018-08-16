#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketService.h"

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

	CSvrMessageHandler *pHandler = new CSvrMessageHandler(5); // packet handle thread pool num = 5
	CImmSocketClient *pClient = new CImmSocketClient (fdClientSocket, pHandler);

#ifdef _DEBUG_TCP
	pClient->setTcpSocket ();
#endif

	pClient->startReceiver ();


	return pClient;
}

void CSvrClientHandler::onRemoveClient (CImmSocketClient *pClient)
{
	_UTL_LOG_N ("%s\n", __PRETTY_FUNCTION__);

	if (!pClient) {
		return ;
	}

	pClient->syncStopReceiver ();

	CImmSocketClient::IPacketHandler *pHandler = pClient->getPacketHandler();
	if (pHandler) {
		_UTL_LOG_N ("client socket:[%d] --> packetHandler delete\n", pClient->getFd());
		delete pHandler;
		pHandler = NULL;
	}

	_UTL_LOG_N ("client socket:[%d] --> instance delete\n", pClient->getFd());
	delete pClient;
	pClient = NULL;
}