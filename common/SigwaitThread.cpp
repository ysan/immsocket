#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <signal.h>

#include "SigwaitThread.h"


CSigwaitThread::CSigwaitThread (void) :
	mIsStop (false)
{
	sigemptyset (&mSigset);
	pthread_mutex_init (&mMutex, NULL);
	pthread_mutex_init (&mMutexSigproc, NULL);
}

CSigwaitThread::~CSigwaitThread (void)
{
	pthread_mutex_destroy (&mMutex);
	pthread_mutex_destroy (&mMutexSigproc);
}


bool CSigwaitThread::start (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	if (isAlive()) {
		_UTL_LOG_W ("already started\n");
		return true;
	}

	mIsStop = false;
	return create ();
}

void CSigwaitThread::stop (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;
}

void CSigwaitThread::syncStop (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	mIsStop = true;

	waitDestroy ();
}

void CSigwaitThread::regSigwaitHandler (const CSigwaitThread::ISigwaitHandler* pHandler)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSigproc);

	if (!pHandler) {
		return ;
	}

	mHandlerList.push_back (const_cast<CSigwaitThread::ISigwaitHandler*>(pHandler));
}

void CSigwaitThread::unregSigwaitHandler (const CSigwaitThread::ISigwaitHandler* pHandler)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSigproc);

	if (!pHandler) {
		return ;
	}

	if (mHandlerList.size() == 0) {
		return ;
	}

	vector <CSigwaitThread::ISigwaitHandler*>::iterator iter = mHandlerList.begin();
	while (iter != mHandlerList.end()) {
		if (*iter == pHandler) {
			// erase & update next iter
			iter = mHandlerList.erase (iter);
			continue;
		}

		++ iter;
	}
}

bool CSigwaitThread::watchSignal (const int *pSetArray)
{
	if (!pSetArray) {
		return false;
	}

	return setSignalMask (pSetArray);
}

bool CSigwaitThread::unwatchSignal (void)
{
	return unsetSignalMask ();
}

bool CSigwaitThread::setSignalMask (const int *pSetArray)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSigproc);

	if (!pSetArray) {
		return false;
	}

	sigemptyset (&mSigset);

	while (*pSetArray != 0) {
		sigaddset (&mSigset, *pSetArray);
		++ pSetArray;
	}

	int rtn = pthread_sigmask (SIG_BLOCK, &mSigset, NULL);
	if (rtn != 0) {
		_UTL_PERROR ("pthread_sigmask");
		return false;
	}

	return true;
}

bool CSigwaitThread::unsetSignalMask (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSigproc);

	int rtn = pthread_sigmask (SIG_UNBLOCK, &mSigset, NULL);
	if (rtn != 0) {
		_UTL_PERROR ("pthread_sigmask");
		return false;
	}

	sigemptyset (&mSigset);

	return true;
}

void CSigwaitThread::execHandler (int signo)
{
	CUtils::CScopedMutex scopedMutex (&mMutexSigproc);

	vector <CSigwaitThread::ISigwaitHandler*>::iterator iter;
	for (iter = mHandlerList.begin(); iter != mHandlerList.end(); ++ iter) {
		CSigwaitThread::ISigwaitHandler *pHandler = *iter;
		if (pHandler) {
			pHandler->onHandleSignal (signo);
		}
	}
}

void CSigwaitThread::onThreadMainRoutine (void)
{
	setName ((char*)"SigwaitThread");
	_UTL_LOG_I ("%s %s\n", __FILE__, __func__);


	struct timespec stTimeout;
	siginfo_t stSiginfo;
	while (1) {
		stTimeout.tv_sec = 1L;
		stTimeout.tv_nsec = 0L;
		memset (&stSiginfo, 0x00, sizeof(stSiginfo));

		if (sigtimedwait(&mSigset, &stSiginfo, &stTimeout) > 0) {

			execHandler (stSiginfo.si_signo);

		} else {
			if (errno == EAGAIN) {
				// timeout
				if (mIsStop) {
					_UTL_LOG_I ("stop\n");
					break;
				}
			} else {
				_UTL_PERROR ("sigwait()");
			}
		}
	}


	_UTL_LOG_I ("%s %s end...\n", __FILE__, __func__);

	// thread end
}
