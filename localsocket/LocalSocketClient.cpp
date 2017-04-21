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
#include "LocalSocketClient.h"
#include "Utils.h"

namespace LocalSocket {

// for client single // local
CLocalSocketClient::CLocalSocketClient (void) :
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
	strncpy (mSocketEndpointPath, DEFAULT_LOCALSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for client single // local
CLocalSocketClient::CLocalSocketClient (const char *pPath) :
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

// for client single // local
CLocalSocketClient::CLocalSocketClient (const char *pPath, CLocalSocketClient::IPacketHandler *pHandler) :
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
CLocalSocketClient::CLocalSocketClient (const char *pszIpAddr, uint16_t port) :
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
	strncpy (mSocketEndpointPath, DEFAULT_LOCALSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	if (pszIpAddr && (strlen(pszIpAddr) > 0)) {
		memcpy (mIpAddr, pszIpAddr, sizeof(mIpAddr) -1);
	}

	mPort = port;

	setTcpSocket ();
}

// for client single // tcp
CLocalSocketClient::CLocalSocketClient (const char *pszIpAddr, uint16_t port, CLocalSocketClient::IPacketHandler *pHandler) :
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
	strncpy (mSocketEndpointPath, DEFAULT_LOCALSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	if (pszIpAddr && (strlen(pszIpAddr) > 0)) {
		memcpy (mIpAddr, pszIpAddr, sizeof(mIpAddr) -1);
	}

	mPort = port;

	setTcpSocket ();
}

// for server side
CLocalSocketClient::CLocalSocketClient (int fdClientSocket) :
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
	strncpy (mSocketEndpointPath, DEFAULT_LOCALSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

// for server side
CLocalSocketClient::CLocalSocketClient (int fdClientSocket, CLocalSocketClient::IPacketHandler *pHandler) :
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
	strncpy (mSocketEndpointPath, DEFAULT_LOCALSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	memset (mIpAddr, 0x00, sizeof(mIpAddr));
	memcpy (mIpAddr, DEFAULT_TCP_SERVER_ADDR, strlen(DEFAULT_TCP_SERVER_ADDR));

	setLocalSocket ();
}

CLocalSocketClient::~CLocalSocketClient (void)
{
	pthread_mutex_destroy (&mMutex);
	pthread_mutex_destroy (&mMutexSend);
}


bool CLocalSocketClient::startReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (isAlive()) {
		_LSOCK_LOG_W ("already started\n");
		return true;
	}


	if (mpPacketHandler) {
		mpPacketHandler->onSetup (this);
	}

	return create ();
}

void CLocalSocketClient::stopReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);


	if (mpPacketHandler) {
		mpPacketHandler->onTeardown (this);
	}

	mIsStop = true;
}

void CLocalSocketClient::syncStopReceiver (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);


	if (mpPacketHandler) {
		mpPacketHandler->onTeardown (this);
	}

	mIsStop = true;

	waitDestroy ();
}

int CLocalSocketClient::setupClientSocket (void)
{
	int fd = 0;
	struct sockaddr_un stClientAddr;

	if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		_UTL_PERROR ("socket");
		return -1;
	}

    memset (&stClientAddr, 0x00, sizeof(stClientAddr));
	stClientAddr.sun_family = AF_UNIX;
	strcpy (stClientAddr.sun_path, mSocketEndpointPath);

	if (connect (fd, (struct sockaddr*)&stClientAddr, sizeof(stClientAddr.sun_family)+strlen(mSocketEndpointPath)) < 0) {
		_UTL_PERROR ("connect");
		return -1;
	}

	return fd;
}

int CLocalSocketClient::setupClientSocket_Tcp (void)
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
		_UTL_PERROR ("inet_aton()");
		return -1;
	}

	fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		_UTL_PERROR ("socket()");
		return -1;
	}

	rtn = connect (fd, (struct sockaddr*)&stClientAddr, sizeof(struct sockaddr));
	if( rtn < 0 ){
		_UTL_PERROR ("connect()");
		close (fd);
		return -1;
	}

	return fd;
}

bool CLocalSocketClient::connectToServer (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket != 0) {
		_LSOCK_LOG_W ("already connected\n");
		return true;
	} 

	int fd = (this->*mpcbSetupClientSocket) ();
	if (fd < 0) {
		return  false;
	}

	mFdClientSocket = fd;
	return true;
}

void CLocalSocketClient::disconnectFromServer (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket != 0) {
		close (mFdClientSocket);
		mFdClientSocket = 0;
	}
}

bool CLocalSocketClient::isConnected (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdClientSocket == 0) {
		return false;
	} else {
		return true;
	}
}

void CLocalSocketClient::onThreadMainRoutine (void)
{
	setName ((char*)"LocalSocketReceiver");
	_LSOCK_LOG_I ("%s %s\n", __FILE__, __func__);


	receiveLoop (mFdClientSocket);


	_LSOCK_LOG_W ("%s %s end\n", __FILE__, __func__);

	// thread end
}

int CLocalSocketClient::receiveWrapper (int fdClientSocket, uint8_t *pBuff, int size)
{
	int rtn = read (fdClientSocket, pBuff, size);
	if (rtn < 0) {
		_UTL_PERROR ("read()");
	}

	return rtn;
}

int CLocalSocketClient::receiveWrapper_Tcp (int fdClientSocket, uint8_t *pBuff, int size)
{
	int rtn = recv (fdClientSocket, pBuff, size, 0);
	if (rtn < 0) {
		_UTL_PERROR ("recv()");
	}

	return rtn;
}

