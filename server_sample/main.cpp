#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "WorkerThread.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "Message.h"
#include "SvrMessageHandler.h"
#include "SvrClientHandler.h"

 
using namespace std;
using namespace ImmSocket;
using namespace ImmSocketService;


int main (void)
{ 
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


	CSvrClientHandler *pClientHandler = new CSvrClientHandler ();

#ifndef _DEBUG_TCP
	CImmSocketServer server ((char*)"/tmp/imm_socket_sample", pClientHandler);
#else
	CImmSocketServer server (65000, pClientHandler);
//	CImmSocketServer server (65000, new CSvrMessageHandler()); // debug single client
#endif
	server.start();


	sleep (1);
	if (server.isAlive()) {
		while (1) {
			char buf[1024] = {0};
			memset (buf, 0x00, sizeof(buf));
			fgets (buf, sizeof(buf)-1, stdin);
			CUtils::deleteLF (buf);

			if ((strlen(buf) == 1) && (strncmp(buf, "q", strlen(buf)) == 0)) {
				// quit
				break;
			}
		}
	}

	server.syncStop();

	if (pClientHandler) {
		delete pClientHandler;
		pClientHandler = NULL;
	}

	exit (EXIT_SUCCESS);
}
