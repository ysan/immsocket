#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "MessageId.h"


namespace ImmSocketService {

CMessageId::CId::CId (void)
	:mNum (0)
	,mTime (0)
{
}

CMessageId::CId::~CId (void)
{
}

void CMessageId::CId::setNum (uint8_t n)
{
	mNum = n;
}

void CMessageId::CId::setTime (time_t time)
{
	mTime = time;
}

uint8_t CMessageId::CId::getNum (void) const
{
	return mNum;
}

time_t CMessageId::CId::getTime (void) const
{
	return mTime;
}


CMessageId::CMessageId (void)
	:mIncId (0)
{
	pthread_mutex_init (&mMutexGenId, NULL);
}

CMessageId::~CMessageId (void)
{
	pthread_mutex_destroy (&mMutexGenId);
}

CMessageId* CMessageId::getInstance (void)
{
	static CMessageId singleton;
	return &singleton;
}

CMessageId::CId CMessageId::generateId (void)
{
	CUtils::CScopedMutex scopedMutex (&mMutexGenId);


	CId id;

	id.setNum (mIncId & 0xff);
	mIncId ++;

	struct timespec t;
	clock_gettime (CLOCK_MONOTONIC_RAW, &t);
	_ISS_LOG_N ("generateId  clock_gettime t.tv_sec=[%ld]\n", t.tv_sec);
	id.setTime (t.tv_sec);

	return id;
}

} // namespace ImmSocketService
