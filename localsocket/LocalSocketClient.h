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

using namespace std;


#define DEFAULT_LOCALSOCKET_ENDPOINT_PATH	"./socket_endpoint"
#define DEFAULT_TCP_SERVER_PORT				((uint16_t)65000)
#define DEFAULT_TCP_SERVER_ADDR				"127.0.0.1"

#define RECEIVED_DATA_SIZE		(4096)

#define SOH						(0x01) // start of heading
#define EOT						(0x04) // end of transfer
#define ACK						(0x06)
#define NAK						(0x15)


typedef enum {
	EN_RECEIVE_STATE_STANDBY__WAIT_SOH,
	EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_0,
	EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_1,
	EN_RECEIVE_STATE_WORKING__CHECK_SIZE,
	EN_RECEIVE_STATE_WORKING__CHECK_DATA,
	EN_RECEIVE_STATE_WORKING__CHECK_EOT,
} EN_RECEIVE_STATE;


class CLocalSocketClient;
typedef int (CLocalSocketClient:: *cbSetupClientSocket) (void);
typedef int (CLocalSocketClient:: *cbReceiveWrapper) (int fdClientSocket, uint8_t *pBuff, int size);
typedef int (CLocalSocketClient:: *cbSendWrapper) (int fdClientSocket, const uint8_t *pData, int size);

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
	bool sendToServer (const uint8_t *pData, int size);
	CLocalSocketClient::IPacketHandler* getPacketHandler (void);

	void setLocalSocket (void);
	void setTcpSocket (void);

private:
	void onThreadMainRoutine (void);
	void receiveLoop (int fdClientSocket);
	int checkData (uint8_t *pBuff, int size);
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
	uint8_t mCurrentPacket [RECEIVED_DATA_SIZE];
	int mCurrentPacketWritePos;
	uint8_t mCurrentPacketDataSize;
	
	CLocalSocketClient::IPacketHandler *mpPacketHandler;

	pthread_mutex_t mMutex;
	pthread_mutex_t mMutexSend;

	char mSocketEndpointPath [64]; // local
	char mIpAddr [15+1];          // tcp
	uint16_t mPort;               // tcp


	// setLocalSocket / setTcpSocket
	cbSetupClientSocket mpcbSetupClientSocket;
	cbReceiveWrapper mpcbReceiveWrapper;
	cbSendWrapper mpcbSendWrapper;

};

#endif
