#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "LocalSocketServer.h"
#include "LocalSocketClient.h"
#include "Utils.h"
#include "ClientHandler.h"
#include "PacketHandler.h"
#include "Message.h"


namespace LocalSocketService {

CMessage::CSync::CSync (void) :
	mCommand (0),
	mIsReplyResultOK (false),
	mDataSize (0)
{
	pthread_cond_init (&mCond, NULL);
	pthread_mutex_init (&mMutexCond, NULL);
	memset (mEntityData, 0x00, sizeof(mEntityData));
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

void CMessage::CSync::condWait (void)
{
	pthread_cond_wait (&mCond, &mMutexCond);
}

void CMessage::CSync::condSignal (void)
{
	pthread_cond_signal (&mCond);
}

uint8_t CMessage::CSync::getCommand (void)
{
	return mCommand;
}

void CMessage::CSync::setCommand (uint8_t command)
{
	mCommand = command;
}

uint8_t* CMessage::CSync::getData (void)
{
//	return mpData;
	return &mEntityData[0];
}

void CMessage::CSync::setData (uint8_t *pData, int size, bool isClear)
{
	if (isClear) {
		memset (mEntityData, 0x00, sizeof(mEntityData));
		mDataSize = 0;
		return;
	}

	if ((!pData) || (size == 0)) {
		return;
	}

//	mpData= pData;
	int cpsize = size > (0xff - (int)sizeof(ST_PACKET)) ? (0xff - (int)sizeof(ST_PACKET)) : size;
	memset (mEntityData, 0x00, sizeof(mEntityData));
	memcpy (mEntityData, pData, cpsize);
	mDataSize = cpsize;
}

int CMessage::CSync::getDataSize (void)
{
	return mDataSize;
}


CMessage::CMessage (void) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	mObjtype = EN_OBJTYPE_NOTHING;
}

// for create reply message
CMessage::CMessage (CMessage *pRequestMsg) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	if (pRequestMsg) {
		mId = pRequestMsg->getId();
		mCommand = pRequestMsg->getCommand();
		mpClientInstance = pRequestMsg->getClientInstance();

		if (pRequestMsg->getObjtype() == EN_OBJTYPE_REPLYABLE) { 
			mObjtype = EN_OBJTYPE_REPLYER;
		} else {
			_UTL_LOG_W ("not EN_OBJTYPE_REPLYABLE\n");
		}

	} else {
		_UTL_LOG_E ("pRequestMsg is null\n");
	}

}

CMessage::CMessage (CLocalSocketClient *pClient) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

CMessage::CMessage (CLocalSocketClient *pClient, uint8_t id) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	mId = id;

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

CMessage::CMessage (CLocalSocketClient *pClient, uint8_t id, uint8_t command) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	mId = id;
	mCommand = command;

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

CMessage::CMessage (CLocalSocketClient *pClient, uint8_t id, uint8_t command, EN_OBJTYPE enType) :
	mId (0),
	mCommand (0),
	mIsReplyResultOK (false),
	mpClientInstance (NULL),
//	mpData (NULL),
	mDataSize (0),
	mObjtype (EN_OBJTYPE_REQUESTER)
{
	if (pClient) {
		mpClientInstance = pClient;
	}

	mId = id;
	mCommand = command;
	mObjtype = enType;

	memset (mEntityData, 0x00, sizeof(mEntityData));
}

// copy constructor
//CMessage::CMessage (const CMessage &obj)
//{
//}

CMessage::~CMessage (void)
{
}

uint8_t CMessage::getCommand (void)
{
	return mCommand;
}

void CMessage::setCommand (uint8_t command)
{
	mCommand = command;
}

uint8_t* CMessage::getData (void)
{
//	return mpData;
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

//	mpData= pData;
	int cpsize = size > (0xff - (int)sizeof(ST_PACKET)) ? (0xff - (int)sizeof(ST_PACKET)) : size;
	memset (mEntityData, 0x00, sizeof(mEntityData));
	memcpy (mEntityData, pData, cpsize);
	mDataSize = cpsize;
}

