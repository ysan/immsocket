#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "ImmSocketService.h"

 
using namespace std;
using namespace ImmSocketService;


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
		case 0x01:
			_UTL_LOG_I ("%s\n", pMsg->isReplyResultOK() ? "reply_ok" : "reply_ng");
			if (pMsg->getDataSize() > 0) {
				_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsg->getData()));
			}
			break;
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


	CMessageId::CId id;
	char *p = (char*)"test";

	// async request
	CMessage *pMsg = new CMessage (&client);
	id = pMsg->generateId();
	pMsg->sendRequestAsync (&id, (uint8_t)0x01, (uint8_t*)p, (int)strlen(p));
	delete pMsg;
	pMsg = NULL;

	// sync request
	CMessage *pMsgSync = new CMessage (&client);
	id = pMsgSync->generateId();
	pMsgSync->sendRequestSync ((uint8_t)0x01, (uint8_t*)p, (int)strlen(p));
	if (pMsgSync->isReplyResultOK()) {
		_UTL_LOG_I ("REPLY_OK\n");
	} else {
		_UTL_LOG_I ("REPLY_NG\n");
	}
	if (pMsgSync->getDataSize() > 0) {
		_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsgSync->getData()));
	}
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
