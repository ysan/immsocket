#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "ImmSocketService.h"

#include "ClMessageHandler.h"

 
using namespace std;
using namespace ImmSocketService;


static void *sync_req_test (void *args);

int main (void)
{
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


	CClMessageHandler *pHandler = new CClMessageHandler(2); // packet handle thread pool num = 2

#ifndef _DEBUG_TCP
	CClient client ((const char*)"/tmp/imm_socket_sample", pHandler);
#else
//	CClient client ((const char*)"43.3.177.96", 65000, pHandler);
	CClient client ((const char*)"127.0.0.1", 65000, pHandler);
#endif
	if (!client.connectToServer()) {
		exit (EXIT_FAILURE);
	}

	client.startReceiver();


	pthread_t thid;
	if (pthread_create (&thid, NULL, sync_req_test, (void*)&client) != 0) {
		_UTL_PERROR ("pthread_create");
	}



	char buf[65536] = {0};
	int n = 0;
	CMessageId::CId id;
	while (1) {
		memset (buf, 0x00, sizeof(buf));
		fgets (buf, sizeof(buf)-1, stdin);
		CUtils::deleteLF (buf);

		if ((strlen(buf) == 1) && (strncmp(buf, "q", strlen(buf)) == 0)) {
			// quit
			break;
		}


		_UTL_LOG_I ("async request\n");

		CRequestMessage *pRequestMsg = new CRequestMessage(&client);
		if ((int)strlen(buf) > 0) {
			id = pRequestMsg->generateId();
			if (!pRequestMsg->sendAsync(&id, (uint8_t)0x01, (uint8_t*)buf, (int)strlen(buf))) {
				delete pRequestMsg;
				pRequestMsg = NULL;
				continue;
			}

		} else {
			n ++;
			id = pRequestMsg->generateId();
			if (!pRequestMsg->sendAsync(&id, (uint8_t)0x05, (uint8_t*)&n, sizeof(int))) {
				delete pRequestMsg;
				pRequestMsg = NULL;
				continue;
			}
		}

		delete pRequestMsg;
		pRequestMsg = NULL;

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
	CImmSocketClient* pcl = (CImmSocketClient*)args;

	CRequestMessage *pRequestMsg = NULL;
	int rtn = 0;

	while (1) {
		sleep (5);

		_UTL_LOG_I ("sync request\n");
		pRequestMsg = new CRequestMessage(pcl);
		try {
			rtn = pRequestMsg->sendSync ((uint8_t)0x02);
			if (rtn == 0) {
				if (pRequestMsg->isReplyResultOK()) {
					_UTL_LOG_I ("REPLY_OK\n");
				} else {
					_UTL_LOG_I ("REPLY_NG\n");
				}

				if (pRequestMsg->getDataSize() > 0) {
					_UTL_LOG_I ("replyData [%s]\n", (char*)(pRequestMsg->getData()));
				}

				delete pRequestMsg;
				pRequestMsg = NULL;
			}

		} catch (CRequestMessage::CSyncException& e) {

			if (e.code() == CRequestMessage::CSyncException::INTERNAL_ERROR) {
			} else if (e.code() == CRequestMessage::CSyncException::TIMEOUT) {
			} else if (e.code() == CRequestMessage::CSyncException::SIGNAL_INTERRUPT) {
			} else {
				// UNEXPCTED_ERROR
			}

			_UTL_LOG_E (e.what());
			delete pRequestMsg;
			pRequestMsg = NULL;
			continue;
		}



		sleep (5);

		_UTL_LOG_I ("sync request 2\n");
		pRequestMsg = new CRequestMessage(pcl);
		try {
			rtn = pRequestMsg->sendSync ((uint8_t)0x02, 500);
			if (rtn == 0) {
				if (pRequestMsg->isReplyResultOK()) {
					_UTL_LOG_I ("REPLY_OK\n");
				} else {
					_UTL_LOG_I ("REPLY_NG\n");
				}

				if (pRequestMsg->getDataSize() > 0) {
					_UTL_LOG_I ("replyData [%s]\n", (char*)(pRequestMsg->getData()));
				}

				delete pRequestMsg;
				pRequestMsg = NULL;
			}

		} catch (CRequestMessage::CSyncException& e) {

			if (e.code() == CRequestMessage::CSyncException::INTERNAL_ERROR) {
			} else if (e.code() == CRequestMessage::CSyncException::TIMEOUT) {
			} else if (e.code() == CRequestMessage::CSyncException::SIGNAL_INTERRUPT) {
			} else {
				// UNEXPCTED_ERROR
			}

			_UTL_LOG_E (e.what());
			delete pRequestMsg;
			pRequestMsg = NULL;
			continue;
		}
	}

	return NULL;
}

