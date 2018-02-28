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

CPacketHandler::CPacketHandler (void)
{
	pthread_mutex_init (&mMutexGenId, NULL);

	pthread_mutexattr_t attrMutexWorker;
	pthread_mutexattr_init (&attrMutexWorker);
	pthread_mutexattr_settype (&attrMutexWorker, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&mMutexSyncRequestTable, &attrMutexWorker);
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

void CPacketHandler::removeSyncRequestTable (CMessage *pMsg)
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

CMessage *CPacketHandler::findSyncRequestMessage (const CMessageId::CId *pId)
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

void CPacketHandler::checkSyncRequestMessage (CMessage *pReplyMsg)
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
		"%s  id.num=[0x%02x] id.time=[%ld] type=[0x%02x] command=[0x%02x] size=[0x%02x]\n",
		__func__,
		pstPacket->id.num,
		ntohl(pstPacket->id.time),
		pstPacket->type,
		pstPacket->command,
		pstPacket->size
	);

	CMessageId::CId id;
	id.setNum (pstPacket->id.num);
	id.setTime (ntohl(pstPacket->id.time));
	CMessage msg(pSelf, &id, pstPacket->command, EN_OBJTYPE_NOTHING);

	int type = (int)(pstPacket->type & CMessage::MASK_MSG_TYPE);
	if (type == CMessage::MSG_TYPE_REQUEST) {
		// check sync
		int isSync = ((pstPacket->type & CMessage::MASK_IS_SYNC) >> 7);
		if (isSync == 1) {
			_ISS_LOG_I ("MSG_TYPE_REQUEST sync\n");
			msg.setSync();
		}

	} else if (type == CMessage::MSG_TYPE_REPLY) {
		// replyの場合のみ上位4bitの内下3bitに結果(OK/NG)が入っています
		int res = ((pstPacket->type & CMessage::MASK_REPLY_RESULT) >> 4);
		if (res == 1) {
			msg.setReplyResult (true);
		} else {
			msg.setReplyResult (false);
		}

		// check sync
		int isSync = ((pstPacket->type & CMessage::MASK_IS_SYNC) >> 7);
		if (isSync == 1) {
			_ISS_LOG_I ("MSG_TYPE_REPLY sync\n");
			msg.setSync();
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


	if (msgType == CMessage::MSG_TYPE_REQUEST) {

		pMsg->setObjtype (EN_OBJTYPE_REPLYABLE);

		onHandleRequest (pMsg);

	} else if (msgType == CMessage::MSG_TYPE_REPLY) {

		pMsg->setObjtype (EN_OBJTYPE_NOTHING);

		if (pMsg->isSync()) {
			checkSyncRequestMessage (pMsg);
		} else  {
			onHandleReply (pMsg);
		}

	} else if (msgType == CMessage::MSG_TYPE_NOTIFY) {

		pMsg->setObjtype (EN_OBJTYPE_NOTHING);

		onHandleNotify (pMsg);

	} else {
		_ISS_LOG_E ("%s  invalid packet (unknown type)\n", __func__);
	}

}

} // namespace ImmSocketService
