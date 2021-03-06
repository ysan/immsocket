#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#include "WorkerThread.h"
#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"


namespace ImmSocket {

// local
#if 0
CImmSocketServer::CImmSocketServer (void) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	setLocalSocket ();
}
#endif

// local echo server
CImmSocketServer::CImmSocketServer (const char *pPath) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	if ((pPath) && (strlen(pPath) > 0)) {
		strncpy (mSocketEndpointPath, pPath, sizeof (mSocketEndpointPath) -1);
	}

	setLocalSocket ();
}

// local // multi client
CImmSocketServer::CImmSocketServer (const char *pPath, CImmSocketServer::IClientHandler *pHandler) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	if (pHandler) {
		mpClientHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	if ((pPath) && (strlen(pPath) > 0)) {
		strncpy (mSocketEndpointPath, pPath, sizeof (mSocketEndpointPath) -1);
	}

	setLocalSocket ();
}

// local // single client
CImmSocketServer::CImmSocketServer (const char *pPath, CImmSocketClient::IPacketHandler *pHandler) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	if (pHandler) {
		mpPacketHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	if ((pPath) && (strlen(pPath) > 0)) {
		strncpy (mSocketEndpointPath, pPath, sizeof (mSocketEndpointPath) -1);
	}

	setLocalSocket ();
}

// tcp echo server
CImmSocketServer::CImmSocketServer (uint16_t port) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	mPort = port;

	setTcpSocket ();
}

// tcp // multi client
CImmSocketServer::CImmSocketServer (uint16_t port, CImmSocketServer::IClientHandler *pHandler) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	if (pHandler) {
		mpClientHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	mPort = port;

	setTcpSocket ();
}

// tcp // single client
CImmSocketServer::CImmSocketServer (uint16_t port, CImmSocketClient::IPacketHandler *pHandler) :
	mFdServerSocket (0),
	mpClientHandler (NULL),
	mpPacketHandler (NULL),
	mIsStop (false),
	mPort (DEFAULT_TCP_SERVER_PORT)
{
	init ();

	if (pHandler) {
		mpPacketHandler = pHandler;
	}

	memset (mSocketEndpointPath, 0x00, sizeof(mSocketEndpointPath));
	strncpy (mSocketEndpointPath, DEFAULT_IMMSOCKET_ENDPOINT_PATH, sizeof(mSocketEndpointPath)-1);

	mPort = port;

	setTcpSocket ();
}

CImmSocketServer::~CImmSocketServer (void)
{
	finaliz ();
}


void CImmSocketServer::init (void)
{
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&mMutexClientTable, &attr);
}

void CImmSocketServer::finaliz (void)
{
	pthread_mutex_destroy (&mMutex);
	pthread_mutex_destroy (&mMutexClientTable);
}

bool CImmSocketServer::start (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (isAlive()) {
		_IMMSOCK_LOG_W ("already started\n");
		return true;
	}

	mIsStop = false;
	return create ();
}

void CImmSocketServer::stop (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;
}

void CImmSocketServer::syncStop (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;

	waitDestroy ();
}

bool CImmSocketServer::isStarted (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (mFdServerSocket == 0) {
		return false;
	} else {
		return true;
	}
}

void CImmSocketServer::onThreadMainRoutine (void)
{
	setName ((char*)"ImmSocketServer");
	_IMMSOCK_LOG_I ("%s %s\n", __FILE__, __func__);


	int fd = (this->*mpfnSetupServerSocket) ();
	if (fd < 0) {
		return  ;
	}

	mFdServerSocket = fd;
	acceptLoop (fd);

	forceClearClientTable ();

	_IMMSOCK_LOG_W ("server socket:[%d] close\n", fd);
	close (fd);
	mFdServerSocket = 0;


	_IMMSOCK_LOG_W ("%s %s end...\n", __FILE__, __func__);

	// thread end
}

int CImmSocketServer::setupServerSocket (void)
{
	int fd = 0;
	int rtn = 0;
	struct sockaddr_un stServerAddr;

	unlink (mSocketEndpointPath);

	if ((fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		_IMMSOCK_PERROR ("socket");
		return -1;
	}

	memset (&stServerAddr, 0x00, sizeof(stServerAddr));
	stServerAddr.sun_family = AF_UNIX;
	strcpy (stServerAddr.sun_path, mSocketEndpointPath);

#ifdef _ANDROID_BUILD
	bind (fd, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr.sun_family)+strlen(mSocketEndpointPath));
#else
	rtn = bind (fd, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr.sun_family)+strlen(mSocketEndpointPath));
	if (rtn < 0) {
		_IMMSOCK_PERROR ("bind");
		return -1;
	}