int CMessage::getDataSize (void)
{
	return mDataSize;
}

uint8_t CMessage::getId (void)
{
	return mId;
}

uint8_t CMessage::genId (void)
{
	CPacketHandler *pPacketHandler = (CPacketHandler*)mpClientInstance->getPacketHandler();
	if (!pPacketHandler) {
		_UTL_LOG_E ("PacketHandler is null\n");
		return 0x00;
	}

	uint8_t id = 0x00;
	id = pPacketHandler->genId();
	mId = id;

	return id;
}

bool CMessage::sendRequestSync (void)
{
	if (!mpClientInstance) {
		_UTL_LOG_E ("mpClientInstance is null\n");
		return false;
	}

	CPacketHandler *pPacketHandler = (CPacketHandler*)mpClientInstance->getPacketHandler();
	if (!pPacketHandler) {
		_UTL_LOG_E ("PacketHandler is null\n");
		return false;
	}

	// lock
	sync()->condLock();


	// add 
	uint8_t id = genId(); // need new id
	pPacketHandler->addSyncRequestTable (this);

	if (!sendRequest (id)) {
		// remove
		pPacketHandler->removeSyncRequestTable (this);
		// unlock
		sync()->condUnlock();
		// send error
		return false;
	}

	// condition wait
	sync()->condWait();

	// unlock
	sync()->condUnlock();


	//### syncronized reply message ###


	// remove
	pPacketHandler->removeSyncRequestTable (this);

	return true;
}

bool CMessage::sendRequestSync (uint8_t command)
{
	setCommand (command);
	setData (NULL, 0, true);
	return sendRequestSync();
}

bool CMessage::sendRequestSync (uint8_t command, uint8_t *pData, int size)
{
	setCommand (command);
	setData (pData, size);
	return sendRequestSync();
}

bool CMessage::sendRequestAsync (uint8_t id)
{
	return sendRequest (id);
}

bool CMessage::sendRequestAsync (uint8_t id, uint8_t command)
{
	setCommand (command);
	setData (NULL, 0, true);
	return sendRequestAsync (id);
}

bool CMessage::sendRequestAsync (uint8_t id, uint8_t command, uint8_t *pData, int size)
{
	setCommand (command);
	setData (pData, size);
	return sendRequestAsync (id);
}

bool CMessage::sendRequest (uint8_t id)
{
	if (mObjtype != EN_OBJTYPE_REQUESTER) {
		_UTL_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_UTL_LOG_E ("mpClientInstance is null\n");
		return false;
	}
//	if ((mDataSize == 0) && mpData) {
//		_UTL_LOG_E ("(mDataSize == 0) && mpData --> invalid data\n");
//		return false;
//	}
//	if ((mDataSize > 0) && !mpData) {
//		_UTL_LOG_E ("(mDataSize > 0) && !mpData --> invalid data\n");
//		return false;
//	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_UTL_LOG_E ("data size over\n");
		return false;
	}

	CPacketHandler *pPacketHandler = (CPacketHandler*)mpClientInstance->getPacketHandler();
	if (!pPacketHandler) {
		_UTL_LOG_E ("PacketHandler is null\n");
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
	if (!setPacket (id, MSG_TYPE_REQUEST, mCommand, buff, totalsize)) {
		return false;
	}


	return mpClientInstance->sendToServer (buff, totalsize);
}

bool CMessage::sendReplyOK (void)
{
	setReplyResult (true);
	setData (NULL, 0, true);
	return sendReply ();
}

bool CMessage::sendReplyOK (uint8_t *pReplyData, int size)
{
	setReplyResult (true);
	setData (pReplyData, size);
	return sendReply ();
}

bool CMessage::sendReplyNG (void)
{
	setReplyResult (false);
	setData (NULL, 0, true);
	return sendReply ();
}

bool CMessage::sendReplyNG (uint8_t *pReplyData, int size)
{
	setReplyResult (false);
	setData (pReplyData, size);
	return sendReply ();
}

