#
#   Defines
#
BASEDIR		:=	../

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/common \
	-I$(BASEDIR)/immsocket \
	-I$(BASEDIR)/immsocketservice \

LIBS		:= \
	-L$(BASEDIR)/immsocketservice -limmsocketservice \
	-L$(BASEDIR)/immsocket -limmsocket \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocket \
	-Wl,-rpath=`pwd`/$(BASEDIR)/immsocketservice \

APPEND_OBJS	:=

USERDEFS	:= -D_DEBUG_TCP


#
#   Target object
#
TARGET_NAME	:=	server_sample

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
	SvrClientHandler.cpp\
	SvrMessageHandler.cpp \
	main.cpp \

#
#   Configurations
#
include $(BASEDIR)/Config.make

