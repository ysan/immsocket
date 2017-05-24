#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "WorkerThread.h"
#include "ImmSocketClient.h"
#include "Utils.h"

namespace ImmSocket {

// for client single // imm
CImmSocketClient::CImmSocketClient (void) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for client single // imm
CImmSocketClient::CImmSocketClient (const char *pPath) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	if ((pPath) && (strlen(pPath) > 0)) {
		strncpy (mSocketEndpointPath, pPath, sizeof (mSocketEndpointPath) -1);
	}

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for client single // imm
CImmSocketClient::CImmSocketClient (const char *pPath, CImmSocketClient::IPacketHandler *pHandler) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	if (pHandler) {
		mpPacketHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	if ((pPath) && (strlen(pPath) > 0)) {
		strncpy (mSocketEndpointPath, pPath, sizeof (mSocketEndpointPath) -1);
	}

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for client single // tcp
CImmSocketClient::CImmSocketClient (const char *pszIpAddr, uint16_t port) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	if (pszIpAddr && (strlen(pszIpAddr) > 0)) {
		memcpy (mIpAddr, pszIpAddr, sizeof(mIpAddr) -1);
	}

	mPort = port;

	setTcpSocket ();
}

// for client single // tcp
CImmSocketClient::CImmSocketClient (const char *pszIpAddr, uint16_t port, CImmSocketClient::IPacketHandler *pHandler) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	if (pHandler) {
		mpPacketHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	if (pszIpAddr && (strlen(pszIpAddr) > 0)) {
		memcpy (mIpAddr, pszIpAddr, sizeof(mIpAddr) -1);
	}

	mPort = port;

	setTcpSocket ();
}

// for server side
CImmSocketClient::CImmSocketClient (int fdClientSocket) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	mFdClientSocket = fdClientSocket;

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for server side
CImmSocketClient::CImmSocketClient (int fdClientSocket, CImmSocketClient::IPacketHandler *pHandler) :
	mIsStop (false),
	mState (EN_RECEIVE_STATE_STANDBY__WAIT_SOH),
	mFdClientSocket (0),
	mCurrentPacketWritePos (0),
	mCurrentPacketDataSize(0),
	mpPacketHandler (NULL),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSend, NULL);

	mFdClientSocket = fdClientSocket;

	if (pHandler) {
		mpPacketHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

CImmSocketClient::~CImmSocketClient (void)
{
	pthread_mutex_destroy (&mMutex);
	pthread_mutex_destroy (&mMutexSend);
}


bool CImmSocketClient::startReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (isAlive()) {
		_IMMSOCK_LOG_W ("already started\n");
		return true;
	}

	mIsStop = false;
	return create ();
}

void CImmSocketClient::stopReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;
}

void CImmSocketClient::syncStopReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;

	waitDestroy ();
}

int CImmSocketClient::setupClientSocket (void)
{
	int fd = 0;
	struct sockaddr_un stClientAddr;

	if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		_IMMSOCK_PERROR ("socket");
		return -1;
	}

    memset (&stClientAddr, 0x00, sizeof(stClientAddr));
	stClientAddr.sun_family = AF_UNIX;
	strcpy (stClientAddr.sun_path, mSocketEndpointPath);

	if (connect (fd, (struct sockaddr*)&stClientAddr, sizeof(stClientAddr.sun_family)+strlen(mSocketEndpointPath)) < 0) {
		_IMMSOCK_PERROR ("connect");
		return -1;
	}

	return fd;
}

int CImmSocketClient::setupClientSocket_Tcp (void)
{
	int fd = 0;
	int rtn = 0;
	struct sockaddr_in stClientAddr;

	memset (&stClientAddr, 0x00, sizeof(struct sockaddr_in));
	stClientAddr.sin_family = AF_INET;
	stClientAddr.sin_port = htons (mPort);
//	stClientAddr.sin_addr.s_addr = inet_addr (mIpAddr);
	rtn = inet_aton (mIpAddr, &stClientAddr.sin_addr);
	if (rtn == 0) {
		_IMMSOCK_PERROR ("inet_aton()");
		return -1;
	}

	fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		_IMMSOCK_PERROR ("socket()");
		return -1;
	}

	rtn = connect (fd, (struct sockaddr*)&stClientAddr, sizeof(struct sockaddr));
	if( rtn < 0 ){
		_IMMSOCK_PERROR ("connect()");
		close (fd);
		return -1;
	}

	return fd;
}

