#ifndef _MESSAGE_H_
#define _MESSAGE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <arpa/inet.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "PacketHandler.h"
#include "ImmSocketServiceCommon.h"
#include "MessageId.h"

using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {

typedef enum {
	EN_OBJTYPE_REQUESTER = 0,
	EN_OBJTYPE_REPLYER,
	EN_OBJTYPE_REPLYABLE,
	EN_OBJTYPE_NOTHING,
} EN_OBJTYPE;


class CMessage
{
public:
	static const uint8_t MSG_TYPE_REQUEST;
	static const uint8_t MSG_TYPE_REPLY;
	static const uint8_t MSG_TYPE_NOTIFY;

	static const uint8_t MASK_IS_SYNC;
	static const uint8_t MASK_REPLY_RESULT;
	static const uint8_t MASK_MSG_TYPE;

	static const uint32_t REQUEST_TIMEOUT_MAX;
	static const uint32_t REQUEST_TIMEOUT_FEVER;

public:
	// for syncRequest -> reply
	class CSync
	{
	public:
		friend class CMessage;
		friend class CPacketHandler;

		CSync (void);
		virtual ~CSync (void);


		uint8_t getCommand (void);

		uint8_t* getData (void);
		int getDataSize (void);

		bool isReplyResultOK (void) {
			return mIsReplyResultOK;
		}

	private:
		void setCommand (uint8_t command); // friend access
		void setData (uint8_t *pData, int size, bool isClear=false); // friend access
		void setReplyResult (bool isReplyResultOK) { // friend access
			mIsReplyResultOK = isReplyResultOK;
		}

		void condLock (void);   // friend access
		void condUnlock (void); // friend access
		int condWait (uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER);   // friend access
		void condSignal (void); // friend access

		pthread_cond_t mCond;
		pthread_mutex_t mMutexCond;

		uint8_t mCommand;
		bool mIsReplyResultOK;
//		uint8_t *mpData;
		int mDataSize;

		uint8_t mEntityData [0xff]; // 0xff - sizeof(ST_PACKET)
	};

public:
	friend class CPacketHandler;

	CMessage (void);
	CMessage (CImmSocketClient *pClient);
	CMessage (CMessage *pRequestMsg); // for create reply message
	CMessage (CImmSocketClient *pClient, CMessageId::CId *pId);
	CMessage (CImmSocketClient *pClient, CMessageId::CId *pId, uint8_t command);
	CMessage (CImmSocketClient *pClient, CMessageId::CId *pId, uint8_t command, EN_OBJTYPE enType);
//	CMessage (const CMessage &obj); // copy constructor
	virtual ~CMessage (void);


	uint8_t getCommand (void);

	uint8_t* getData (void);
	int getDataSize (void);

	bool isReplyResultOK (void) {
		return mIsReplyResultOK;
	}


	CMessageId::CId *getId (void); // reference current id value
	CMessageId::CId generateId (void);

	int sendRequestSync (uint8_t command, uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER);
	int sendRequestSync (uint8_t command, uint8_t *pData, int size, uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER);
	bool sendRequestAsync (CMessageId::CId *pId, uint8_t command);
	bool sendRequestAsync (CMessageId::CId *pId, uint8_t command, uint8_t *pData, int size);
	bool sendReplyOK (void);
	bool sendReplyOK (uint8_t *pReplyData, int size);
	bool sendReplyNG (void);
	bool sendReplyNG (uint8_t *pReplyData, int size);
	bool sendNotify (uint8_t command);
	bool sendNotify (uint8_t command, uint8_t *pData, int size);

	CImmSocketClient *getClientInstance (void);

	// for syncRequest -> reply
	CSync *sync (void) {
		return &mSync;
	}

private:
	void setCommand (uint8_t command);
	void setData (uint8_t *pData, int size, bool isClear=false); // friend access
	void setReplyResult (bool isReplyResultOK) { // friend access
		mIsReplyResultOK = isReplyResultOK;
	}

	EN_OBJTYPE getObjtype (void);      // friend access
	void setObjtype (EN_OBJTYPE type); // friend access


	int sendRequestSync (uint32_t nTimeoutMsec=REQUEST_TIMEOUT_FEVER);
	bool sendRequestAsync (CMessageId::CId *pId);

	bool sendRequest (CMessageId::CId *pId);
	bool sendReply (void);
	bool sendNotify (void);

	bool setPacket (CMessageId::CId *pId, uint8_t type, uint8_t command, uint8_t *pOut, int outsize);


	CMessageId::CId mId;
	uint8_t mCommand;
	bool mIsReplyResultOK;
	CImmSocketClient *mpClientInstance;
//	uint8_t *mpData;
	int mDataSize;
	EN_OBJTYPE mObjtype;

	uint8_t mEntityData [0xff]; // 0xff - sizeof(ST_PACKET)

	CSync mSync;

};

} // namespace ImmSocketService

#endif
