#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "SyncRequestManager.h"


namespace ImmSocketService {

CSyncRequestManager::CSyncRequestManager (void)
{
	pthread_mutexattr_t attrMutexWorker;
	pthread_mutexattr_init (&attrMutexWorker);
	pthread_mutexattr_settype (&attrMutexWorker, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&mMutexSyncRequestTable, &attrMutexWorker);	
}

CSyncRequestManager::~CSyncRequestManager (void)
{
	pthread_mutex_destroy (&mMutexSyncRequestTable);
}

void CSyncRequestManager::addSyncRequestTable (CMessage *pMsg)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);


	if (!pMsg) {
		_ISS_LOG_E ("%s pMsg is null\n", __func__);
		return;
	}

	SYNC_REQUEST_TABLE::iterator iter = mSyncRequestTable.begin();
	while (iter != mSyncRequestTable.end()) {
		if (*((*iter)->getId()) == *(pMsg->getId())) {
			break;
		}

		++ iter;
	}

	if (iter == mSyncRequestTable.end()) {
		mSyncRequestTable.push_back (pMsg);

	} else {
		// already use // bug
		_ISS_LOG_E ("BUG: CMessageId::CId is already use.\n");
	}
}

void CSyncRequestManager::removeSyncRequestTable (CMessage *pMsg)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);


	if (!pMsg) {
		_ISS_LOG_E ("%s pMsg is null\n", __func__);
		return;
	}

	SYNC_REQUEST_TABLE::iterator iter = mSyncRequestTable.begin();
	while (iter != mSyncRequestTable.end()) {
		if (*((*iter)->getId()) == *(pMsg->getId())) {
			break;
		}

		++ iter;
	}

	if (iter != mSyncRequestTable.end()) {
		mSyncRequestTable.erase (iter);
	}
}

CMessage *CSyncRequestManager::findSyncRequestMessage (const CMessageId::CId *pId)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);

	if (!pId) {
		_ISS_LOG_E ("%s pId is null\n", __func__);
		return NULL;
	}

	CMessage *pRtn = NULL;

	SYNC_REQUEST_TABLE::iterator iter = mSyncRequestTable.begin();
	while (iter != mSyncRequestTable.end()) {
		if (*((*iter)->getId()) == *pId) {
			break;
		}

		++ iter;
	}

	if (iter != mSyncRequestTable.end()) {
		pRtn = *iter;
	}

	return pRtn;
}

void CSyncRequestManager::checkSyncRequestMessage (CMessage *pReplyMsg)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);

	if (!pReplyMsg) {
		_ISS_LOG_E ("%s pReplyMsg is null\n", __func__);
		return;
	}

	CMessageId::CId *pId = pReplyMsg->getId();
	CMessage *pWaitMsg = findSyncRequestMessage (pId);
	if (!pWaitMsg) {
		_ISS_LOG_E ("%s meybe timeout droped.\n", __func__);
		return ;
	}

	bool isOK = pReplyMsg->isReplyResultOK();
	pWaitMsg->setReplyResult (isOK);

	int replyDataSize = pReplyMsg->getDataSize();
	uint8_t* pReplyData = pReplyMsg->getData();
	if ((replyDataSize > 0) && (pReplyData)) {
		pWaitMsg->setData(pReplyData, replyDataSize);
	}

	pWaitMsg->sync()->condLock();
	pWaitMsg->sync()->condSignal();
	pWaitMsg->sync()->condUnlock();

	return;
}


} // namespace ImmSocketService
