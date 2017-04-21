#
#   Defines
#
BASEDIR		:=	../

INCLUDES    := \
	-I./ \
	-I$(BASEDIR)/common \

LIBS		:= \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common

APPEND_OBJS	:=

SUBDIRS		:= \
	local_send_test \

USERDEFS	:=

#
#   Target object
#
TARGET_NAME	:=	localsocket

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
	LocalSocketServer.cpp \
	LocalSocketClient.cpp \
	LocalSocketCommon.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

