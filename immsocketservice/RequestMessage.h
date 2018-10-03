#ifndef _REQUEST_MESSAGE_H_
#define _REQUEST_MESSAGE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <arpa/inet.h>

#include <stdexcept>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Common.h"
#include "Message.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {


class CRequestMessage : public CMessage
{
public:
	class CSyncException : public std::runtime_error {
	public:
		const static int INTERNAL_ERROR;
		const static int TIMEOUT;
		const static int SIGNAL_INTERRUPT;
		const static int UNEXPECTED_ERROR;

	public:
		CSyncException (const std::string& arg, int code) : runtime_error (arg) {
			mErrcode = code;
		}

		int code() const {
			return mErrcode;
		}

	private:
		int mErrcode;
	};

public:
	CRequestMessage (CMessage *pMsg);
	CRequestMessage (CImmSocketClient *pClient);
	virtual ~CRequestMessage (void);


	int sendSync (uint8_t command, uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER) throw (CSyncException);
	int sendSync (uint8_t command, uint8_t *pData, int size, uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER) throw (CSyncException);
	bool sendAsync (CMessageId::CId *pId, uint8_t command);
	bool sendAsync (CMessageId::CId *pId, uint8_t command, uint8_t *pData, int size);
};

} // namespace ImmSocketService

#endif
