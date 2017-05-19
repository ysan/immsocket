#ifndef _LOCAL_SOCKET_CLIENT_H_
#define _LOCAL_SOCKET_CLIENT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include <vector>
#include <map>

#include "Utils.h"
#include "WorkerThread.h"
#include "LocalSocketCommon.h"

using namespace std;


namespace LocalSocket {

class CLocalSocketClient;
typedef int (CLocalSocketClient:: *P_CB_SETUP_CLIENT_SOCKET) (void);
typedef int (CLocalSocketClient:: *P_CB_RECEIVE_WRAPPER) (int fdClientSocket, uint8_t *pBuff, int size);
typedef int (CLocalSocketClient:: *P_CB_SEND_WRAPPER) (int fdClientSocket, const uint8_t *pData, int size);

class CLocalSocketClient : public CWorkerThread
{
public:
	class IPacketHandler
	{
	public:
		virtual ~IPacketHandler (void) {};

		virtual void onSetup (CLocalSocketClient *pSelf) = 0;
		virtual void onTeardown (CLocalSocketClient *pSelf) = 0;
		virtual void onReceivePacket (CLocalSocketClient *pSelf, uint8_t *pPacket, int size) = 0;
	};


public:
	// for client single
	CLocalSocketClient (void);                                                                               // local
	CLocalSocketClient (const char *pszPath);                                                                // local
	CLocalSocketClient (const char *pszPath, CLocalSocketClient::IPacketHandler *pHandler);                  // local
	CLocalSocketClient (const char *pszIpAddr, uint16_t port);                                               // tcp
	CLocalSocketClient (const char *pszIpAddr, uint16_t port, CLocalSocketClient::IPacketHandler *pHandler); // tcp
	// for sever side
	CLocalSocketClient (int fdClientSocket);
	CLocalSocketClient (int fdClientSocket, CLocalSocketClient::IPacketHandler *pHandler);

	virtual ~CLocalSocketClient (void);


	bool startReceiver (void); // async
	void stopReceiver (void); // async
	void syncStopReceiver (void);

	bool connectToServer (void);
	void disconnectFromServer (void);
	bool isConnected (void);

	int getFd (void);
	bool sendToConnection (const uint8_t *pData, int size);
	int receiveOnePacket (uint8_t *pBuff, int size);
	CLocalSocketClient::IPacketHandler* getPacketHandler (void);

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void receiveLoop (int fdClientSocket);
	void receiveOnce (int fdClientSocket);
	int checkData (uint8_t *pBuff, int size, bool isOnce=false);
	bool setPacket (const uint8_t *pIn, int insize, uint8_t *pOut, int outsize);

	int setupClientSocket     (void);
	int setupClientSocket_Tcp (void);
	int receiveWrapper     (int fdClientSocket, uint8_t *pBuff, int size);
	int receiveWrapper_Tcp (int fdClientSocket, uint8_t *pBuff, int size);
	int sendWrapper     (int fdClientSocket, const uint8_t *pData, int size);
	int sendWrapper_Tcp (int fdClientSocket, const uint8_t *pData, int size);


	bool mIsStop;
	EN_RECEIVE_STATE mState;
	int mFdClientSocket;
	uint8_t mCurrentPacket [RECEIVED_SIZE_MAX];
	int mCurrentPacketWritePos;
	uint8_t mCurrentPacketDataSize;
	
	CLocalSocketClient::IPacketHandler *mpPacketHandler;

	pthread_mutex_t mMutex;
	pthread_mutex_t mMutexSend;

	char mSocketEndpointPath [64]; // local
	char mIpAddr [15+1];          // tcp
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	P_CB_SETUP_CLIENT_SOCKET mpcbSetupClientSocket;
	P_CB_RECEIVE_WRAPPER mpcbReceiveWrapper;
	P_CB_SEND_WRAPPER mpcbSendWrapper;

};

} // namespace LocalSocket

#endif