void CLocalSocketClient::receiveLoop (int fdClientSocket)
{
	uint8_t buff[1024];
	int rtn = 0;
	int chk = 0;
	fd_set stFds;
	struct timeval stTimeout;


	while (1) {

//		_LSOCK_LOG_I ("select(read) blocking...");
		FD_ZERO (&stFds);
		FD_SET (fdClientSocket, &stFds);
		stTimeout.tv_sec = 1;
		stTimeout.tv_usec = 0;

		rtn = select (fdClientSocket+1, &stFds, NULL, NULL, &stTimeout);
		if (rtn < 0) {
			_UTL_PERROR ("select()");
			continue;

		} else if (rtn == 0) {
			// timeout
			if (mIsStop) {
				_LSOCK_LOG_W ("stop --> receiveLoop break\n");
				break;
			}
		}


		if (FD_ISSET (fdClientSocket, &stFds)) {
			memset (buff, 0x00, sizeof(buff));
			rtn = (this->*mpcbReceiveWrapper) (fdClientSocket, buff, sizeof(buff));
			if (rtn == 0) {
				_LSOCK_LOG_N ("disconnect.");
				break;
			} else if (rtn < 0) {
				_UTL_PERROR ("read()");
				continue;
			} else {
				_LSOCK_LOG_I ("data come  size[%d]\n", rtn);
				CUtils::dumper ((const uint8_t*)buff, rtn, true, getLogLevel());


				chk = checkData (buff, rtn);
				if (chk == -1) {
					_LSOCK_LOG_E ("error checkData()\n");
					continue;
				} else if (chk == -2) {
					continue;
				} else {
					_LSOCK_LOG_I ("continue\n");
					continue;
				}
			}
		}

	}
}

int CLocalSocketClient::checkData (uint8_t *pBuff, int size)
{
	if ((!pBuff) || (size == 0)) {
		return -1;
	}


	while (1) {

		if (mState == EN_RECEIVE_STATE_STANDBY__WAIT_SOH) {
			// wait SOH

			if (*pBuff == SOH) {
				mState = EN_RECEIVE_STATE_WORKING__CHECK_RESERVE_0;
			} else {
				_LSOCK_LOG_W ("unexpected data --> drop\n");
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
					_LSOCK_LOG_N ("packet complete\n");
					CUtils::dumper ((const uint8_t*)mCurrentPacket, mCurrentPacketWritePos, true, getLogLevel());

					if (mpPacketHandler) {
						mpPacketHandler->onReceivePacket (this, mCurrentPacket, mCurrentPacketWritePos);
					}

				} else {
					_LSOCK_LOG_N ("null packet\n");
				}

				// clear member
				mState = EN_RECEIVE_STATE_STANDBY__WAIT_SOH;
				memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
				mCurrentPacketWritePos = 0;
				mCurrentPacketDataSize = 0;

			} else {
				// invalid packet(invalid EOT)
				_LSOCK_LOG_E ("invalid packet(invalid EOT)\n");

				// clear member
				mState = EN_RECEIVE_STATE_STANDBY__WAIT_SOH;
				memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
				mCurrentPacketWritePos = 0;
				mCurrentPacketDataSize = 0;
			}
		}


		if (mCurrentPacketWritePos >= RECEIVED_DATA_SIZE) {
			// buffer over
			_LSOCK_LOG_E ("buffer over(mCurrentPacket)\n");

			// clear member
			mState = EN_RECEIVE_STATE_STANDBY__WAIT_SOH;
			memset (mCurrentPacket, 0x00, sizeof (mCurrentPacket));
			mCurrentPacketWritePos = 0;
			mCurrentPacketDataSize = 0;

			return -2;
		}

		pBuff ++;
		size --;

		if (size == 0) {
			break;
		}
	}

	return 0;
}

int CLocalSocketClient::getFd (void)
{
	return mFdClientSocket;
}

bool CLocalSocketClient::sendToConnection (const uint8_t *pData, int size)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSend);

	if ((!pData) || (size <= 0) || (size > 0xff)) {
		return false;
	}


	int totalsize = size + 5; // append 5bytes area => (SOH, reserve, reserve, size, EOT)
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	if (!setPacket (pData, size, buff, totalsize)) {
		_LSOCK_LOG_E ("setPacket() is failure.\n");
		return false;
	}


	int rtn = (this->*mpcbSendWrapper) (mFdClientSocket, buff, totalsize);
	if (rtn < 0) {
		return false;
	}

	return true;
}

bool CLocalSocketClient::setPacket (const uint8_t *pIn, int insize, uint8_t *pOut, int outsize)
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

int CLocalSocketClient::sendWrapper (int fdClientSocket, const uint8_t *pData, int size)
{
	int rtn = write (fdClientSocket, pData, size);
	if (rtn < 0) {
		_UTL_PERROR ("write");
	}

	return rtn;
}

int CLocalSocketClient::sendWrapper_Tcp (int fdClientSocket, const uint8_t *pData, int size)
{
	int rtn = send (fdClientSocket, pData, size, 0);
	if (rtn < 0) {
		_UTL_PERROR ("send");
	}

	return rtn;
}

CLocalSocketClient::IPacketHandler *CLocalSocketClient::getPacketHandler (void)
{
	return mpPacketHandler;
}

void CLocalSocketClient::setLocalSocket (void)
{
	mpcbSetupClientSocket = &CLocalSocketClient::setupClientSocket;
	mpcbReceiveWrapper = &CLocalSocketClient::receiveWrapper;
	mpcbSendWrapper = &CLocalSocketClient::sendWrapper;
}

void CLocalSocketClient::setTcpSocket (void)
{
	mpcbSetupClientSocket = &CLocalSocketClient::setupClientSocket_Tcp;
	mpcbReceiveWrapper = &CLocalSocketClient::receiveWrapper_Tcp;
	mpcbSendWrapper = &CLocalSocketClient::sendWrapper_Tcp;
}

} // namespace LocalSocket
