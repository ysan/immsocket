#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "ImmSocketService.h"

using namespace std;
using namespace ImmSocketService;


class CSvrMessageHandler : public CPacketHandler
{
public:
	explicit CSvrMessageHandler (int threadPoolNum) : CPacketHandler (threadPoolNum) {}
	virtual ~CSvrMessageHandler (void) {}

private:
	// override
	void onHandleRequest (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
		switch ((int)pMsg->getCommand()) {
		case 0x01: {
			char *p = (char*)pMsg->getData();
			_UTL_LOG_I ("received ->  [%s]\n", p);

			CMessage *pReplyMsg = new CMessage(pMsg);
			pReplyMsg->sendReplyOK ((uint8_t*)p, (int)strlen(p)); // echo reply
			delete pReplyMsg;
			pReplyMsg = NULL;
			} break;
		default: {
			CMessage *pReplyMsg = new CMessage(pMsg);
			pReplyMsg->sendReplyNG();
			delete pReplyMsg;
			pReplyMsg = NULL;
			} break;
		}
    }

	// override
	void onHandleReply (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}

	// override
	void onHandleNotify (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}
};

class CSvrClientHandler : public CImmSocketServer::IClientHandler
{
public:
	CSvrClientHandler (void) {}
	virtual ~CSvrClientHandler (void) {}

private:
	// override
	CImmSocketClient* onAcceptClient (int fdClientSocket) {
		_UTL_LOG_N ("%s\n", __PRETTY_FUNCTION__);
		CSvrMessageHandler *pHandler = new CSvrMessageHandler(5); // packet handle thread pool num = 5
		CImmSocketClient *pClient = new CImmSocketClient (fdClientSocket, pHandler);
		pClient->setTcpSocket ();
		pClient->startReceiver ();
		return pClient;
	}

	// override
	void onRemoveClient (CImmSocketClient *pClient) {
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
};


int main (void)
{ 
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


	CSvrClientHandler *pClientHandler = new CSvrClientHandler ();

	CImmSocketServer server (65000, pClientHandler); // specified tcp port 65000
	server.start();


	fgetc (stdin);


	server.syncStop();


	if (pClientHandler) {
		delete pClientHandler;
		pClientHandler = NULL;
	}


	exit (EXIT_SUCCESS);
}
