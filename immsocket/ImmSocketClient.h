#ifndef _IMM_SOCKET_CLIENT_H_
#define _IMM_SOCKET_CLIENT_H_


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
#include "ImmSocketCommon.h"

using namespace std;


namespace ImmSocket {

class CImmSocketClient;
typedef int (CImmSocketClient:: *P_FN_SETUP_CLIENT_SOCKET) (void);
typedef int (CImmSocketClient:: *P_FN_RECEIVE_WRAPPER) (int fdClientSocket, uint8_t *pBuff, int size);
typedef int (CImmSocketClient:: *P_FN_SEND_WRAPPER) (int fdClientSocket, const uint8_t *pData, int size);

class CImmSocketClient : public CWorkerThread
{
public:
	class IPacketHandler
	{
	public:
		virtual ~IPacketHandler (void) {};

		virtual void onSetup (CImmSocketClient *pSelf) = 0;
		virtual void onTeardown (CImmSocketClient *pSelf) = 0;
		virtual void onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size) = 0;
	};


public:
	// for client single
	CImmSocketClient (void);                                                                             // local
	CImmSocketClient (const char *pszPath);                                                              // local
	CImmSocketClient (const char *pszPath, CImmSocketClient::IPacketHandler *pHandler);                  // local
	CImmSocketClient (const char *pszIpAddr, uint16_t port);                                             // tcp
	CImmSocketClient (const char *pszIpAddr, uint16_t port, CImmSocketClient::IPacketHandler *pHandler); // tcp
	// for sever side
	CImmSocketClient (int fdClientSocket);
	CImmSocketClient (int fdClientSocket, CImmSocketClient::IPacketHandler *pHandler);

	virtual ~CImmSocketClient (void);


	bool startReceiver (void); // async
	void stopReceiver (void); // async
	void syncStopReceiver (void);

	bool connectToServer (void);
	void disconnectFromServer (void);
	bool isConnected (void);

	int getFd (void);
	bool sendToConnection (const uint8_t *pData, int size);

	void syncReceivePacketLoop (void);
	int syncReceivePacketOnce (uint8_t *pBuff, int size, uint32_t nTimeoutMsec=0);

	CImmSocketClient::IPacketHandler* getPacketHandler (void);

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void receiveLoop (int fdClientSocket);
	int receiveOnce (int fdClientSocket, uint32_t nTimeoutMsec=0);
	void clearReceiveDependVals (void);
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
	
	CImmSocketClient::IPacketHandler *mpPacketHandler;

	pthread_mutex_t mMutex;
	pthread_mutex_t mMutexSend;

	char mSocketEndpointPath [64]; // imm
	char mIpAddr [15+1];          // tcp
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	P_FN_SETUP_CLIENT_SOCKET mpfnSetupClientSocket;
	P_FN_RECEIVE_WRAPPER mpfnReceiveWrapper;
	P_FN_SEND_WRAPPER mpfnSendWrapper;

};

} // namespace ImmSocket

#endif
