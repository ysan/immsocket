#ifndef _ASYNC_PROC_PROXY_H_
#define _ASYNC_PROC_PROXY_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Message.h"
#include "PacketHandler.h"
#include "ImmSocketServiceCommon.h"

#include <queue>


using namespace std;
using namespace ImmSocket;


namespace ImmSocketService {

#define PROXY_THREAD_POOL_NUM		(2)

struct ST_REQ_QUEUE {
public:
	ST_REQ_QUEUE (void) : isEmpty (true) {}
	ST_REQ_QUEUE (CMessage *pMsg, int type) {
		msg = *pMsg;
		msgType = type;
		isEmpty = false;
	}
	ST_REQ_QUEUE (const ST_REQ_QUEUE& obj) :
		msg (obj.msg),
		msgType (obj.msgType),
		isEmpty (obj.isEmpty) {}
	~ST_REQ_QUEUE (void) {}

	ST_REQ_QUEUE &operator=(const ST_REQ_QUEUE &obj) {
		msg = obj.msg;
		msgType = obj.msgType;
		isEmpty = obj.isEmpty;
		return *this;
	}

	CMessage msg;
	int msgType;
	bool isEmpty;
};
//template<class T> struct ST_REQ_QUEUE {
//	T *pInstance;
//	void (T::*cbFunc) (CMessage msg);
//	CMessage msg;
//};

typedef queue <ST_REQ_QUEUE> REQ_QUEUE;


class CAsyncProcProxy
{
public:
	class CProxyThread : public CWorkerThread
	{
	public:
		CProxyThread (void);
		virtual ~CProxyThread (void);

		bool start (void); // async
		void stop (void); // async
		void syncStop (void);

		void setAsyncProcProxy (CAsyncProcProxy *pAsyncProcProxy);


	private:
		void onThreadMainRoutine (void);


		bool mIsStop;
		CAsyncProcProxy *mpAsyncProcProxy;

		pthread_mutex_t mMutex;

	};


public:
	friend class CProxyThread;

	CAsyncProcProxy (void);
	virtual ~CAsyncProcProxy (void);

	bool start (void);
	void stop (void);
	void syncStop (void);

	void request (ST_REQ_QUEUE *pstReqQue);


private:
	void enQueue (ST_REQ_QUEUE *pstReqQue);
	ST_REQ_QUEUE deQueue (bool isPeep=false); // friend access


	CProxyThread mProxyThread [PROXY_THREAD_POOL_NUM]; // thread pool
	REQ_QUEUE mQue;
	pthread_mutex_t mMutexQue;

	pthread_mutex_t mMutexCond; // friend access
	pthread_cond_t mCond;       // friend access

};

} // namespace ImmSocketService

#endif
