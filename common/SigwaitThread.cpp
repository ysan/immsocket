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
}

CSigwaitThread::~CSigwaitThread (void)
{
	pthread_mutex_destroy (&mMutex);
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

bool CSigwaitThread::setSignalMask (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	sigemptyset (&mSigset);
//	sigaddset (&mSigset, SIGINT);
	sigaddset (&mSigset, SIGTERM);
	sigaddset (&mSigset, SIGPIPE);
	sigaddset (&mSigset, SIGQUIT); //TODO terminal (ctrl + \)
	int rtn = pthread_sigmask (SIG_BLOCK, &mSigset, NULL);
	if (rtn != 0) {
		_UTL_PERROR ("pthread_sigmask");
		return false;
	}

	return true;
}

bool CSigwaitThread::unsetSignalMask (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutex);

	sigemptyset (&mSigset);
//	sigaddset (&mSigset, SIGINT);
	sigaddset (&mSigset, SIGTERM);
	sigaddset (&mSigset, SIGPIPE);
	sigaddset (&mSigset, SIGQUIT); //TODO terminal (ctrl + \)
	int rtn = pthread_sigmask (SIG_UNBLOCK, &mSigset, NULL);
	if (rtn != 0) {
		_UTL_PERROR ("pthread_sigmask");
		return false;
	}

	return true;
}

void CSigwaitThread::onHandleSignal (int sig)
{
	switch (sig) {
	case SIGINT:
		_UTL_LOG_W ("catch SIGINT\n");

		mIsStop = true;

		break;

	case SIGTERM:
		_UTL_LOG_W ("catch SIGTERM\n");

		mIsStop = true;

		break;

	case SIGPIPE:
		_UTL_LOG_W ("catch SIGPIPE\n");
		break;

	case SIGQUIT:
		_UTL_LOG_W ("catch SIGQUIT\n");
		break;

	default:
		_UTL_LOG_E ("unexpected signal.\n");
		break;
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
			onHandleSignal (stSiginfo.si_signo);

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