bool CImmSocketClient::connectToServer (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket != 0) {
		_IMMSOCK_LOG_W ("already connected\n");
		return true;
	} 

	int fd = (this->*mpcbSetupClientSocket) ();
	if (fd < 0) {
		return  false;
	}

	mFdClientSocket = fd;
	return true;
}

void CImmSocketClient::disconnectFromServer (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket != 0) {
		close (mFdClientSocket);
		mFdClientSocket = 0;
	}
}

bool CImmSocketClient::isConnected (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket == 0) {
		return false;
	} else {
		return true;
	}
}

void CImmSocketClient::onThreadMainRoutine (void)
{
	setName ((char*)"ImmSocketReceiver");
	_IMMSOCK_LOG_I ("%s %s\n", __FILE__, __func__);


	if (mpPacketHandler) {
		mpPacketHandler->onSetup (this);
	}


	// don't use receiveOnce and receiveLoop together
	receiveLoop (mFdClientSocket);


	if (mpPacketHandler) {
		mpPacketHandler->onTeardown (this);
	}


	_IMMSOCK_LOG_W ("%s %s end\n", __FILE__, __func__);

	// thread end
}

int CImmSocketClient::receiveWrapper (int fdClientSocket, uint8_t *pBuff, int size)
{
	int rtn = read (fdClientSocket, pBuff, size);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("read()");
	}

	return rtn;
}

int CImmSocketClient::receiveWrapper_Tcp (int fdClientSocket, uint8_t *pBuff, int size)
{
	int rtn = recv (fdClientSocket, pBuff, size, 0);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("recv()");
	}

	return rtn;
}

void CImmSocketClient::receiveLoop (int fdClientSocket)
{
	uint8_t buff[1024];
	int rtn = 0;
	int chk = 0;
	fd_set stFds;
	struct timeval stTimeout;


	// clear member
	clearReceiveDependVals ();

	while (1) {

//		_IMMSOCK_LOG_I ("select(read) blocking...");
		FD_ZERO (&stFds);
		FD_SET (fdClientSocket, &stFds);
		stTimeout.tv_sec = 1;
		stTimeout.tv_usec = 0;

		rtn = select (fdClientSocket+1, &stFds, NULL, NULL, &stTimeout);
		if (rtn < 0) {
			_IMMSOCK_PERROR ("select()");
			continue;

		} else if (rtn == 0) {
			// timeout
			if (mIsStop) {
				_IMMSOCK_LOG_W ("stop --> receiveLoop break\n");
				break;
			}
		}


		if (FD_ISSET (fdClientSocket, &stFds)) {
			memset (buff, 0x00, sizeof(buff));
			rtn = (this->*mpcbReceiveWrapper) (fdClientSocket, buff, sizeof(buff));
			if (rtn == 0) {
				_IMMSOCK_LOG_N ("disconnect.");
				break;
			} else if (rtn < 0) {
				_IMMSOCK_PERROR ("read()/recv()");
				continue;
			} else {
				_IMMSOCK_LOG_I ("data come  size[%d]\n", rtn);
				if (getLogLevel() <= EN_LOG_LEVEL_I) {
					CUtils::dumper ((const uint8_t*)buff, rtn);
				}

				chk = checkData (buff, rtn);
				if (chk == -1) {
					_IMMSOCK_LOG_E ("checkData()  error\n");
					continue;
				} else if (chk == -2) {
					_IMMSOCK_LOG_E ("checkData()  buffer over\n");
					continue;
				} else {
					_IMMSOCK_LOG_I ("continue\n");
					continue;
				}
			}
		}
	}
}

