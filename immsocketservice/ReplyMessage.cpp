#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "ReplyMessage.h"
#include "SyncRequestManager.h"
#include "SenderManager.h"


namespace ImmSocketService {

CReplyMessage::CReplyMessage (CMessage *pMsg)
	:CMessage (pMsg)
{
}

CReplyMessage::CReplyMessage (CRequestMessage *pRequestMsg)
{
	if (pRequestMsg) {
		setRequest2Reply (pRequestMsg);
	}
}

CReplyMessage::~CReplyMessage (void)
{
}

bool CReplyMessage::sendOK (void)
{
	setReplyResult (true);
	setData (NULL, 0, true);
	return sendReply ();
}

bool CReplyMessage::sendOK (uint8_t *pReplyData, int size)
{
	setReplyResult (true);
	setData (pReplyData, size);
	return sendReply ();
}

bool CReplyMessage::sendNG (void)
{
	setReplyResult (false);
	setData (NULL, 0, true);
	return sendReply ();
}

bool CReplyMessage::sendNG (uint8_t *pReplyData, int size)
{
	setReplyResult (false);
	setData (pReplyData, size);
	return sendReply ();
}

} // namespace ImmSocketService
