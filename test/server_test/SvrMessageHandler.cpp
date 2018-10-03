#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "SvrMessageHandler.h"


CSvrMessageHandler::CSvrMessageHandler (int threadPoolNum)
	:CPacketHandler (threadPoolNum)
{
}

CSvrMessageHandler::~CSvrMessageHandler (void)
{
}

void CSvrMessageHandler::onHandleRequest (CRequestMessage *pRequestMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	switch ((int)pRequestMsg->getCommand()) {
	case 0x05: {
		int data = *((int*)pRequestMsg->getData());
		_UTL_LOG_I ("received ->  [%d]\n", data);

		CReplyMessage *pReplyMsg = new CReplyMessage (pRequestMsg);
		if (((int)pRequestMsg->getId()->getNum() % 10) != 0) { // debug
			pReplyMsg->sendOK();
		} else {
			pReplyMsg->sendNG();
		}
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	case 0x01: {
		char *pData = (char*)pRequestMsg->getData();
		_UTL_LOG_I ("received ->  [%s]\n", pData);

		CReplyMessage *pReplyMsg = new CReplyMessage(pRequestMsg);
		pReplyMsg->sendOK();
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	case 0x02: {
		_UTL_LOG_I ("debug sleep 1s\n");
		sleep (1); // debug

		CReplyMessage *pReplyMsg = new CReplyMessage(pRequestMsg);
		pReplyMsg->sendOK((uint8_t*)"testreply", 9);
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	default: {
		CReplyMessage *pReplyMsg = new CReplyMessage(pRequestMsg);
		pReplyMsg->sendNG();
		delete pReplyMsg;
		pReplyMsg = NULL;
		} break;
	}

}

void CSvrMessageHandler::onHandleReply (CReplyMessage *pReplyMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
}

void CSvrMessageHandler::onHandleNotify (CNotifyMessage *pNotifyMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
}
