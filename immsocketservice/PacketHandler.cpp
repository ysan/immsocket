#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "Message.h"


namespace ImmSocketService {

CPacketHandler::CPacketHandler (void) :
	mId (0)
{
	pthread_mutex_init (&mMutexGenId, NULL);
	pthread_mutex_init (&mMutexSyncRequestTable, NULL);
}

CPacketHandler::~CPacketHandler (void)
{
	pthread_mutex_destroy (&mMutexGenId);
	pthread_mutex_destroy (&mMutexSyncRequestTable);
}

void CPacketHandler::addSyncRequestTable (CMessage *pMsg)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);


	if (!pMsg) {
		_ISS_LOG_E ("%s pMsg is null\n", __func__);
		return;
	}

	uint8_t id = pMsg->getId();

	SYNC_REQUEST_TABLE::iterator iter;
	iter = mSyncRequestTable.find (id);

	if (iter == mSyncRequestTable.end()) {
		// new
		mSyncRequestTable.insert (pair<uint8_t, CMessage*>(id, pMsg));

	} else {
//TODO
		// already use
		removeSyncRequestTable (pMsg);
		addSyncRequestTable (pMsg);
	}
}

void CPacketHandler::removeSyncRequestTable (CMessage *pMsg)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);


	if (!pMsg) {
		_ISS_LOG_E ("%s pMsg is null\n", __func__);
		return;
	}

	uint8_t id = pMsg->getId();

	SYNC_REQUEST_TABLE::iterator iter;
	iter = mSyncRequestTable.find (id);

	if (iter != mSyncRequestTable.end()) {
		mSyncRequestTable.erase (id);
	}
}

CMessage *CPacketHandler::findSyncRequestMessage (uint8_t id)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSyncRequestTable);


	CMessage *pRtn = NULL;

	SYNC_REQUEST_TABLE::iterator iter;
	iter = mSyncRequestTable.find (id);

	if (iter != mSyncRequestTable.end()) {
		pRtn = iter->second;
	}

	return pRtn;
}

void CPacketHandler::checkSyncRequestMessage (CMessage *pReplyMsg)
{
	if (!pReplyMsg) {
		_ISS_LOG_E ("%s pReplyMsg is null\n", __func__);
		return;
	}

	uint8_t id = pReplyMsg->getId();
	CMessage *pWaitMsg = findSyncRequestMessage (id);
	if (!pWaitMsg) {
		return ;
	}

	bool isOK = pReplyMsg->isReplyResultOK();
	pWaitMsg->sync()->setReplyResult (isOK);

	int replyDataSize = pReplyMsg->getDataSize();
	uint8_t* pReplyData = pReplyMsg->getData();
	if ((replyDataSize > 0) && (pReplyData)) {
		pWaitMsg->sync()->setData(pReplyData, replyDataSize);
	}

	pWaitMsg->sync()->condLock();
	pWaitMsg->sync()->condSignal();
	pWaitMsg->sync()->condUnlock();
}

uint8_t CPacketHandler::generateId (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexGenId);


	uint8_t rtn = 0;

	rtn = mId & 0xff;
	mId ++;

	return rtn;
}

void CPacketHandler::onHandleRequest (CMessage *pMsg)
{
	_ISS_LOG_I ("%s %s\n", __FILE__, __func__);
}

void CPacketHandler::onHandleReply (CMessage *pMsg)
{
	_ISS_LOG_I ("%s %s\n", __FILE__, __func__);
}

void CPacketHandler::onHandleNotify (CMessage *pMsg)
{
	_ISS_LOG_I ("%s %s\n", __FILE__, __func__);
}

void CPacketHandler::onSetup (CImmSocketClient *pSelf)
{
	if (pSelf) {
		mpClientInstance = pSelf;
	}

	mAsyncProcProxy.start();

}

void CPacketHandler::onTeardown (CImmSocketClient *pSelf)
{

	mAsyncProcProxy.syncStop();

}

void CPacketHandler::onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size)
{
	if ((!pPacket) || (size < (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("%s  invalid packet\n", __func__);
		return ;
	}


	ST_PACKET *pstPacket = (ST_PACKET*)pPacket;
	_ISS_LOG_N (
		"%s  id=[0x%02x] type=[0x%02x] command=[0x%02x] size=[0x%02x]\n",
		__func__,
		pstPacket->id,
		pstPacket->type,
		pstPacket->command,
		pstPacket->size
	);


	CMessage msg(pSelf, pstPacket->id, pstPacket->command, EN_OBJTYPE_NOTHING);

	int type = (int)(pstPacket->type & 0x0f);
	if (type == MSG_TYPE_REPLY) {
		// replyの場合のみ上位4bitに結果(OK/NG)が入っています
		if ((pstPacket->type & 0xf0) == 0) {
			msg.setReplyResult (true);
		} else {
			msg.setReplyResult (false);
		}
	}

	if (pstPacket->size > 0) {
		// data exist
		if (size != (int)sizeof(ST_PACKET) + (int)pstPacket->size) {
			_ISS_LOG_E ("%s  invalid packet (mismatch size)\n", __func__);
			return ;
		}

		uint8_t *pData = pPacket + sizeof(ST_PACKET);
		msg.setData (pData, pstPacket->size);

	} else {
		// data nothing
		if (size != (int)sizeof(ST_PACKET)) {
			_ISS_LOG_E ("%s  invalid packet (mismatch size)\n", __func__);
			return ;
		}
	}


//	handleMsg (&msg, type); // ProxyThreadでたたくようにしました
	ST_REQ_QUEUE stReq (&msg, type);
	mAsyncProcProxy.request (&stReq);

}

// ここはProxyThreadでたたかれます
// PROXY_THREAD_POOL_NUM が2以上の時は
// CPacketHandler::handleMsgはスレッドセーフな処理にする必要があります
// リエントラントなコード必須
void CPacketHandler::handleMsg (CMessage *pMsg, int msgType)
{
	if (!pMsg) {
		_ISS_LOG_E ("%s pMsg is null\n", __func__);
		return ;
	}


	switch (msgType) {
	case MSG_TYPE_REQUEST:
		pMsg->setObjtype (EN_OBJTYPE_REPLYABLE);
		onHandleRequest (pMsg);
		break;

	case MSG_TYPE_REPLY:
		pMsg->setObjtype (EN_OBJTYPE_NOTHING);
		onHandleReply (pMsg);

		checkSyncRequestMessage (pMsg);

		break;

	case MSG_TYPE_NOTIFY:
		pMsg->setObjtype (EN_OBJTYPE_NOTHING);
		onHandleNotify (pMsg);
		break;

	default:
		_ISS_LOG_E ("%s  invalid packet (unknown type)\n", __func__);
		break;
	}

}

} // namespace ImmSocketService
