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
#include "SyncRequestManager.h"
#include "SenderManager.h"


namespace ImmSocketService {

const uint8_t CMessage::MSG_TYPE_REQUEST = 0x00;
const uint8_t CMessage::MSG_TYPE_REPLY   = 0x01;
const uint8_t CMessage::MSG_TYPE_NOTIFY  = 0x02;

const uint8_t CMessage::MASK_IS_SYNC      = 0x80;
const uint8_t CMessage::MASK_REPLY_RESULT = 0x70;
const uint8_t CMessage::MASK_MSG_TYPE     = 0x0f;

const uint32_t CMessage::REQUEST_TIMEOUT_MAX   = 0x05265C00; // 24時間 msec
const uint32_t CMessage::REQUEST_TIMEOUT_FEVER = 0xffffffff;


CMessage::CSync::CSync (void)
{
	pthread_cond_init (&mCond, NULL);
	pthread_mutex_init (&mMutexCond, NULL);
}

CMessage::CSync::~CSync (void)
{
	pthread_cond_destroy (&mCond);
	pthread_mutex_destroy (&mMutexCond);
}

void CMessage::CSync::condLock (void)
{
	pthread_mutex_lock (&mMutexCond);
}

void CMessage::CSync::condUnlock (void)
{
	pthread_mutex_unlock (&mMutexCond);
}

int CMessage::CSync::condWait (uint32_t nTimeoutMsec)
{
	// pthread_cond_wait
	if (nTimeoutMsec == REQUEST_TIMEOUT_FEVER) {
		pthread_cond_wait (&mCond, &mMutexCond);
		return 0;
	}


	// pthread_cond_timedwait

	if (nTimeoutMsec < 0) {
		nTimeoutMsec = 0;
	} else if (nTimeoutMsec > REQUEST_TIMEOUT_MAX) {
		nTimeoutMsec = REQUEST_TIMEOUT_MAX;
	}

	struct timespec stTimeout = {0};
	struct timeval stNowTimeval = {0};

	CUtils::getTimeOfDay (&stNowTimeval);

	time_t after_decpoint = (stNowTimeval.tv_usec * 1000 + nTimeoutMsec * 1000000) % 1000000000;
	long adv = (stNowTimeval.tv_usec * 1000 + nTimeoutMsec * 1000000) / 1000000000;

	stTimeout.tv_sec = stNowTimeval.tv_sec + adv;
	stTimeout.tv_nsec = after_decpoint;

	int nRtn = pthread_cond_timedwait (&mCond, &mMutexCond, &stTimeout);
	switch (nRtn) {
	case 0:
	case ETIMEDOUT:
	case EINTR:
		break;
	default:
		_ISS_LOG_E ("BUG: pthread_cond_timedwait() => unexpected return value [%d]\n", nRtn);
		break;
	}

	return nRtn;
}

void CMessage::CSync::condSignal (void)
{
	pthread_cond_signal (&mCond);
}


CMessage::CMessage (void)
	:mCommand (0)
	,mIsReplyResultOK (false)
	,mpClientInstance (NULL)
	,mDataSize (0)
	,mObjtype (EN_OBJTYPE_REQUESTER)
	,mIsSync (false)
{
	mObjtype = EN_OBJTYPE_NOTHING;
}

CMessage::CMessage (CImmSocketClient *pClient, EN_OBJTYPE enType)
	:mCommand (0)
	,mIsReplyResultOK (false)
	,mpClientInstance (NULL)
	,mDataSize (0)
	,mObjtype (EN_OBJTYPE_REQUESTER)
	,mIsSync (false)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	mObjtype = enType;

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

CMessage::CMessage (CMessage *pMsg)
	:mCommand (0)
	,mIsReplyResultOK (false)
	,mpClientInstance (NULL)
	,mDataSize (0)
	,mObjtype (EN_OBJTYPE_REQUESTER)
	,mIsSync (false)
{
	if (pMsg) {
		*this = *pMsg;
	}
}

CMessage::CMessage (CImmSocketClient *pClient, CMessageId::CId *pId, uint8_t command, EN_OBJTYPE enType)
	:mCommand (0)
	,mIsReplyResultOK (false)
	,mpClientInstance (NULL)
	,mDataSize (0)
	,mObjtype (EN_OBJTYPE_REQUESTER)
	,mIsSync (false)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	if (pId) {
		mId = *pId;
	}

