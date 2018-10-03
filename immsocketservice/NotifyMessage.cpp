#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "NotifyMessage.h"
#include "SyncRequestManager.h"
#include "SenderManager.h"


namespace ImmSocketService {

CNotifyMessage::CNotifyMessage (CMessage *pMsg)
	:CMessage (pMsg)
{
}

CNotifyMessage::CNotifyMessage (CImmSocketClient *pClient)
	:CMessage (pClient, EN_OBJTYPE_REQUESTER)
{
}

CNotifyMessage::~CNotifyMessage (void)
{
}

bool CNotifyMessage::send (uint8_t command)
{
	setCommand (command);
	setData (NULL, 0, true);
	return sendNotify();
}

bool CNotifyMessage::send (uint8_t command, uint8_t *pData, int size)
{
	setCommand (command);
	setData (pData, size);
	return sendNotify();
}

} // namespace ImmSocketService
