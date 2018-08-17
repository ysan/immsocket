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
		CMessageId::CId id = *pMsg->getId();
		if (id == g_id) { // id match
			_UTL_LOG_I ("%s (async)\n", pMsg->isReplyResultOK() ? "REPLY_OK" : "REPLY_NG");
			_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsg->getData()));
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