	mCommand = command;
	mObjtype = enType;

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

// copy ctor
//CMessage::CMessage (const CMessage &obj)
//{
//}

CMessage::~CMessage (void)
{
}

uint8_t CMessage::getCommand (void) const
{
	return mCommand;
}

void CMessage::setCommand (uint8_t command)
{
	mCommand = command;
}

uint8_t* CMessage::getData (void)
{
	return &mEntityData[0];
}

void CMessage::setData (uint8_t *pData, int size, bool isClear)
{
	if (isClear) {
		memset (mEntityData, 0x00, sizeof(mEntityData));
		mDataSize = 0;
		return;
	}

	if ((!pData) || (size <= 0)) {
		return;
	}

	int cpsize = 0;
	if (size > (0xff - (int)sizeof(ST_PACKET))) {
		cpsize = (0xff - (int)sizeof(ST_PACKET));
		_ISS_LOG_W ("send data was truncated. [max %d bytes]\n", cpsize);
	} else {
		cpsize = size;
	}
	memset (mEntityData, 0x00, sizeof(mEntityData));
	memcpy (mEntityData, pData, cpsize);
	mDataSize = cpsize;
}

int CMessage::getDataSize (void) const
{
	return mDataSize;
}

void CMessage::setReplyResult (bool isReplyResultOK)
{
	mIsReplyResultOK = isReplyResultOK;
}

bool CMessage::isReplyResultOK (void) const
{
	return mIsReplyResultOK;
}

CMessageId::CId *CMessage::getId (void)
{
	return &mId;
}

CMessageId::CId CMessage::generateId (void)
{
	CMessageId::CId id = CMessageId::getInstance()->generateId();
	mId = id;

	return id;
}

int CMessage::sendRequestSync (uint32_t nTimeoutMsec)
{
	if (!mpClientInstance) {
		_ISS_LOG_E ("mpClientInstance is null\n");
		return -1;
	}

	CPacketHandler *pPacketHandler = (CPacketHandler*)mpClientInstance->getPacketHandler();
	if (!pPacketHandler) {
		_ISS_LOG_E ("PacketHandler is null\n");
		return -1;
	}

	// lock
	sync()->condLock();


	// add 
	CMessageId::CId id = generateId(); // need new id
	pPacketHandler->getSyncRequestManager()->addSyncRequestTable (this);

	if (!sendRequest (&id, true)) {
		// remove
		pPacketHandler->getSyncRequestManager()->removeSyncRequestTable (this);
		// unlock
		sync()->condUnlock();
		// send error
		return -1;
	}

	// condition wait
	int nRtn = sync()->condWait (nTimeoutMsec);

	// unlock
	sync()->condUnlock();


	//### syncronized reply message ###


	// remove
	pPacketHandler->getSyncRequestManager()->removeSyncRequestTable (this);

	return nRtn;
}

bool CMessage::sendRequestAsync (CMessageId::CId *pId)
{
	return sendRequest (pId);
}

bool CMessage::sendRequest (CMessageId::CId *pId, bool isSync)
{
	if (!pId) {
		_ISS_LOG_E ("pId is null.\n");
		return false;
	}
	if (mObjtype != EN_OBJTYPE_REQUESTER) {
		_ISS_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_ISS_LOG_E ("mpClientInstance is null\n");
		return false;
	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("data size over\n");
		return false;
	}


	// 同期の場合 最上位bitを立てる
	uint8_t type = MSG_TYPE_REQUEST;
	if (isSync) {
		type |= MASK_IS_SYNC;
	}

	int totalsize = 0;
	if (mDataSize == 0) {
		totalsize = (int)sizeof(ST_PACKET);
	} else {
		totalsize = (int)sizeof(ST_PACKET) + (int)mDataSize;
	}
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	if (!setPacket (pId, type, mCommand, buff, totalsize)) {
		return false;
	}


	_sender_handle_info *pinfo = new _sender_handle_info (buff, totalsize);
	CSenderManager::getInstance()->getAsyncHandler()->setClient(mpClientInstance);
	CSenderManager::getInstance()->getProxy()->request (pinfo);

// ProxyThreadでたたくようにしました
//	return mpClientInstance->sendToConnection (buff, totalsize);
	return true;
}

bool CMessage::sendReply (void)
{
	if (mObjtype != EN_OBJTYPE_REPLYER) {
		_ISS_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_ISS_LOG_E ("mpClientInstance is null\n");
		return false;
	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("data size over\n");
		return false;
	}


	// reply結果をtypeの上位4bitの内3bitに埋め込み 実際には1bit分
	uint8_t type = MSG_TYPE_REPLY;
	if (isReplyResultOK()) {
		type |= 0x10;
	}

	// 同期の場合 最上位bitを立てる
	if (isSync()) {
		type |= MASK_IS_SYNC;
	}

	int totalsize = 0;
	if (mDataSize == 0) {
		totalsize = (int)sizeof(ST_PACKET);
	} else {
		totalsize = (int)sizeof(ST_PACKET) + (int)mDataSize;
	}
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	if (!setPacket (&mId, type, mCommand, buff, totalsize)) {
		return false;
	}


//TODO replyは一度限り
	mObjtype = EN_OBJTYPE_NOTHING;


	_sender_handle_info *pinfo = new _sender_handle_info (buff, totalsize);
	CSenderManager::getInstance()->getAsyncHandler()->setClient(mpClientInstance);
	CSenderManager::getInstance()->getProxy()->request (pinfo);

// ProxyThreadでたたくようにしました
//	return mpClientInstance->sendToConnection (buff, totalsize);
	return true;
}

bool CMessage::sendNotify (void)
{
	if (mObjtype != EN_OBJTYPE_REQUESTER) {
		_ISS_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_ISS_LOG_E ("mpClientInstance is null\n");
		return false;
	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("data size over\n");
		return false;
	}


	int totalsize = 0;
	if (mDataSize == 0) {
		totalsize = (int)sizeof(ST_PACKET);
	} else {
		totalsize = (int)sizeof(ST_PACKET) + (int)mDataSize;
	}
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	CMessageId::CId id; // dummy
	if (!setPacket (&id, MSG_TYPE_NOTIFY, mCommand, buff, totalsize)) {
		return false;
	}


	_sender_handle_info *pinfo = new _sender_handle_info (buff, totalsize);
	CSenderManager::getInstance()->getAsyncHandler()->setClient(mpClientInstance);
	CSenderManager::getInstance()->getProxy()->request (pinfo);

// ProxyThreadでたたくようにしました
//	return mpClientInstance->sendToConnection (buff, totalsize);
	return true;
}

bool CMessage::setPacket (CMessageId::CId *pId, uint8_t type, uint8_t command, uint8_t *pOut, int outsize)
{
	if (!pId) {
		return false;
	}

	if ((!pOut) || (outsize <= 0)) {
		return false;
	}

	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("data size over\n");
		return false;
	}


