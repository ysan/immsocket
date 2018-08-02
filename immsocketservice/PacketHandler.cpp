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


namespace ImmSocketService {

CPacketHandler::CPacketHandler (void)
	:mProxy (2)
{
}

CPacketHandler::~CPacketHandler (void)
{
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

	mProxy.start ();
}

void CPacketHandler::onTeardown (CImmSocketClient *pSelf)
{
	mProxy.syncStop ();
}

void CPacketHandler::onReceivePacket (CImmSocketClient *pSelf, uint8_t *pPacket, int size)
{
	if ((!pPacket) || (size < (int)sizeof(ST_PACKET))) {
		_ISS_LOG_E ("%s  invalid packet\n", __func__);
		return ;
	}


	ST_PACKET *pstPacket = (ST_PACKET*)pPacket;
	_ISS_LOG_N (
		"%s  id.num=[0x%02x] id.hash=[0x%08x] type=[0x%02x] command=[0x%02x] size=[0x%02x]\n",
		__func__,
		pstPacket->id.num,
		ntohl(pstPacket->id.hash),
		pstPacket->type,
		pstPacket->command,
		pstPacket->size
	);

	CMessageId::CId id;
	id.setNum (pstPacket->id.num);
	id.setHash (ntohl(pstPacket->id.hash));
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
	ST_PACKET_HANDLED *p = new ST_PACKET_HANDLED (&msg, type);
	CAsyncHandlerImpl<ST_PACKET_HANDLED*> *pImpl = new CAsyncHandlerImpl<ST_PACKET_HANDLED*> (this);
	pImpl->deletable();
	mProxy.request (p, pImpl);
}

// ここはProxyThreadでたたかれます
// handleMsg以下はスレッドセーフな処理にする必要があります
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
			mSyncRequestManager.checkSyncRequestMessage (pMsg);
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
