#
#   Defines
#
BASEDIR		:=	../

CFLAGS		:=	-std=c++11

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/immsocketcommon \
	-I$(BASEDIR)/immsocket \

LIBS		:= \
	-L$(BASEDIR)/immsocket -limmsocket \
	-L$(BASEDIR)/immsocketcommon -limmsocketcommon \
	-lpthread \

LDFLAGS		:=

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
	RequestMessage.cpp \
	ReplyMessage.cpp \
	NotifyMessage.cpp \
	Common.cpp \
	MessageId.cpp \
	SyncRequestManager.cpp \
	SenderManager.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.mak

