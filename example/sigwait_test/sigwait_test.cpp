#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SigwaitThread.h"
 

using namespace std;

static const int g_set [] = {
	SIGUSR1,
	SIGUSR2,
	0 // term
};

class CHandler : public CSigwaitThread::ISignalHandler {
public:
	CHandler (void) : mIsLock (false) {};
	virtual ~CHandler (void) {};

private:

	void onHandleSignal (int signo) {
		switch (signo) {
		case SIGUSR1: {
			_UTL_LOG_W ("catch SIGUSR1\n");

			if (!mIsLock) {
				int rtn = 0;
				int fd_lock = open ("/sys/power/wake_lock", O_RDWR);
				if (fd_lock < 0) {
					_UTL_PERROR ("open");
					break;
				}
				rtn = write (fd_lock, "testsigwait", strlen("testsigwait"));
				if (rtn == (int)strlen("testsigwait")) {
					_UTL_LOG_W ("[%s] /sys/power/wake_lock OK\n", __PRETTY_FUNCTION__);
					mIsLock = true;
				} else {
					_UTL_LOG_E ("[%s] /sys/power/wake_lock NG!!!!!!\n", __PRETTY_FUNCTION__);
					mIsLock = false;
				}
			}

			} break;

		case SIGUSR2: {
			_UTL_LOG_W ("catch SIGUSR2\n");

			if (mIsLock) {
				int rtn = 0;
				int fd_release = open ("/sys/power/wake_unlock", O_RDWR);
				if (fd_release < 0) {
					_UTL_PERROR ("open");
					break;
				}
				rtn = write (fd_release, "testsigwait", strlen("testsigwait"));
				if (rtn == (int)strlen("testsigwait")) {
					_UTL_LOG_W ("[%s] /sys/power/wake_unlock OK\n", __PRETTY_FUNCTION__);
					mIsLock = false;
				} else {
					_UTL_LOG_E ("[%s] /sys/power/wake_unlock NG!!!!!!\n", __PRETTY_FUNCTION__);
					mIsLock = true;
				}
			}

			} break;

        default:
			_UTL_LOG_E ("unexpected signal.\n");
			break;
		}
	}


	bool mIsLock ;

};

int main (void)
{
	CHandler handler;
	CSigwaitThread st;


	st.regSignalHandler (&handler);
	st.watchSignal (g_set);
	st.start();

	while (1) {
		sleep (2);
	}

	st.syncStop();
	st.unwatchSignal();
	st.unregSignalHandler (&handler);


	exit (EXIT_SUCCESS);
}