#endif

	rtn = chmod (mSocketEndpointPath, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("chmod");
		return -1;
	}

	rtn = listen (fd, SOMAXCONN);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("listen");
		return -1;
	}

	return fd;
}

int CImmSocketServer::setupServerSocket_Tcp (void)
{
	int fd = 0;
	int rtn = 0;
	struct sockaddr_in stServerAddr;

	memset (&stServerAddr, 0x00, sizeof(struct sockaddr_in));
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	stServerAddr.sin_port = htons (mPort);

	fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		_IMMSOCK_PERROR ("socket()");
		return -1;
	}

	int optval = 1;
	rtn = setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (rtn < 0) {
		_IMMSOCK_PERROR ("setsockopt()");
		close (fd);
		return -1;
	}

#ifdef _ANDROID_BUILD
	bind (fd, (struct sockaddr*)&stServerAddr, sizeof(struct sockaddr));
#else
	rtn = bind (fd, (struct sockaddr*)&stServerAddr, sizeof(struct sockaddr));
	if (rtn < 0) {
		_IMMSOCK_PERROR ("bind()");
		close (fd);
		return -1;
	}
#endif

	rtn = listen (fd, SOMAXCONN);
	if (rtn < 0) {
		_IMMSOCK_PERROR ("listen()");
		close (fd);
		return -1;
	}

	return fd;
}

int CImmSocketServer::acceptWrapper (int fdServerSocket)
{
	int fdClientSocket = 0;
	struct sockaddr_un stClientAddr;
	socklen_t len = sizeof(struct sockaddr_un);

	memset (&stClientAddr, 0x00, sizeof(stClientAddr));
	fdClientSocket = accept (fdServerSocket, (struct sockaddr*)&stClientAddr, &len);

	return fdClientSocket;
}

int CImmSocketServer::acceptWrapper_Tcp (int fdServerSocket)
{
	int fdClientSocket = 0;
	struct sockaddr_in stClientAddr;
	socklen_t len = (socklen_t)sizeof(struct sockaddr_in);

	memset (&stClientAddr, 0x00, sizeof(stClientAddr));
	fdClientSocket = accept (fdServerSocket, (struct sockaddr*)&stClientAddr, &len);

	return fdClientSocket;
}

void CImmSocketServer::acceptLoop (int fdServerSocket)
{
	int fdClientSocket = 0;
	fd_set stFds;
	int rtn = 0;
	struct timeval stTimeout;
	int n = 0;


	while (1) {

//		_IMMSOCK_LOG_I ("select(accept) blocking...");
		FD_ZERO (&stFds);
		FD_SET (fdServerSocket, &stFds);
		stTimeout.tv_sec = 1;
		stTimeout.tv_usec = 0;

		rtn = select (fdServerSocket+1, &stFds, NULL, NULL, &stTimeout);
		if (rtn < 0) {
			_IMMSOCK_PERROR ("select()");
			continue;

		} else if (rtn == 0) {
			// timeout
			refreshClientTable ();

			if (mIsStop) {
				_IMMSOCK_LOG_W ("stop --> acceptLoop break\n");
				break;
			}


			++ n;
			if (n >= 10) {
				dumpClientTable ();
				n = 0;
			}
		}


		if (FD_ISSET (fdServerSocket, &stFds)) {
			if ((fdClientSocket = (this->*mpfnAcceptWrapper) (fdServerSocket)) < 0) {
				_IMMSOCK_PERROR ("accept");
				continue ;
			}

			_IMMSOCK_LOG_I ("accepted fdClientSocket:[%d]\n", fdClientSocket);

			CImmSocketClient *pClient = NULL;

			if (mpPacketHandler) {
				//---- single client ----

				pClient = new CImmSocketClient (fdClientSocket, mpPacketHandler);
				if (mIsConfigLocal) {
					pClient->setLocalSocket();
				} else {
					pClient->setTcpSocket();
				}

				// ## sync ##
				pClient->syncReceivePacketLoop ();

				_IMMSOCK_LOG_W ("- single client - client socket:[%d] --> instance delete\n", fdClientSocket);
				delete pClient;
				pClient = NULL;

				_IMMSOCK_LOG_W ("- single client - client socket:[%d] close\n", fdClientSocket);
				close (fdClientSocket);

			} else {
				//---- multi client ----

				if (mpClientHandler) {

					pClient = mpClientHandler->onAcceptClient (fdClientSocket);

					addClientTable (fdClientSocket, pClient);

				} else {
					// default is echo server
					pClient = new CImmSocketClient (fdClientSocket);
					if (mIsConfigLocal) {
						pClient->setLocalSocket();
					} else {
						pClient->setTcpSocket();
					}

					// async
					pClient->startReceiver ();

					addClientTable (fdClientSocket, pClient, true);
				}
			}
		}
	}

}

