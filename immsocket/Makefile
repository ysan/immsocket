#
#   Defines
#
BASEDIR		:=	../

CFLAGS		:=

INCLUDES    := \
	-I./ \
	-I$(BASEDIR)/immsocketcommon \

LIBS		:= \
	-L$(BASEDIR)/immsocketcommon -limmsocketcommon \
	-lpthread \

LDFLAGS		:=

APPEND_OBJS	:=

SUBDIRS		:=

USERDEFS	:=

#
#   Target object
#
TARGET_NAME	:=	immsocket

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
	ImmSocketServer.cpp \
	ImmSocketClient.cpp \
	ImmSocketCommon.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

