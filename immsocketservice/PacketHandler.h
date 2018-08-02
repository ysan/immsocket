#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Message.h"
#include "ImmSocketServiceCommon.h"
#include "SyncRequestManager.h"
#include "AsyncProcProxy.h"


using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {

typedef struct {
	struct _id {
		uint8_t num;
		uint32_t hash;
	} id;
	uint8_t type;
	uint8_t command;
	uint8_t reserve_0;
	uint8_t reserve_1;
	uint8_t size;
} ST_PACKET;


struct ST_PACKET_HANDLED {
public:
	ST_PACKET_HANDLED (CMessage *pMsg, int type) {
		msg = *pMsg;
		msgType = type;
	}

	CMessage msg;
	int msgType;
};


class CPacketHandler : public CImmSocketClient::IPacketHandler
{
public:
	template <typename T>
	class CAsyncHandlerImpl : public IAsyncHandler<T>
	{
	public:
		CAsyncHandlerImpl (CPacketHandler *pHandler)
			:mpPacketHandler (NULL)
		{
			if (pHandler) {
				mpPacketHandler = pHandler;
			}
		}

		virtual ~CAsyncHandlerImpl (void) {}

	private:
		void onAsyncHandled (T arg) {
			ST_PACKET_HANDLED *p = arg;
			if (!p) {
				return ;
			}

			if (mpPacketHandler) {
				mpPacketHandler->handleMsg (&(p->msg), p->msgType);
			}

			delete p;
		}


		CPacketHandler *mpPacketHandler;
	};

public:
	CPacketHandler (void);
	virtual ~CPacketHandler (void);

	CSyncRequestManager *getSyncRequestManager (void) {
		return &mSyncRequestManager;
	}

protected:
	virtual void onHandleRequest (CMessage *pMsg);
	virtual void onHandleReply (CMessage *pMsg);
	virtual void onHandleNotify (CMessage *pMsg);


private:
	void onSetup (CImmSocketClient *pSelf);
	void onTeardown (CImmSocketClient *pSelf);
	void onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size);

	void handleMsg (CMessage *pMsg, int msgType);


	CImmSocketClient *mpClientInstance;
	CSyncRequestManager mSyncRequestManager;
	CAsyncProcProxy<ST_PACKET_HANDLED*> mProxy;
};

} // namespace ImmSocketService

#endif