void CImmSocketServer::addClientTable (int fd, CImmSocketClient *pInstance, bool isEchoMode)
{
	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter;
	iter = mClientTable.find (fd);

	if (iter == mClientTable.end()) {
		// new
		ST_CLIENT_INFO st = {fd, pInstance, isEchoMode};
		mClientTable.insert (pair<int, ST_CLIENT_INFO>(fd, st));

	} else {
//TODO
		// already use
		removeClientTable (fd);
		addClientTable (fd, pInstance, isEchoMode);
	}
}

// clientThread instance delete in
bool CImmSocketServer::removeClientTable (int fd)
{
	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter;
	iter = mClientTable.find (fd);

	if (iter != mClientTable.end()) {

		CImmSocketClient *pClient = iter->second.pInstance;
		if (pClient) {
			if (iter->second.isEchoMode) {
				pClient->syncStopReceiver (); // thread end

				_IMMSOCK_LOG_W ("client socket:[%d] --> instance delete\n", fd);
				delete iter->second.pInstance;
				iter->second.pInstance = NULL;

				_IMMSOCK_LOG_W ("client socket:[%d] close\n", fd);
				close (fd);

			} else {

				if (mpClientHandler) {
					mpClientHandler->onRemoveClient (pClient);
				}

				_IMMSOCK_LOG_W ("client socket:[%d] close\n", fd);
				close (fd);
			}
		}

		mClientTable.erase (fd);
		return true;
	}

	return false;
}

void CImmSocketServer::refreshClientTable (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter = mClientTable.begin();

	while (iter != mClientTable.end()) {
		CImmSocketClient *pClient = iter->second.pInstance;
		if (pClient) {
			bool isAlive = pClient->isAlive(); // Always false if do not startReceiver
			int fd = pClient->getFd();
			if (!isAlive) {
				if (removeClientTable (fd)) {
					// iter update
					iter = mClientTable.begin();
					continue;
				}
			}
		}

		++ iter;
	}
}

void CImmSocketServer::forceClearClientTable (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter = mClientTable.begin();

	while (iter != mClientTable.end()) {
		CImmSocketClient *pClient = iter->second.pInstance;
		if (pClient) {
			int fd = pClient->getFd();
			if (removeClientTable (fd)) {
				// iter update
				iter = mClientTable.begin();
				continue;
			}
		}

		++ iter;
	}
}

void CImmSocketServer::dumpClientTable (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter = mClientTable.begin();

	_IMMSOCK_LOG_I ("--- dumpClientTable ---\n");

	while (iter != mClientTable.end()) {

		CImmSocketClient *pClient = iter->second.pInstance;
		if (pClient) {
			bool isAlive = pClient->isAlive();
			int fd = pClient->getFd();
			_IMMSOCK_LOG_I (" fd:[%d] isAlive:[%d]\n", fd, isAlive);
		}

		++ iter;
	}

	_IMMSOCK_LOG_I ("-----------------------\n");
}

void CImmSocketServer::sendToClient (uint8_t *pData, int size)
{
	// args check in pClient->sendToConnection

	CUtils::CScopedMutex scopedMutex (&mMutexClientTable);

	CLIENT_TABLE::iterator iter = mClientTable.begin();

	while (iter != mClientTable.end()) {

		CImmSocketClient *pClient = iter->second.pInstance;
		if (pClient) {
			pClient->sendToConnection (pData, size);
		}

		++ iter;
	}
}

// socket config
void CImmSocketServer::setLocalSocket (void)
{
	mpfnSetupServerSocket = &CImmSocketServer::setupServerSocket;
	mpfnAcceptWrapper = &CImmSocketServer::acceptWrapper;
	mIsConfigLocal = true;
}

// socket config
void CImmSocketServer::setTcpSocket (void)
{
	mpfnSetupServerSocket = &CImmSocketServer::setupServerSocket_Tcp;
	mpfnAcceptWrapper = &CImmSocketServer::acceptWrapper_Tcp;
	mIsConfigLocal = false;
}

} // namespace ImmSocket
