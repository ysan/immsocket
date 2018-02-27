#
#   Defines
#
BASEDIR		:=	../

CFLAGS		:=

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/common \
	-I$(BASEDIR)/immsocket \

LIBS		:= \
	-L$(BASEDIR)/immsocket -limmsocket \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocket \

APPEND_OBJS	:=


#
#   Target object
#
TARGET_NAME	:=	immsocketservice

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
	Message.cpp \
	AsyncProcProxy.cpp \
	ImmSocketServiceCommon.cpp \
	MessageId.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

