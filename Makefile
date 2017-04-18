#
#   Defines
#
BASEDIR		:=	./

LIBS		:=

LDFLAGS		:=

APPEND_OBJS	:=

SUBDIRS		:= \
	common \
	localsocket \
	localsocketservice \
	client_sample \
	server_sample \


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

