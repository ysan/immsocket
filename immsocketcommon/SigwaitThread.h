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
	class ISigwaitHandler
	{
	public:
		virtual ~ISigwaitHandler (void) {};

		virtual void onHandleSignal (int signo) = 0;
    };

public:
	CSigwaitThread (void);
	virtual ~CSigwaitThread (void);

	bool start (void); // async
	void stop (void); // async
	void syncStop (void);

	void regSigwaitHandler (const CSigwaitThread::ISigwaitHandler* pHandler);
	void unregSigwaitHandler (const CSigwaitThread::ISigwaitHandler* pHandler);

	bool watchSignal (const int *pSetArray);
	bool unwatchSignal (void);

private:
	bool setSignalMask (const int *pSetArray);
	bool unsetSignalMask (void);
	void execHandler (int signo);

	// override
	void onThreadMainRoutine (void);


	bool mIsStop;
	sigset_t mSigset;
	pthread_mutex_t mMutex;
	pthread_mutex_t mMutexSigproc;

	vector <CSigwaitThread::ISigwaitHandler*> mHandlerList;
};

#endif
