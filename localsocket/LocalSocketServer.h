#ifndef _LOCAL_SOCKET_SERVER_H_
#define _LOCAL_SOCKET_SERVER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <map>

#include "WorkerThread.h"
#include "Utils.h"
#include "LocalSocketCommon.h"
#include "LocalSocketClient.h"

using namespace std;


namespace LocalSocket {

class CLocalSocketClient;

typedef struct client_info {
	int fd;
	CLocalSocketClient *pInstance;
} ST_CLIENT_INFO;

typedef map<int, ST_CLIENT_INFO> CLIENT_TABLE; // <fd, ST_CLIENT_INFO>


class CLocalSocketServer;
typedef int (CLocalSocketServer:: *P_CB_SETUP_SERVER_SOCKET) (void);
typedef int (CLocalSocketServer:: *P_CB_ACCEPT_WRAPPER) (int fdServerSocket);


class CLocalSocketServer : public CWorkerThread
{
public:
	class IClientHandler
	{
	public:
		virtual ~IClientHandler (void) {};

		virtual CLocalSocketClient *onAcceptClient (int fdClientSocket) = 0;
	};


public:
	CLocalSocketServer (void);                                                            // local
	CLocalSocketServer (const char *pPath);                                               // local
	CLocalSocketServer (const char *pPath, CLocalSocketServer::IClientHandler *pHandler); // local
	CLocalSocketServer (uint16_t port);                                                   // tcp
	CLocalSocketServer (uint16_t port, CLocalSocketServer::IClientHandler *pHandler);     // tcp
	virtual ~CLocalSocketServer (void);


//	CLocalSocketServer *getInstance (void);
	bool start (void); // async
	void stop (void); // async
	void syncStop (void);

	void sendToClient (uint8_t *pData, int size);

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void acceptLoop (int fdServerSocket);
	void addClientTable (int fd, CLocalSocketClient *pInstance);
	bool removeClientTable (int fd);
	void refreshClientTable (void);
	void forceClearClientTable (void);
	void dumpClientTable (void);

	int setupServerSocket     (void);
	int setupServerSocket_Tcp (void);
	int acceptWrapper     (int fdServerSocket);
	int acceptWrapper_Tcp (int fdServerSocket);


	int mFdServerSocket;
	CLocalSocketServer::IClientHandler *mpClientHandler;
	bool mIsStop;

	pthread_mutex_t mMutex;

	CLIENT_TABLE mClientTable;
	pthread_mutex_t mMutexClientTable;

	char mSocketEndpointPath[64]; // local
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	P_CB_SETUP_SERVER_SOCKET mpcbSetupServerSocket;
	P_CB_ACCEPT_WRAPPER mpcbAcceptWrapper;

};

} // namespace LocalSocket

#endif
