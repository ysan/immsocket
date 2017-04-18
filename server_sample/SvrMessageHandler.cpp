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
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);


	if ((int)pMsg->getCommand() == 0x05) {

		int data = *((int*)pMsg->getData());
		_UTL_LOG_N ("######  %d  ######\n", data);

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyOK();
		delete pReplyMsg;
		pReplyMsg = NULL;

	} else if ((int)pMsg->getCommand() == 0x01) {

		uint8_t *pData = pMsg->getData();
		char tmp[pMsg->getDataSize() +1];
		memset (tmp, 0x00, sizeof(tmp));
		strncpy (tmp, (char*)pData, sizeof(tmp) -1);
		_UTL_LOG_N ("######  %s  ######\n", pData);

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyOK();
		delete pReplyMsg;
		pReplyMsg = NULL;

	} else if ((int)pMsg->getCommand() == 0x02) {

		sleep (1); // debug

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyOK((uint8_t*)"testreply", 9);
		delete pReplyMsg;
		pReplyMsg = NULL;

	} else {

		CMessage *pReplyMsg = new CMessage(pMsg);
		pReplyMsg->sendReplyNG();
		delete pReplyMsg;
		pReplyMsg = NULL;

	}

}

void CSvrMessageHandler::onHandleReply (CMessage *pMsg)
{
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);
}

void CSvrMessageHandler::onHandleNotify (CMessage *pMsg)
{
	_UTL_LOG_N ("%s %s\n", __FILE__, __PRETTY_FUNCTION__);
}
