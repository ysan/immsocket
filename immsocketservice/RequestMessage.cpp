#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "RequestMessage.h"
#include "SyncRequestManager.h"
#include "SenderManager.h"


namespace ImmSocketService {

const int CRequestMessage::CSyncException::INTERNAL_ERROR   = 0;
const int CRequestMessage::CSyncException::TIMEOUT          = 1;
const int CRequestMessage::CSyncException::SIGNAL_INTERRUPT = 2;
const int CRequestMessage::CSyncException::UNEXPECTED_ERROR = 3;


CRequestMessage::CRequestMessage (CMessage *pMsg)
	:CMessage (pMsg)
{
}

CRequestMessage::CRequestMessage (CImmSocketClient *pClient)
	:CMessage (pClient, EN_OBJTYPE_REQUESTER)
{
}

CRequestMessage::~CRequestMessage (void)
{
}

int CRequestMessage::sendSync (uint8_t command, uint32_t nTimeoutMsec) throw (CSyncException)
{
	setCommand (command);
	setData (NULL, 0, true);
	int rtn = sendRequestSync (nTimeoutMsec);
	switch (rtn) {
	case 0:
		// success
		break;
	case -1:
		throw CSyncException ("exception: internal error", CSyncException::INTERNAL_ERROR);
		break;
	case ETIMEDOUT:
		throw CSyncException ("exception: timeout", CSyncException::TIMEOUT);
		break;
	case EINTR:
		throw CSyncException ("exception: signal interrupt", CSyncException::SIGNAL_INTERRUPT);
		break;
	default:
		throw CSyncException ("exception: unexpected error", CSyncException::UNEXPECTED_ERROR);
		break;
	}

	return rtn;
}

int CRequestMessage::sendSync (uint8_t command, uint8_t *pData, int size, uint32_t nTimeoutMsec) throw (CSyncException)
{
	setCommand (command);
	setData (pData, size);
	int rtn = sendRequestSync (nTimeoutMsec);
	switch (rtn) {
	case 0:
		// success
		break;
	case -1:
		throw CSyncException ("exception: internal error", CSyncException::INTERNAL_ERROR);
		break;
	case ETIMEDOUT:
		throw CSyncException ("exception: timeout", CSyncException::TIMEOUT);
		break;
	case EINTR:
		throw CSyncException ("exception: signal interrupt", CSyncException::SIGNAL_INTERRUPT);
		break;
	default:
		throw CSyncException ("exception: unexpected error", CSyncException::UNEXPECTED_ERROR);
		break;
	}

	return rtn;
}

bool CRequestMessage::sendAsync (CMessageId::CId *pId, uint8_t command)
{
	setCommand (command);
	setData (NULL, 0, true);
	return sendRequestAsync (pId);
}

bool CRequestMessage::sendAsync (CMessageId::CId *pId, uint8_t command, uint8_t *pData, int size)
{
	setCommand (command);
	setData (pData, size);
	return sendRequestAsync (pId);
}

} // namespace ImmSocketService
