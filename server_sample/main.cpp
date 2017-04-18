#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "WorkerThread.h"
#include "LocalSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "Message.h"
#include "SvrMessageHandler.h"
#include "SvrClientHandler.h"

 
using namespace std;
using namespace LocalSocketService;


int main (void)
{ 
//TODO
	sigset_t sigset;
	sigemptyset (&sigset);
	sigaddset (&sigset, SIGPIPE);
	sigprocmask (SIG_BLOCK, &sigset, NULL);


	CSvrClientHandler *pClientHandler = new CSvrClientHandler ();

#ifndef _DEBUG_TCP
	CLocalSocketServer server ((char*)"/tmp/local_socket_sample", pClientHandler);
#else
	CLocalSocketServer server (65000, pClientHandler);
#endif
	server.start();


	sleep (1);
	if (server.isAlive()) {

		char b[1];
		read (STDIN_FILENO, b, 1);
		server.syncStop();

	}


	if (pClientHandler) {
		delete pClientHandler;
		pClientHandler = NULL;
	}

	exit (EXIT_SUCCESS);
}
