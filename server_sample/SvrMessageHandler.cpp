#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "Utils.h"
#include "ClientHandler.h"
#include "PacketHandler.h"
#include "Message.h"
#include "SvrMessageHandler.h"


CSvrMessageHandler::CSvrMessageHandler (void)
{
}

CSvrMessageHandler::~CSvrMessageHandler (void)
{
}

void CSvrMessageHandler::onHandleRequest (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);


	switch ((int)pMsg->getCommand()) {
	case 0x05: {
		int data = *((int*)pMsg->getData());
		_UTL_LOG_I ("received ->  [%d]\n", data);

		CMessage *pReplyMsg = new CMessage(pMsg);
		if (((int)pMsg->getId() % 10) != 0) { // debug
			pReplyMsg->sendReplyOK();
		} else {
			pReplyMsg->sendReplyNG();
		}
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	case 0x01: {
		char *pData = (char*)pMsg->getData();
		_UTL_LOG_I ("received ->  [%s]\n", pData);

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyOK();
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	case 0x02: {
		_UTL_LOG_I ("debug sleep 1s\n");
		sleep (1); // debug

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyOK((uint8_t*)"testreply", 9);
		delete pReplyMsg;
		pReplyMsg = NULL;

		} break;

	default: {
		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyNG();
		delete pReplyMsg;
		pReplyMsg = NULL;
		} break;
	}

}

void CSvrMessageHandler::onHandleReply (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
}

void CSvrMessageHandler::onHandleNotify (CMessage *pMsg)
{
	_UTL_LOG_I ("%s\n", __PRETTY_FUNCTION__);
}
