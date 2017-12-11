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
	bool isEchoMode;
} ST_CLIENT_INFO;

typedef map<int, ST_CLIENT_INFO> CLIENT_TABLE; // <fd, ST_CLIENT_INFO>

typedef struct client_ref {
	CLIENT_TABLE *pTable;
	pthread_mutex_t *pMutex;
} ST_CLIENT_REF;

class CImmSocketServer;
typedef int (CImmSocketServer:: *P_FN_SETUP_SERVER_SOCKET) (void);
typedef int (CImmSocketServer:: *P_FN_ACCEPT_WRAPPER) (int fdServerSocket);


class CImmSocketServer : public CWorkerThread
{
public:
#if 0
	class IClientHandler
	{
	public:
		virtual ~IClientHandler (void) {};

		virtual CImmSocketClient *onAcceptClient (int fdClientSocket) = 0;
		virtual void onRemoveClient (CImmSocketClient *pClient) = 0;
	};
#endif

public:
//	CImmSocketServer (void); // local
	CImmSocketServer (const char *pPath); // local echo server
	CImmSocketServer (const char *pPath, CImmSocketClient::IPacketHandler *pHandler, bool isMultiClient=true); // local
	CImmSocketServer (uint16_t port); // tcp echo server
	CImmSocketServer (uint16_t port, CImmSocketClient::IPacketHandler *pHandler, bool isMultiClient=true); // tcp
	virtual ~CImmSocketServer (void);


	bool start (void); // async
	void stop (void); // async
	void syncStop (void);
	bool isStarted (void);

	void sendToClient (uint8_t *pData, int size);

	const ST_CLIENT_REF getClientRef (void) {
		ST_CLIENT_REF ref = {&mClientTable, &mMutexClientTable};
		return ref;
	};

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void acceptLoop (int fdServerSocket);
	void addClientTable (int fd, CImmSocketClient *pInstance, bool isEchoMode=false);
	bool removeClientTable (int fd);
	void refreshClientTable (void);
	void forceClearClientTable (void);
	void dumpClientTable (void);

	int setupServerSocket     (void);
	int setupServerSocket_Tcp (void);
	int acceptWrapper     (int fdServerSocket);
	int acceptWrapper_Tcp (int fdServerSocket);


	int mFdServerSocket;
	CImmSocketClient::IPacketHandler *mpPacketHandler;
	bool mIsStop;

	pthread_mutex_t mMutex;

	CLIENT_TABLE mClientTable;
	pthread_mutex_t mMutexClientTable;

	bool mIsMultiClient;

	char mSocketEndpointPath[64]; // local
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	P_FN_SETUP_SERVER_SOCKET mpfnSetupServerSocket;
	P_FN_ACCEPT_WRAPPER mpfnAcceptWrapper;
	bool mIsConfigLocal;

};

} // namespace ImmSocket

#endif