void CImmSocketClient::receiveOnce (int fdClientSocket)
{
	uint8_t buff[1024];
	int rtn = 0;
	int chk = 0;
	fd_set stFds;
	struct timeval stTimeout;


	// clear member
	clearReceiveDependVals ();

	while (1) {

		FD_ZERO (&stFds);
		FD_SET (fdClientSocket, &stFds);
		stTimeout.tv_sec = 1;
		stTimeout.tv_usec = 0;

		rtn = select (fdClientSocket+1, &stFds, NULL, NULL, &stTimeout);
		if (rtn < 0) {
			_IMMSOCK_PERROR ("select()");
			break;

		} else if (rtn == 0) {
			// timeout
			if (mIsStop) {
				_IMMSOCK_LOG_W ("stop --> receiveOnce break\n");
				break;
			}
		}


		if (FD_ISSET (fdClientSocket, &stFds)) {
			memset (buff, 0x00, sizeof(buff));
			rtn = (this->*mpcbReceiveWrapper) (fdClientSocket, buff, sizeof(buff));
			if (rtn == 0) {
				_IMMSOCK_LOG_N ("disconnect.");
				break;
			} else if (rtn < 0) {
				_IMMSOCK_PERROR ("read()");
				break;
			} else {
				_IMMSOCK_LOG_I ("data come  size[%d]\n", rtn);
				if (getLogLevel() <= EN_LOG_LEVEL_I) {
					CUtils::dumper ((const uint8_t*)buff, rtn);
				}

				chk = checkData (buff, rtn, true);
				if (chk == -1) {
					_IMMSOCK_LOG_E ("checkData()  error\n");
					break;
				} else if (chk == -2) {
					_IMMSOCK_LOG_E ("checkData()  buffer over\n");
					break;
				} else if (chk == 1) {
					// packet complete
					break;
				} else {
					_IMMSOCK_LOG_I ("continue\n");
					continue;
				}
			}
		}
	}
}

void CImmSocketClient::syncReceivePacketLoop (void)
{
	if (mpPacketHandler) {
		mpPacketHandler->onSetup (this);
	}


	// don't use receiveOnce and receiveLoop together
	receiveLoop (mFdClientSocket);


	if (mpPacketHandler) {
		mpPacketHandler->onTeardown (this);
	}
}

int CImmSocketClient::syncReceivePacketOnce (uint8_t *pBuff, int size)
{
	if (!pBuff || size <= 0) {
		return 0;
	}

	// don't use receiveOnce and receiveLoop together
	receiveOnce (mFdClientSocket);

	int cpsize = size > mCurrentPacketWritePos ? mCurrentPacketWritePos : size;
	memcpy (pBuff, mCurrentPacket, cpsize);

	return cpsize;
}

void CImmSocketClient::clearReceiveDependVals (void)
{
	mState = EN_RECEIVE_STATE_STANDBY__WAIT_SOH;
	memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
	mCurrentPacketWritePos = 0;
	mCurrentPacketDataSize = 0;
}

