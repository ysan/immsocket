#ifndef _CLIENT_HANDLER_IMPL_H_
#define _CLIENT_HANDLER_IMPL_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "Utils.h"
#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Message.h"
#include "PacketHandler.h"


using namespace std;


namespace ImmSocketService {

using namespace ImmSocket;


template <typename T>
class CClientHandler : public CImmSocketServer::IClientHandler
{
public:
	explicit CClientHandler (int threadPoolNum = 1)
		: mThreadPoolNum (1)
	{
		static_assert (std::is_base_of<CPacketHandler, T>::value, "T is not inherit CPacketHandler.");

		if (threadPoolNum > 1) {
			mThreadPoolNum = threadPoolNum;
		}
	}
	virtual ~CClientHandler (void) {}

private:
	CImmSocketClient* onAcceptClient (int fdClientSocket) override {
		_ISS_LOG_N ("%s\n", __PRETTY_FUNCTION__);

		T *pHandler = new T (mThreadPoolNum);
		CImmSocketClient *pClient = new CImmSocketClient (fdClientSocket, pHandler);
		pClient->setTcpSocket ();
		pClient->startReceiver ();

		return pClient;
	}

	void onRemoveClient (CImmSocketClient *pClient) override {
		_ISS_LOG_N ("%s\n", __PRETTY_FUNCTION__);

		if (!pClient) {
			return ;
		}

		pClient->syncStopReceiver ();

		CImmSocketClient::IPacketHandler *pHandler = pClient->getPacketHandler();
		if (pHandler) {
			_ISS_LOG_N ("client socket:[%d] --> packetHandler delete\n", pClient->getFd());
			delete pHandler;
			pHandler = NULL;
		}

		_ISS_LOG_N ("client socket:[%d] --> instance delete\n", pClient->getFd());
		delete pClient;
		pClient = NULL;
	}


	int mThreadPoolNum;
};

} // namespace ImmSocketService

#endif
