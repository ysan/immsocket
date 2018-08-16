#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketService.h"

#include "ClMessageHandler.h"


CClMessageHandler::CClMessageHandler (int threadPoolNum)
	:CPacketHandler (threadPoolNum)
{
}

CClMessageHandler::~CClMessageHandler (void)
{
}

void CClMessageHandler::onHandleRequest (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I ("id.num[0x%02x] id.time[0x%08x] command[0x%02x]\n",
					(int)pMsg->getId()->getNum(), pMsg->getId()->getHash(), (int)pMsg->getCommand());

	if (pMsg->getDataSize() > 0) {
		_UTL_LOG_I ("requestData [%s]\n", (char*)(pMsg->getData()));
	}

}

void CClMessageHandler::onHandleReply (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I ("id.num[0x%02x] id.hash[0x%08x] command[0x%02x] %s\n",
					(int)pMsg->getId()->getNum(), pMsg->getId()->getHash(), (int)pMsg->getCommand(),
					pMsg->isReplyResultOK() ? "reply_ok" : "reply_ng");

	if (pMsg->getDataSize() > 0) {
		_UTL_LOG_I ("replyData [%s]\n", (char*)(pMsg->getData()));
	}


}

void CClMessageHandler::onHandleNotify (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I ("id.num[0x%02x] id.hash[0x%08x] command[0x%02x]\n",
					(int)pMsg->getId()->getNum(), pMsg->getId()->getHash(), (int)pMsg->getCommand());

	if (pMsg->getDataSize() > 0) {
		_UTL_LOG_I ("notifyData [%s]\n", (char*)(pMsg->getData()));
	}


}
