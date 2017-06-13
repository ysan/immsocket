#ifndef _IMM_SOCKET_SERVER_H_
#define _IMM_SOCKET_SERVER_H_


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
#include "ImmSocketCommon.h"
#include "ImmSocketClient.h"

using namespace std;


namespace ImmSocket {

class CImmSocketClient;

typedef struct client_info {
	int fd;
	CImmSocketClient *pInstance;
} ST_CLIENT_INFO;

typedef map<int, ST_CLIENT_INFO> CLIENT_TABLE; // <fd, ST_CLIENT_INFO>

typedef struct client_ref {
	CLIENT_TABLE *pTable;
	pthread_mutex_t *pMutex;
} ST_CLIENT_REF;

class CImmSocketServer;
typedef int (CImmSocketServer:: *P_CB_SETUP_SERVER_SOCKET) (void);
typedef int (CImmSocketServer:: *P_CB_ACCEPT_WRAPPER) (int fdServerSocket);


class CImmSocketServer : public CWorkerThread
{
public:
	class IClientHandler
	{
	public:
		virtual ~IClientHandler (void) {};

		virtual CImmSocketClient *onAcceptClient (int fdClientSocket) = 0;
	};


public:
	CImmSocketServer (void);                                                          // local
	CImmSocketServer (const char *pPath);                                             // local
	CImmSocketServer (const char *pPath, CImmSocketServer::IClientHandler *pHandler); // local
	CImmSocketServer (const char *pPath, CImmSocketClient::IPacketHandler *pHandler); // local // single client
	CImmSocketServer (uint16_t port);                                                 // tcp
	CImmSocketServer (uint16_t port, CImmSocketServer::IClientHandler *pHandler);     // tcp
	CImmSocketServer (uint16_t port, CImmSocketClient::IPacketHandler *pHandler);     // tcp // single client
	virtual ~CImmSocketServer (void);


//	CImmSocketServer *getInstance (void);
	bool start (void); // async
	void stop (void); // async
	void syncStop (void);
	bool isStarted (void);

	void sendToClient (uint8_t *pData, int size);

	ST_CLIENT_REF getClientRef (void) {
		ST_CLIENT_REF ref = {&mClientTable, &mMutexClientTable};
		return ref;
	};

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void acceptLoop (int fdServerSocket);
	void addClientTable (int fd, CImmSocketClient *pInstance);
	bool removeClientTable (int fd);
	void refreshClientTable (void);
	void forceClearClientTable (void);
	void dumpClientTable (void);

	int setupServerSocket     (void);
	int setupServerSocket_Tcp (void);
	int acceptWrapper     (int fdServerSocket);
	int acceptWrapper_Tcp (int fdServerSocket);


	int mFdServerSocket;
	CImmSocketServer::IClientHandler *mpClientHandler;
	CImmSocketClient::IPacketHandler *mpPacketHandler;
	bool mIsStop;

	pthread_mutex_t mMutex;

	CLIENT_TABLE mClientTable;
	pthread_mutex_t mMutexClientTable;

	char mSocketEndpointPath[64]; // local
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	P_CB_SETUP_SERVER_SOCKET mpcbSetupServerSocket;
	P_CB_ACCEPT_WRAPPER mpcbAcceptWrapper;
	bool isConfigLocal;

};

} // namespace ImmSocket

#endif
