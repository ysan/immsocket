#ifndef _SIGWAIT_THREAD_H_
#define _SIGWAIT_THREAD_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <signal.h>

#include "WorkerThread.h"
#include "Utils.h"


using namespace std;

class CSigwaitThread : public CWorkerThread
{
public:
	CSigwaitThread (void);
	virtual ~CSigwaitThread (void);

	bool start (void); // async
	void stop (void); // async
	void syncStop (void);

	bool setSignalMask (void);
	bool unsetSignalMask (void);

protected:
	virtual void onHandleSignal (int sig);

private:
	void onThreadMainRoutine (void);

	bool mIsStop;

	sigset_t mSigset;
	pthread_mutex_t mMutex;
};

#endif