	ST_PACKET stPacket;
	memset (&stPacket, 0x00, sizeof(stPacket));
	stPacket.id.num = pId->getNum();
	stPacket.id.hash = htonl(pId->getHash());
	stPacket.type = type;
	stPacket.command = command;
	stPacket.size = mDataSize;

	memset (pOut, 0x00, outsize);
	if (mDataSize == 0) {
		memcpy (pOut, &stPacket, outsize);
	} else {
		memcpy (pOut, &stPacket, sizeof(stPacket));
		memcpy (pOut+sizeof(stPacket), mEntityData, mDataSize);
	}

	return true;
}

CImmSocketClient *CMessage::getClientInstance (void)
{
	return mpClientInstance;
}

EN_OBJTYPE CMessage::getObjtype (void) const
{
	return mObjtype;
}

void CMessage::setObjtype (EN_OBJTYPE type)
{
	if ((type < EN_OBJTYPE_REQUESTER) || (type >= EN_OBJTYPE_NOTHING)) {
		mObjtype = EN_OBJTYPE_NOTHING;
	} else {
		mObjtype = type;
	}
}

bool CMessage::isSync (void) const
{
	return mIsSync;
}

void CMessage::setSync (void)
{
	mIsSync = true;
}

void CMessage::setRequest2Reply (CMessage * pRequestMsg)
{
	if (pRequestMsg) {
		mId = *(pRequestMsg->getId());
		mCommand = pRequestMsg->getCommand();
		mpClientInstance = pRequestMsg->getClientInstance();
		mIsSync = pRequestMsg->isSync();

		if (pRequestMsg->getObjtype() == EN_OBJTYPE_REPLYABLE) {
			mObjtype = EN_OBJTYPE_REPLYER;
		} else {
			_ISS_LOG_E ("not EN_OBJTYPE_REPLYABLE\n");
			mObjtype = EN_OBJTYPE_NOTHING;
		}

	} else {
		_ISS_LOG_E ("pRequestMsg is null\n");
		mObjtype = EN_OBJTYPE_NOTHING;
	}
}

} // namespace ImmSocketService
