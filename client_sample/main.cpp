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
using namespace LocalSocketService;


static void *threadHandler (void *args)
{
	CLocalSocketClient* pcl = (CLocalSocketClient*)args;

	while (1) {
		sleep (5);

		_UTL_LOG_N ("sync request\n");
		CMessage *pMsg = new CMessage(pcl);
		if (!pMsg->sendRequestSync ((uint8_t)0x02)) { // reply wait
			continue;
		}

		switch ((int)pMsg->sync()->getCommand()) {
		case REPLY_OK:
			_UTL_LOG_N ("REPLY_OK\n");
			break;

		case REPLY_NG:
			_UTL_LOG_N ("REPLY_NG\n");
			break;

		default:
			_UTL_LOG_E ("invalid reply message\n");
			break;
		}

		if (pMsg->sync()->getDataSize() > 0) {
			_UTL_LOG_N ("replyData [%s]\n", (char*)(pMsg->sync()->getData()));
		}

		delete pMsg;
		pMsg = NULL;
	}

	return NULL;
}


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


	pthread_t pth_id;
	if (pthread_create (&pth_id, NULL, threadHandler, (void*)&client) != 0) {
		_UTL_PERROR ("pthread_create");
	}



	char buf[1024] = {0};
	int n = 0;
	uint8_t id = 0x00;
	while (1) {
		memset (buf, 0x00, sizeof(buf));
		fgets (buf, sizeof(buf)-1, stdin);
		CUtils::deleteLF (buf);

		if ((strlen(buf) == 3) && (strncmp(buf, "end", strlen(buf)) == 0)) {
			break;
		}


		_UTL_LOG_N ("async request\n");

		CMessage *pMsg = new CMessage(&client);
		if ((int)strlen(buf) > 0) {
			id = pMsg->genId();
			if (!pMsg->sendRequestAsync(id, (uint8_t)0x01, (uint8_t*)buf, (int)strlen(buf))) {
				continue;
			}

		} else {
			n ++;
			id = pMsg->genId();
			if (!pMsg->sendRequestAsync(id, (uint8_t)0x05, (uint8_t*)&n, sizeof(int))) {
				continue;
			}
		}

		delete pMsg;
		pMsg = NULL;

	}


	// disconnect
	client.syncStopReceiver();
	client.disconnectFromServer();


	delete pHandler;
	pHandler = NULL;

	exit (EXIT_SUCCESS);
}
