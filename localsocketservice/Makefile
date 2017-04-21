#
#   Defines
#
BASEDIR		:=	../

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/common \
	-I$(BASEDIR)/localsocket \

LIBS		:= \
	-L$(BASEDIR)/localsocket -llocalsocket \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common \
	-Wl,-rpath=`pwd`/$(BASEDIR)/localsocket \

APPEND_OBJS	:=


#
#   Target object
#
TARGET_NAME	:=	localsocketservice

#
#   Target type
#     (EXEC/SHARED/STATIC/OBJECT)
#
TARGET_TYPE	:=	SHARED

#
#   Compile sources
#
SRCS		:=

SRCS_CPP	:= \
	PacketHandler.cpp \
	ClientHandler.cpp \
	Message.cpp \
	AsyncProcProxy.cpp \
	LocalSocketServiceCommon.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

