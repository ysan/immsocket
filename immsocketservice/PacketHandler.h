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
#include "AsyncProcProxy.h"
#include "ImmSocketServiceCommon.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {

typedef struct {
	uint8_t id;
	uint8_t type;
	uint8_t command;
	uint8_t reserve_0;
	uint8_t reserve_1;
	uint8_t size;
} ST_PACKET;

typedef map<uint8_t, CMessage*> SYNC_REQUEST_TABLE; // <id, CMessage>


class CPacketHandler : public CImmSocketClient::IPacketHandler
{
public:
	friend class CAsyncProcProxy;

	CPacketHandler (void);
	virtual ~CPacketHandler (void);


	void addSyncRequestTable (CMessage *pMsg);
	void removeSyncRequestTable (CMessage *pMsg);
	CMessage *findSyncRequestMessage (uint8_t id);
	void checkSyncRequestMessage (CMessage *pMsg);

	uint8_t generateId (void);


protected:
	virtual void onHandleRequest (CMessage *pMsg);
	virtual void onHandleReply (CMessage *pMsg);
	virtual void onHandleNotify (CMessage *pMsg);


private:
	void onSetup (CImmSocketClient *pSelf);
	void onTeardown (CImmSocketClient *pSelf);
	void onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size);
	void handleMsg (CMessage *pMsg, int msgType); // friend access


	CImmSocketClient *mpClientInstance;

	uint8_t mId;
	pthread_mutex_t mMutexGenId;

	SYNC_REQUEST_TABLE mSyncRequestTable;
	pthread_mutex_t mMutexSyncRequestTable;

	CAsyncProcProxy mAsyncProcProxy;

};

} // namespace ImmSocketService

#endif
