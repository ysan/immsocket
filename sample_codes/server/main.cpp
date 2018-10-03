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
	// specified message handle thread pool num = 2
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
