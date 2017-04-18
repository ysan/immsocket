#
#   Defines
#
BASEDIR		:=	../

INCLUDES	:= \
	-I./ \
	-I$(BASEDIR)/common \
	-I$(BASEDIR)/localsocket \
	-I$(BASEDIR)/localsocketservice \

LIBS		:= \
	-L$(BASEDIR)/localsocketservice -llocalsocketservice \
	-L$(BASEDIR)/localsocket -llocalsocket \
	-L$(BASEDIR)/common -lcommon \
	-lpthread \

LDFLAGS		:= \
	-Wl,-rpath=`pwd`/$(BASEDIR)/common \
	-Wl,-rpath=`pwd`/$(BASEDIR)/localsocket \
	-Wl,-rpath=`pwd`/$(BASEDIR)/localsocketservice \

APPEND_OBJS	:=

USERDEFS	:= -D_DEBUG_TCP


#
#   Target object
#
TARGET_NAME	:=	client_sample

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

