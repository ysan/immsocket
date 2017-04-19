#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "Utils.h"
#include "ClientHandler.h"
#include "PacketHandler.h"
#include "Message.h"
#include "ClMessageHandler.h"


CClMessageHandler::CClMessageHandler (void)
{
}

CClMessageHandler::~CClMessageHandler (void)
{
}

void CClMessageHandler::onHandleRequest (CMessage *pMsg)
{
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);
}

void CClMessageHandler::onHandleReply (CMessage *pMsg)
{
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);


	_UTL_LOG_N ("id[%d] command[%d] %s\n", (int)pMsg->getId(), (int)pMsg->getCommand(), pMsg->isReplyResultOK() ? "REPLY_OK" : "REPLY_NG");

	if (pMsg->getDataSize() > 0) {
		_UTL_LOG_N ("replyData [%s]\n", (char*)(pMsg->getData()));
	}


}

void CClMessageHandler::onHandleNotify (CMessage *pMsg)
{
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);
}
