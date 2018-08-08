#
#   Defines
#
BASEDIR		:=	../

CFLAGS		:=

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
	Common.cpp \
	MessageId.cpp \
	SyncRequestManager.cpp \
	SenderManager.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

