#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "MessageId.h"


namespace ImmSocketService {

/**
 * FNV Hash Algorithm
 */
static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;
static uint32_t fnv_1_hash_32 (uint8_t *bytes, int len)
{
	if ((!bytes) || (len <= 0)) {
		return 0;
	}

	uint32_t hash = 0;

	hash = FNV_OFFSET_BASIS_32;
	for (int i = 0; i < len; ++ i) {
		hash = (FNV_PRIME_32 * hash) ^ (*(bytes + i));
	}

	return hash;
}

CMessageId::CId::CId (void)
	:mNum (0)
	,mHash (0)
{
}

CMessageId::CId::~CId (void)
{
}

void CMessageId::CId::setNum (uint8_t n)
{
	mNum = n;
}

void CMessageId::CId::setHash (uint32_t hash)
{
	mHash = hash;
}

uint8_t CMessageId::CId::getNum (void) const
{
	return mNum;
}

time_t CMessageId::CId::getHash (void) const
{
	return mHash;
}


CMessageId::CMessageId (void)
	:mIncId (0)
{
	srand ((uint32_t)time(NULL));
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
	uint8_t n = mIncId & 0xff;
	id.setNum (n);
	++ mIncId;

	uint32_t h = hash (n);
	_ISS_LOG_N ("generateId  hash=[0x%08x]\n", h);
	id.setHash (h);

	return id;
}

uint32_t CMessageId::hash (uint8_t id) const
{
	uint8_t bytes [8] = {0};
	bytes [0] = id;
	bytes [1] = uint8_t(rand () & 0xff);
	bytes [2] = uint8_t(rand () & 0xff);
	bytes [3] = uint8_t(rand () & 0xff);
	bytes [4] = uint8_t(rand () & 0xff);
	bytes [5] = uint8_t(rand () & 0xff);
	bytes [6] = uint8_t(rand () & 0xff);
	bytes [7] = uint8_t(rand () & 0xff);
	return fnv_1_hash_32 (bytes, sizeof(bytes));
}

} // namespace ImmSocketService
