#
#   Defines
#
BASEDIR		:=	../

CFLAGS		:=

INCLUDES    := \
	-I./ \
	-I$(BASEDIR)/common \

LIBS		:= \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common

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

