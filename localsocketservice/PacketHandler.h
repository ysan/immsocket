#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "LocalSocketServer.h"
#include "LocalSocketClient.h"
#include "Message.h"
#include "AsyncProcProxy.h"

using namespace std;

namespace LocalSocketService {

typedef struct {
	uint8_t id;
	uint8_t type;
	uint8_t command;
	uint8_t reserve_0;
	uint8_t reserve_1;
	uint8_t size;
} ST_PACKET;

typedef map<uint8_t, CMessage*> SYNC_REQUEST_TABLE; // <id, CMessage>


class CPacketHandler : public CLocalSocketClient::IPacketHandler
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
	void onSetup (CLocalSocketClient *pSelf);
	void onTeardown (CLocalSocketClient *pSelf);
	void onReceivePacket (CLocalSocketClient *pSelf, uint8_t *pPacket, int size);
	void handleMsg (CMessage *pMsg, int msgType); // friend access


	CLocalSocketClient *mpClientInstance;

	uint8_t mId;
	pthread_mutex_t mMutexGenId;

	SYNC_REQUEST_TABLE mSyncRequestTable;
	pthread_mutex_t mMutexSyncRequestTable;

	CAsyncProcProxy mAsyncProcProxy;

};

} // namespace LocalSocketService

#endif
