immsocket
===============

socket library which can be used immediately (sock_stream) (unix_domain/inet_domain)


How to build
--------

Clone and make.

	$ git clone https://github.com/ysan/immsocket
	$ cd immsocket
	$ make

Then shared libraries is created.

	$ tree -P *.so
	.
	├── immsocket
	│   └── libimmsocket.so
	├── immsocketcommon
	│   └── libimmsocketcommon.so
	└── immsocketservice
	    └── libimmsocketservice.so


Example of use (sample codes)
--------

server code (echo server)

```c
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
```

client code

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "ImmSocketService.h"

 
using namespace std;
using namespace ImmSocketService;


CMessageId::CId g_id;

class CClMessageHandler : public CPacketHandler
{
public:
	explicit CClMessageHandler (int threadPoolNum) : CPacketHandler (threadPoolNum) {}
	virtual ~CClMessageHandler (void) {}

private:
	// override
	void onHandleRequest (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}

	// override
	void onHandleReply (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
		switch ((int)pMsg->getCommand()) {
		case 0x01: {
			CMessageId::CId id = *pMsg->getId();
			if (id == g_id) { // id match
				_UTL_LOG_I ("%s (async)\n", pMsg->isReplyResultOK() ? "REPLY_OK" : "REPLY_NG");
				_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsg->getData()));
			}

			} break;
		default:
			// unexpected reply
			break;
		}
	}

	// override
	void onHandleNotify (CMessage *pMsg) {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}
};

int main (void)
{
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


	CClMessageHandler *pHandler = new CClMessageHandler(2); // packet handle thread pool num = 2

	CImmSocketClient client ((const char*)"127.0.0.1", 65000, pHandler); // specified tcp port 65000

	// connect
	bool r = client.connectToServer();
	if (!r) {
		exit (EXIT_FAILURE);
	}
	client.startReceiver();


	char *p = (char*)"test";

	// async request
	CMessage *pMsg = new CMessage (&client);
	g_id = pMsg->generateId(); // for async reply id match
	pMsg->sendRequestAsync (&g_id, (uint8_t)0x01, (uint8_t*)p, (int)strlen(p));
	delete pMsg;
	pMsg = NULL;

	// sync request
	CMessage *pMsgSync = new CMessage (&client);
	pMsgSync->sendRequestSync ((uint8_t)0x01, (uint8_t*)p, (int)strlen(p));
	if (pMsgSync->isReplyResultOK()) {
		_UTL_LOG_I ("REPLY_OK (sync)\n");
	} else {
		_UTL_LOG_I ("REPLY_NG (sync)\n");
	}
	_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsgSync->getData()));
	delete pMsgSync;
	pMsgSync = NULL;


	fgetc (stdin);


	// disconnect
	client.syncStopReceiver();
	client.disconnectFromServer();

	if (pHandler) {
		delete pHandler;
		pHandler = NULL;
	}

	exit (EXIT_SUCCESS);
}
```

Component diagram
------------
![component diagram](https://github.com/ysan/immsocket/blob/master/etc/component_diagram.png)

Class diagram
------------
![class diagram](https://github.com/ysan/immsocket/blob/master/etc/class_diagram.png)

Packet format
------------
![packet format](https://github.com/ysan/immsocket/blob/master/etc/packet_format.png)


Platforms
------------
Generic Linux will be ok. (confirmed worked on Ubuntu, Fedora)

