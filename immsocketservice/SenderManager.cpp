#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "ImmSocketClient.h"
#include "Utils.h"
#include "Message.h"
#include "SenderManager.h"


namespace ImmSocketService {

// singleton
static CSenderManager s_instance;


CSenderManager::CSenderManager (void)
	:mProxy (&mAsyncHandler)
{
	mProxy.start ();
}

CSenderManager::~CSenderManager (void)
{
	mProxy.syncStop ();
}


CSenderManager *CSenderManager::getInstance (void)
{
	return &s_instance;
}

CAsyncProcProxy<_sender_handle_info*> *CSenderManager::getProxy (void)
{
	return &mProxy;
}

CSenderManager::CAsyncHandlerImpl<_sender_handle_info*> *CSenderManager::getAsyncHandler (void)
{
	return &mAsyncHandler;
}

} // namespace ImmSocketService
