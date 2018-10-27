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
			_UTL_LOG_I ("%s (async)\n", pReplyMsg->isReplyResultOK() ? "REPLY_OK" : "REPLY_NG");
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
	pRequestMsgSync->sendSync (command, (uint8_t*)p_data, (int)strlen(p_data)); // request and wait reply
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
