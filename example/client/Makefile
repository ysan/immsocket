#
#   Defines
#
BASEDIR		:=	../../

CFLAGS		:=

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/immsocketcommon \
	-I$(BASEDIR)/immsocket \
	-I$(BASEDIR)/immsocketservice \

LIBS		:= \
	-L$(BASEDIR)/immsocketservice -limmsocketservice \
	-L$(BASEDIR)/immsocket -limmsocket \
	-L$(BASEDIR)/immsocketcommon -limmsocketcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocketcommon \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocket \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocketservice \

APPEND_OBJS	:=

USERDEFS	:= -D_DEBUG_TCP


#
#   Target object
#
TARGET_NAME	:=	client

#
#   Target type
#     (EXEC/SHARED/STATIC/OBJECT)
#
TARGET_TYPE	:=	EXEC

#
#   Compile sources
#
SRCS		:=

SRCS_CPP	:= \
	ClMessageHandler.cpp \
	main.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

