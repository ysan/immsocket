#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "LocalSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "Message.h"
#include "ClMessageHandler.h"

 
using namespace std;
using namespace LocalSocket;
using namespace LocalSocketService;


static void *sync_req_test (void *args);

int main (void)
{
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


//	CPacketHandler *pHandler = new CPacketHandler();
	CClMessageHandler *pHandler = new CClMessageHandler();

#ifndef _DEBUG_TCP
	CLocalSocketClient client ((const char*)"/tmp/local_socket_sample", pHandler);
#else
//	CLocalSocketClient client ((const char*)"43.3.177.96", 65000, pHandler);
	CLocalSocketClient client ((const char*)"127.0.0.1", 65000, pHandler);
#endif
	if (!client.connectToServer()) {
		exit (EXIT_FAILURE);
	}

	client.startReceiver();


	pthread_t thid;
	if (pthread_create (&thid, NULL, sync_req_test, (void*)&client) != 0) {
		_UTL_PERROR ("pthread_create");
	}



	char buf[1024] = {0};
	int n = 0;
	uint8_t id = 0x00;
	while (1) {
		memset (buf, 0x00, sizeof(buf));
		fgets (buf, sizeof(buf)-1, stdin);
		CUtils::deleteLF (buf);

		if ((strlen(buf) == 1) && (strncmp(buf, "q", strlen(buf)) == 0)) {
			// quit
			break;
		}


		_UTL_LOG_I ("async request\n");

		CMessage *pMsg = new CMessage(&client);
		if ((int)strlen(buf) > 0) {
			id = pMsg->generateId();
			if (!pMsg->sendRequestAsync(id, (uint8_t)0x01, (uint8_t*)buf, (int)strlen(buf))) {
				delete pMsg;
				pMsg = NULL;
				continue;
			}

		} else {
			n ++;
			id = pMsg->generateId();
			if (!pMsg->sendRequestAsync(id, (uint8_t)0x05, (uint8_t*)&n, sizeof(int))) {
				delete pMsg;
				pMsg = NULL;
				continue;
			}
		}

		delete pMsg;
		pMsg = NULL;

	}


	// disconnect
	client.syncStopReceiver();
	client.disconnectFromServer();

	if (pHandler) {
		delete pHandler;
		pHandler = NULL;
	}

	exit (EXIT_SUCCESS);
}

static void *sync_req_test (void *args)
{
	CLocalSocketClient* pcl = (CLocalSocketClient*)args;

	while (1) {
		sleep (5);

		_UTL_LOG_I ("sync request\n");
		CMessage *pMsg = new CMessage(pcl);
		if (!pMsg->sendRequestSync ((uint8_t)0x02)) { // reply wait
			delete pMsg;
			pMsg = NULL;
			continue;
		}

		if (pMsg->sync()->isReplyResultOK()) {
			_UTL_LOG_I ("REPLY_OK\n");
		} else {
			_UTL_LOG_I ("REPLY_NG\n");
		}

		if (pMsg->sync()->getDataSize() > 0) {
			_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsg->sync()->getData()));
		}

		delete pMsg;
		pMsg = NULL;
	}

	return NULL;
}

