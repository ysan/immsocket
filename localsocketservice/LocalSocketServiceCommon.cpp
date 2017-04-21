#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "LocalSocketServiceCommon.h"
#include "Utils.h"


namespace LocalSocketService {

static EN_LOG_LEVEL s_Loglevel = EN_LOG_LEVEL_N;

void setLogLevel (EN_LOG_LEVEL enLvl)
{
	s_Loglevel = enLvl;
}

EN_LOG_LEVEL getLogLevel (void)
{
	return s_Loglevel;
}

} // namespace LocalSocketService