bool CMessage::sendReply (void)
{
	if (mObjtype != EN_OBJTYPE_REPLYER) {
		_UTL_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_UTL_LOG_E ("mpClientInstance is null\n");
		return false;
	}
//	if ((mDataSize == 0) && mpData) {
//		_UTL_LOG_E ("(mDataSize == 0) && mpData --> invalid data\n");
//		return false;
//	}
//	if ((mDataSize > 0) && !mpData) {
//		_UTL_LOG_E ("(mDataSize > 0) && !mpData --> invalid data\n");
//		return false;
//	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_UTL_LOG_E ("data size over\n");
		return false;
	}


	// reply結果をtypeの上位4bitに埋め込みます
	uint8_t type = 0x00;
	if (mIsReplyResultOK) {
		type = MSG_TYPE_REPLY;
	} else {
		type = MSG_TYPE_REPLY | ((0x01 << 4) & 0xff);
	}


	int totalsize = 0;
	if (mDataSize == 0) {
		totalsize = (int)sizeof(ST_PACKET);
	} else {
		totalsize = (int)sizeof(ST_PACKET) + (int)mDataSize;
	}
	uint8_t buff [totalsize];
	memset (buff, 0x00, sizeof(buff));
	if (!setPacket (mId, type, mCommand, buff, totalsize)) {
		return false;
	}

//TODO replyは一度限り
	mObjtype = EN_OBJTYPE_NOTHING;

	return mpClientInstance->sendToServer (buff, totalsize);
}

bool CMessage::sendNotify (void)
{
	if (mObjtype != EN_OBJTYPE_REQUESTER) {
		_UTL_LOG_E ("mObjType:[%d]\n", mObjtype);
		return false;
	}
	if (!mpClientInstance) {
		_UTL_LOG_E ("mpClientInstance is null\n");
		return false;
	}
//	if ((mDataSize == 0) && mpData) {
//		_UTL_LOG_E ("(mDataSize == 0) && mpData --> invalid data\n");
//		return false;
//	}
//	if ((mDataSize > 0) && !mpData) {
//		_UTL_LOG_E ("(mDataSize > 0) && !mpData --> invalid data\n");
//		return false;
//	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_UTL_LOG_E ("data size over\n");
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
	if (!setPacket (0x00, MSG_TYPE_NOTIFY, mCommand, buff, totalsize)) {
		return false;
	}


	return mpClientInstance->sendToServer (buff, totalsize);
}

bool CMessage::setPacket (uint8_t id, uint8_t type, uint8_t command, uint8_t *pOut, int outsize)
{
	if ((!pOut) || (outsize <= 0)) {
		return false;
	}

//	if ((mDataSize == 0) && mpData) {
//		_UTL_LOG_E ("(mDataSize == 0) && mpData --> invalid data\n");
//		return false;
//	}
//	if ((mDataSize > 0) && !mpData) {
//		_UTL_LOG_E ("(mDataSize > 0) && !mpData --> invalid data\n");
//		return false;
//	}
	if (mDataSize > (0xff - (int)sizeof(ST_PACKET))) {
		_UTL_LOG_E ("data size over\n");
		return false;
	}


	ST_PACKET stPacket;
	memset (&stPacket, 0x00, sizeof(stPacket));
	stPacket.id = id ;
	stPacket.type = type;
	stPacket.command = command;
	stPacket.size = mDataSize;

	memset (pOut, 0x00, outsize);
	if (mDataSize == 0) {
		memcpy (pOut, &stPacket, outsize);
	} else {
		memcpy (pOut, &stPacket, sizeof(stPacket));
//		memcpy (pOut+sizeof(stPacket), mpData, mDataSize);
		memcpy (pOut+sizeof(stPacket), mEntityData, mDataSize);
	}

	return true;
}

CLocalSocketClient *CMessage::getClientInstance (void)
{
	return mpClientInstance;
}

EN_OBJTYPE CMessage::getObjtype (void)
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

} // namespace LocalSocketService
