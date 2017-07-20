#
#   Defines
#
BASEDIR		:=	./

CFLAGS		:=

LIBS		:=

LDFLAGS		:=

APPEND_OBJS	:=

SUBDIRS		:= \
	common \
	immsocket \
	immsocketservice \
	example \


#
#   Target object
#
TARGET_NAME	:=

#
#   Target type
#     (EXEC/SHARED/STATIC/OBJECT)
#
TARGET_TYPE	:=

#
#   Compile sources
#
SRCS		:=

SRCS_CPP	:=

#
#   Configurations
#
include $(BASEDIR)/Config.make

