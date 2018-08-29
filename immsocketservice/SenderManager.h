#ifndef _SENDER_MANAGER_H_
#define _SENDER_MANAGER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "ImmSocketClient.h"
#include "Message.h"
#include "AsyncProcProxy.h"


using namespace std;
using namespace ImmSocket;

namespace ImmSocketService {


struct _sender_handle_info {
public:
	_sender_handle_info (uint8_t *p, int size) {
		if ((p) && (size > 0)) {
			mptr = new uint8_t [size];
			memcpy (mptr, p, size);
			msize = size;
		}
	}

	~_sender_handle_info (void) {
		delete [] mptr;
		mptr = NULL;
	}

	uint8_t *mptr;
	int msize;
};


class CSenderManager
{
public:
	template <typename T>
	class CAsyncHandlerImpl : public IAsyncHandler<T>
	{
	public:
		CAsyncHandlerImpl (void) :mpClient (NULL) {}
		virtual ~CAsyncHandlerImpl (void) {}

		void setClient (CImmSocketClient *pClient) {
			if (pClient) {
				mpClient = pClient;
			}
		}

	private:
		void onAsyncHandled (T arg) override {
			_sender_handle_info *p = arg;
			if (!p) {
				return ;
			}

			if (mpClient) {
				mpClient->sendToConnection (p->mptr, p->msize);
			}

			delete p;
		}

		CImmSocketClient *mpClient;
	};

public:
	CSenderManager (void);
	virtual ~CSenderManager (void);

	static CSenderManager* getInstance (void);

	CAsyncProcProxy<_sender_handle_info*> *getProxy (void);
	CAsyncHandlerImpl<_sender_handle_info*> *getAsyncHandler (void);

private:


	CAsyncProcProxy<_sender_handle_info*> mProxy;
	CAsyncHandlerImpl<_sender_handle_info*> mAsyncHandler; // single thread handler
};

} // namespace ImmSocketService

#endif
