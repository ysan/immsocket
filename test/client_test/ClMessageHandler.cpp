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

void CClMessageHandler::onHandleRequest (CRequestMessage *pRequestMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I (
		"id.num[0x%02x] id.time[0x%08x] command[0x%02x]\n",
		(int)pRequestMsg->getId()->getNum(),
		pRequestMsg->getId()->getHash(),
		(int)pRequestMsg->getCommand()
	);

	if (pRequestMsg->getDataSize() > 0) {
		_UTL_LOG_I ("requestData [%s]\n", (char*)(pRequestMsg->getData()));
	}

}

void CClMessageHandler::onHandleReply (CReplyMessage *pReplyMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I (
		"id.num[0x%02x] id.hash[0x%08x] command[0x%02x] %s\n",
		(int)pReplyMsg->getId()->getNum(),
		pReplyMsg->getId()->getHash(),
		(int)pReplyMsg->getCommand(),
		pReplyMsg->isReplyResultOK() ? "reply_ok" : "reply_ng");

	if (pReplyMsg->getDataSize() > 0) {
		_UTL_LOG_I ("replyData [%dbytes] [%s]\n", pReplyMsg->getDataSize(), (char*)(pReplyMsg->getData()));
	}


}

void CClMessageHandler::onHandleNotify (CNotifyMessage *pNotifyMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	_UTL_LOG_I (
		"id.num[0x%02x] id.hash[0x%08x] command[0x%02x]\n",
		(int)pNotifyMsg->getId()->getNum(),
		pNotifyMsg->getId()->getHash(),
		(int)pNotifyMsg->getCommand()
	);

	if (pNotifyMsg->getDataSize() > 0) {
		_UTL_LOG_I ("notifyData [%s]\n", (char*)(pNotifyMsg->getData()));
	}


}