int CImmSocketClient::checkData (uint8_t *pBuff, int size, bool isOnce)
{
	if ((!pBuff) || (size == 0)) {
		return -1;
	}


	while (size > 0) {

		if (mState == EN_RECEIVE_STATE_STANDBY__WAIT_SOH) {
			// wait SOH

			if (*pBuff == SOH) {
				mState = EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_0;
			} else {
				_IMMSOCK_LOG_W ("unexpected data --> drop\n");
			}

		} else if (mState == EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_0) {
			mState = EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_1;

		} else if (mState == EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_1) {
			mState = EN_RECEIVE_STATE_WORKING__CHECK_SIZE;

		} else if (mState == EN_RECEIVE_STATE_WORKING__CHECK_SIZE) {

			mCurrentPacketDataSize = *pBuff;

			if (mCurrentPacketDataSize > 0) {
				// data exist
				mState = EN_RECEIVE_STATE_WORKING__CHECK_DATA;
			} else {
				// data nothing
				mState = EN_RECEIVE_STATE_WORKING__CHECK_EOT;
			}

		} else if (mState == EN_RECEIVE_STATE_WORKING__CHECK_DATA) {
//TODO
			mCurrentPacket [mCurrentPacketWritePos] = *pBuff;
			mCurrentPacketWritePos ++;

			mCurrentPacketDataSize --;
			if (mCurrentPacketDataSize == 0) {
				mState = EN_RECEIVE_STATE_WORKING__CHECK_EOT;
			}

		} else if (mState == EN_RECEIVE_STATE_WORKING__CHECK_EOT) {

			if (*pBuff == EOT) {
				// EOT
				if (mCurrentPacketWritePos > 0) {
					_IMMSOCK_LOG_N ("packet complete\n");
					if (getLogLevel() <= EN_LOG_LEVEL_I) {
						CUtils::dumper ((const uint8_t*)mCurrentPacket, mCurrentPacketWritePos);
					}

					if (isOnce) {
						return 1;
					}

					if (mpPacketHandler) {
						mpPacketHandler->onReceivePacket (this, mCurrentPacket, mCurrentPacketWritePos);
					}

				} else {
					_IMMSOCK_LOG_N ("null packet\n");
				}

				// clear member
				clearReceiveDependVals ();

			} else {
				// invalid packet(invalid EOT)
				_IMMSOCK_LOG_E ("invalid packet(invalid EOT)\n");

				// clear member
				clearReceiveDependVals ();
			}
		}


		if (mCurrentPacketWritePos >= RECEIVED_SIZE_MAX) {
			// buffer over
			_IMMSOCK_LOG_E ("buffer over(mCurrentPacket)\n");

			// clear member
			clearReceiveDependVals ();

			return -2;
		}

		pBuff ++;
		size --;
	}

	return 0;
}

int CImmSocketClient::getFd (void)
{
	return mFdClientSocket;
}

bool CImmSocketClient::sendToConnection (const uint8_t *pData, int size)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSend);

	if ((!pData) || (size <= 0) || (size > 0xff)) {
		return false;
	}


	int totalsize = size + 5; // append 5bytes area => (SOH, reserve, reserve, size, EOT)
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	if (!setPacket (pData, size, buff, totalsize)) {
		_IMMSOCK_LOG_E ("setPacket() is failure.\n");
		return false;
	}


	int rtn = (this->*mpcbSendWrapper) (mFdClientSocket, buff, totalsize);
	if (rtn < 0) {
		return false;
	}

	return true;
}

bool CImmSocketClient::setPacket (const uint8_t *pIn, int insize, uint8_t *pOut, int outsize)
{
	if ((!pIn) || (insize <= 0) || (insize > 0xff)) {
		return false;
	}

	if ((!pOut) || (outsize < (insize+5))) {
		return false;
	}

	*(pOut+0) = SOH;
	*(pOut+1) = 0x00; // reserve
	*(pOut+2) = 0x00; // reserve
	*(pOut+3) = (uint8_t)insize;
	memcpy (pOut+4, pIn, insize);
	pOut [outsize -1] = EOT;

	return true;
}

int CImmSocketClient::sendWrapper (int fdClientSocket, const uint8_t *pData, int size)
{
	int rtn = write (fdClientSocket, pData, size);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("write");
	}

	return rtn;
}

int CImmSocketClient::sendWrapper_Tcp (int fdClientSocket, const uint8_t *pData, int size)
{
	int rtn = send (fdClientSocket, pData, size, 0);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("send");
	}

	return rtn;
}

CImmSocketClient::IPacketHandler *CImmSocketClient::getPacketHandler (void)
{
	return mpPacketHandler;
}

// socket config
void CImmSocketClient::setLocalSocket (void)
{
	mpcbSetupClientSocket = &CImmSocketClient::setupClientSocket;
	mpcbReceiveWrapper = &CImmSocketClient::receiveWrapper;
	mpcbSendWrapper = &CImmSocketClient::sendWrapper;
}

// socket config
void CImmSocketClient::setTcpSocket (void)
{
	mpcbSetupClientSocket = &CImmSocketClient::setupClientSocket_Tcp;
	mpcbReceiveWrapper = &CImmSocketClient::receiveWrapper_Tcp;
	mpcbSendWrapper = &CImmSocketClient::sendWrapper_Tcp;
}

} // namespace ImmSocket
