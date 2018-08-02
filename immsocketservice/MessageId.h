#ifndef _MESSAGE_ID_H_
#define _MESSAGE_ID_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "Utils.h"
#include "Defs.h"
#include "Common.h"


using namespace std;

namespace ImmSocketService {


class CMessageId
{
public:
	class CId
	{
	public:
		CId (void);
		virtual ~CId (void);

		void setNum (uint8_t n);
		void setHash (uint32_t hash);

		uint8_t getNum (void) const;
		time_t getHash (void) const;


//		void operator=(const CId& obj) {
//			mNum = obj.mNum;
//			mHash = obj.mHash;
//		}

		bool operator==(const CId& obj) const {
			if ((mNum == obj.mNum) && (mHash == obj.mHash)) {
				return true;
			} else {
				return false;
			}
		}

		bool operator!=(const CId& obj) const {
			if ((mNum != obj.mNum) || (mHash != obj.mHash)) {
				return true;
			} else {
				return false;
			}
		}

//		bool operator<(const CId& right) const {
//			if ((mNum < right.mNum) && (right.mHash != right.mHash)) {
//				return true;
//			} else {
//				return false;
//			}
//		}

	private:
		uint8_t mNum;
		time_t mHash;
	};

public:
	CMessageId (void);
	virtual ~CMessageId (void);

	static CMessageId* getInstance (void);

	CId generateId (void) ;

private:
	uint32_t hash (uint8_t id) const;


	pthread_mutex_t mMutexGenId;
	uint8_t mIncId;
};

} // namespace ImmSocketService

#endif
