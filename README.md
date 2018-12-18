immsocket
===============

socket library which can be used immediately (sock_stream) (unix_domain/inet_domain)
a kind of short packet message exchanger.


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


//
// You inherit CPacketHandler and implement the following three methods.
// - onHandleRequest
// - onHandleReply
// - onHandleNotify
//
//  Attention:
//    If there are multiple thread pools, the above functions are called in parallel.
//    Therefore, reentrant code is mandatory.
//    Also, putting mutex makes sense of thread pool meaningless.
//
class CSvrMessageHandler : public CPacketHandler
{
public:
	explicit CSvrMessageHandler (int threadPoolNum) : CPacketHandler (threadPoolNum) {}
	virtual ~CSvrMessageHandler (void) {}

private:
	void onHandleRequest (CRequestMessage *pRequestMsg) override {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
		switch ((int)pRequestMsg->getCommand()) {
		case 0x01: {
			char *p = (char*)pRequestMsg->getData();
			_UTL_LOG_I ("received ->  [%s]\n", p);

			CReplyMessage *pReplyMsg = new CReplyMessage (pRequestMsg);
			pReplyMsg->sendOK ((uint8_t*)p, (int)strlen(p)); // echo reply
			delete pReplyMsg;
			pReplyMsg = NULL;
			} break;
		default: {
			CReplyMessage *pReplyMsg = new CReplyMessage (pRequestMsg);
			pReplyMsg->sendNG();
			delete pReplyMsg;
			pReplyMsg = NULL;
			} break;
		}
	}

	void onHandleReply (CReplyMessage *pReplyMsg) override {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}

	void onHandleNotify (CNotifyMessage *pNotifyMsg) override {
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


	// Specify a class that inherits from CPacketHandler in template.
	// Specify message handle thread pool num = 2
	// CSvrMessageHandler instance is created for each client connection.
	// thread pool is created for each connection.
	CClientHandler<CSvrMessageHandler> *pClientHandler = new CClientHandler<CSvrMessageHandler> (2);

	CServer server (65000, pClientHandler); // specified tcp port 65000
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

//
// You inherit CPacketHandler and implement the following three methods.
// - onHandleRequest
// - onHandleReply
// - onHandleNotify
//
//  Attention:
//    If there are multiple thread pools, the above functions are called in parallel.
//    Therefore, reentrant code is mandatory.
//    Also, putting mutex makes sense of thread pool meaningless.
//
class CClMessageHandler : public CPacketHandler
{
public:
	explicit CClMessageHandler (int threadPoolNum) : CPacketHandler (threadPoolNum) {}
	virtual ~CClMessageHandler (void) {}

private:
	void onHandleRequest (CRequestMessage *pRequestMsg) override {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
	}

	void onHandleReply (CReplyMessage *pReplyMsg) override {
		_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
		CMessageId::CId id = *pReplyMsg->getId();
		if (id == g_id) { // id match
			_UTL_LOG_I ("%s (async)\n", pReplyMsg->isReplyResultOK() ? "REPLY_OK" : "REPLY_NG")
;
			_UTL_LOG_I ("replyData [%s]\n", (char*)(pReplyMsg->getData()));
		}
	}

	void onHandleNotify (CNotifyMessage *pNotifyMsg) override {
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


	// specify message handle thread pool num = 2
	CClMessageHandler *pHandler = new CClMessageHandler(2);

	CClient client ((const char*)"127.0.0.1", 65000, pHandler); // specified tcp port 65000

	// connect
	bool r = client.connectToServer();
	if (!r) {
		exit (EXIT_FAILURE);
	}
	client.startReceiver();


	uint8_t command = 0x01;
	char *p_data = (char*)"test";

	// async request  -> reply is handled by CClMessageHandler.
	CRequestMessage *pRequestMsg = new CRequestMessage (&client);
	g_id = pRequestMsg->generateId(); // for async reply id match
	pRequestMsg->sendAsync (&g_id, command, (uint8_t*)p_data, (int)strlen(p_data));
	delete pRequestMsg;
	pRequestMsg = NULL;

	// sync request
	CRequestMessage *pRequestMsgSync = new CRequestMessage (&client);
	pRequestMsgSync->sendSync (command, (uint8_t*)p_data, (int)strlen(p_data)); // request and wait rep
ly
	if (pRequestMsgSync->isReplyResultOK()) {
		_UTL_LOG_I ("REPLY_OK (sync)\n");
	} else {
		_UTL_LOG_I ("REPLY_NG (sync)\n");
	}
	_UTL_LOG_I ("replyData [%s]\n", (char*)(pRequestMsgSync->getData()));
	delete pRequestMsgSync;
	pRequestMsgSync = NULL;


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

