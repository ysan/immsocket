#ifndef _SYNC_REQUEST_MANAGER_H_
#define _SYNC_REQUEST_MANAGER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Common.h"
#include "Message.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {

typedef vector <CMessage*> SYNC_REQUEST_TABLE;

class CSyncRequestManager
{
public:
	CSyncRequestManager (void);
	virtual ~CSyncRequestManager (void);


	void addSyncRequestTable (CMessage *pMsg);
	void removeSyncRequestTable (CMessage *pMsg);
	CMessage *findSyncRequestMessage (const CMessageId::CId *pId);
	void checkSyncRequestMessage (CMessage *pMsg);

private:
	SYNC_REQUEST_TABLE mSyncRequestTable;
	pthread_mutex_t mMutexSyncRequestTable;
};

} // namespace ImmSocketService

#endif
