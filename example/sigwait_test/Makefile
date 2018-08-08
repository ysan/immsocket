#
#   Defines
#
BASEDIR		:=	../../

INCLUDES    := \
	-I./ \
	-I$(BASEDIR)/immsocketcommon \

LIBS		:= \
	-lpthread \
	-L$(BASEDIR)/immsocketcommon -limmsocketcommon \

LDFLAGS		:= \
#	 -Wl,-rpath=`pwd`/$(BASEDIR)/immsocketcommon

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

