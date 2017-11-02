#ifndef _SIGWAIT_THREAD_H_
#define _SIGWAIT_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <vector>

#include "WorkerThread.h"
#include "Utils.h"


using namespace std;


class CSigwaitThread : public CWorkerThread
{
public:
	class ISignalHandler
	{
	public:
		virtual ~ISignalHandler (void) {};

		virtual void onHandleSignal (int signo) = 0;
    };

public:
	CSigwaitThread (void);
	virtual ~CSigwaitThread (void);

	bool start (void); // async
	void stop (void); // async
	void syncStop (void);

	void regSignalHandler (const CSigwaitThread::ISignalHandler* pHandler);
	void unregSignalHandler (const CSigwaitThread::ISignalHandler* pHandler);

	bool watchSignal (const int *pSetArray);
	bool unwatchSignal (void);

private:
	bool setSignalMask (const int *pSetArray);
	bool unsetSignalMask (void);
	void execHandler (int signo);
	void onThreadMainRoutine (void);


	bool mIsStop;
	sigset_t mSigset;
	pthread_mutex_t mMutex;
	pthread_mutex_t mMutexSigproc;

	vector <CSigwaitThread::ISignalHandler*> mHandlerList;
};

#endif
