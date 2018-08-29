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
#include "Common.h"
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


typedef struct _packet_handle_info {
public:
	_packet_handle_info (CMessage *pMsg, int type) {
		msg = *pMsg;
		msgType = type;
	}

	CMessage msg;
	int msgType;
} ST_PACKET_HANDLE_INFO;


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
		void onAsyncHandled (T arg) override {
			_packet_handle_info *p = arg;
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
	explicit CPacketHandler (int threadPoolNum=1);
	virtual ~CPacketHandler (void);

	CSyncRequestManager *getSyncRequestManager (void) {
		return &mSyncRequestManager;
	}

protected:
	virtual void onHandleRequest (CMessage *pMsg);
	virtual void onHandleReply (CMessage *pMsg);
	virtual void onHandleNotify (CMessage *pMsg);


private:
	void onSetup (CImmSocketClient *pSelf) override;
	void onTeardown (CImmSocketClient *pSelf) override;
	void onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size) override;

	void handleMsg (CMessage *pMsg, int msgType);


	CImmSocketClient *mpClientInstance;
	CSyncRequestManager mSyncRequestManager;
	CAsyncProcProxy<_packet_handle_info*> mProxy;
};

} // namespace ImmSocketService

#endif
