#ifndef _IMM_SCOKET_SERVICE_H_
#define _IMM_SCOKET_SERVICE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "Utils.h"
#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "RequestMessage.h"
#include "ReplyMessage.h"
#include "NotifyMessage.h"
#include "PacketHandler.h"
#include "ClientHandler.h"

using namespace std;


namespace ImmSocketService {

using namespace ImmSocket;


class CServer : public CImmSocketServer {
public:
	template <typename T>
	CServer (uint16_t port, CClientHandler<T> *pHandler) : CImmSocketServer (port, pHandler) {}
	template <typename T>
	CServer (const char* pPath, CClientHandler<T> *pHandler) : CImmSocketServer (pPath, pHandler) {}
	~CServer (void) {}
};

class CClient : public CImmSocketClient {
public:
	CClient (const char *pszIpAddr, uint16_t port, CPacketHandler *pHandler) : CImmSocketClient (pszIpAddr, port, pHandler) {}
	CClient (const char *pszPath, CPacketHandler *pHandler) : CImmSocketClient (pszPath, pHandler) {}
	~CClient (void) {}
};



} // namespace ImmSocketService

#endif
