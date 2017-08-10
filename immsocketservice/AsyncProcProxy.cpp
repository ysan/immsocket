#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketServer.h"
#include "ImmSocketClient.h"
#include "Utils.h"
#include "PacketHandler.h"
#include "Message.h"
#include "AsyncProcProxy.h"


namespace ImmSocketService {

CAsyncProcProxy::CProxyThread::CProxyThread (void) :
	mIsStop (false),
	mpAsyncProcProxy (NULL)
{
	pthread_mutex_init (&mMutex, NULL);
}

CAsyncProcProxy::CProxyThread::~CProxyThread (void)
{
	pthread_mutex_destroy (&mMutex);
}


bool CAsyncProcProxy::CProxyThread::start (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (isAlive()) {
		_ISS_LOG_W ("already started\n");
		return true;
	}

    mIsStop = false;
    return create ();
}

void CAsyncProcProxy::CProxyThread::stop (void)
{
    CUtils::CScopedMutex scopedMutex (&mMutex);

    mIsStop = true;
}

void CAsyncProcProxy::CProxyThread::syncStop (void)
{
    CUtils::CScopedMutex scopedMutex (&mMutex);

    mIsStop = true;

    waitDestroy ();
}

void CAsyncProcProxy::CProxyThread::setAsyncProcProxy (CAsyncProcProxy *pAsyncProcProxy)
{
	if (pAsyncProcProxy) {
		mpAsyncProcProxy = pAsyncProcProxy;
	}
}

void CAsyncProcProxy::CProxyThread::onThreadMainRoutine (void)
{
	char szName [64] = {0};
	snprintf (szName, sizeof(szName), "%s(%ld)", "ProxyTh", syscall(SYS_gettid));
	setName (szName);
	_ISS_LOG_I ("%s %s\n", __FILE__, __func__);


	if (!mpAsyncProcProxy) {
		_ISS_LOG_E ("mpAsyncProcProxy is null\n");
		return;
	}

	pthread_mutex_t *pMutexCond = &(mpAsyncProcProxy->mMutexCond);
	pthread_cond_t *pCond = &(mpAsyncProcProxy->mCond);

	CImmSocketClient *pClient = NULL;
	CPacketHandler *pHandler = NULL;
	ST_REQ_QUEUE q ;

	int rtn = 0;
	struct timespec stTimeout = {0L, 0L};

	while (1) {
		// lock
		pthread_mutex_lock (pMutexCond);

		q = mpAsyncProcProxy->deQueue();
		if (q.isEmpty) {

			stTimeout.tv_sec = 1L;
			stTimeout.tv_nsec = 0L;
			rtn = pthread_cond_timedwait (pCond, pMutexCond, &stTimeout);

			// unlock
			pthread_mutex_unlock (pMutexCond);

			if (rtn == ETIMEDOUT) {
			// timeout
				if (mIsStop) {
					_ISS_LOG_W ("stop --> waitloop break\n");
					break;
				}
			}

		} else {

			// unlock
			pthread_mutex_unlock (pMutexCond);

			pClient = q.msg.getClientInstance();
			if (pClient) {
				pHandler = (CPacketHandler*)(pClient->getPacketHandler());
				if (pHandler) {
					// ####   execute   ####
					pHandler->handleMsg (&q.msg, q.msgType);
				}
			}
		}
	}


	_ISS_LOG_I ("%s %s end...\n", __FILE__, __func__);

	// thread end
}


CAsyncProcProxy::CAsyncProcProxy (void)
{
	pthread_mutex_init (&mMutexQue, NULL);
	pthread_mutex_init (&mMutexCond, NULL);
	pthread_cond_init (&mCond, NULL);
}

CAsyncProcProxy::~CAsyncProcProxy (void)
{
	pthread_mutex_destroy (&mMutexQue);
	pthread_mutex_destroy (&mMutexCond);
	pthread_cond_destroy (&mCond);
}

bool CAsyncProcProxy::start (void)
{
	bool rtn = true;

	for (int i = 0; i < PROXY_THREAD_POOL_NUM; i ++) {
		mProxyThread[i].setAsyncProcProxy (this);	
		if (!mProxyThread[i].start()) {
			_ISS_LOG_E ("mProxyThread[%d].start() is failure\n", i);
		}
	}

	return rtn;
}

void CAsyncProcProxy::stop (void)
{
	for (int i = 0; i < PROXY_THREAD_POOL_NUM; i ++) {
		mProxyThread[i].stop();
	}
}

void CAsyncProcProxy::syncStop (void)
{
	for (int i = 0; i < PROXY_THREAD_POOL_NUM; i ++) {
		mProxyThread[i].syncStop();
	}
}

void CAsyncProcProxy::request (ST_REQ_QUEUE *pstReqQue)
{
	if (!pstReqQue) {
		return;
	}

	CUtils::CScopedMutex scopedMutex (&mMutexCond);


	enQueue (pstReqQue);
	pthread_cond_broadcast (&mCond); // thread pool start
}

void CAsyncProcProxy::enQueue (ST_REQ_QUEUE *pstReqQue)
{
	CUtils::CScopedMutex scopedMutex (&mMutexQue);


	if (pstReqQue) {
		mQue.push (*pstReqQue);
	}
}

ST_REQ_QUEUE CAsyncProcProxy::deQueue (bool isPeep)
{
	CUtils::CScopedMutex scopedMutex (&mMutexQue);


	ST_REQ_QUEUE rtn;

	if (mQue.size() == 0) {
		return rtn;

	} else {

		if (isPeep) {
			return mQue.front();
		} else {
			rtn = mQue.front();
			mQue.pop();
			return rtn;
		}
	}
}

} // namespace ImmSocketService
