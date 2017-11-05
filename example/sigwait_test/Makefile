#
#   Defines
#
BASEDIR		:=	../../

INCLUDES    := \
	-I./ \
	-I$(BASEDIR)/common \

LIBS		:= \
	-lpthread \
	-L$(BASEDIR)/common -lcommon \

LDFLAGS		:= -Wl,-rpath=`pwd`/$(BASEDIR)/common

APPEND_OBJS	:=

SUBDIRS		:=

USERDEFS	:=

#
#   Target object
#
TARGET_NAME	:= sigwait_test

#
#   Target type
#     (EXEC/SHARED/STATIC/OBJECT)
#
TARGET_TYPE	:=	EXEC

#
#   Compile sources
#
SRCS		:=

SRCS_CPP	:= main.cpp

#
#   Configurations
#
include $(BASEDIR)/Config.make

