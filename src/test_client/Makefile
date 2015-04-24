#author 曾超
#通用Makefile文件

#make目标
.PHONY: all clean

#编译release版本打开此注释,默认关闭编译debug版本
#COMPILE = release
ifeq ($(COMPILE), release)
	RELEASE=1
endif

#编译动态库打开此注释,默认编译可执行程序
#COMPILE_SHARED_LIB = 1

#源文件目录
DIR_SRCS = .
DIR_SRCS += ../network

#生成的可执行程序目录
DIR_EXES = .

#可执行程序名
EXE = client

#生成的动态库目录
DIR_LIBS = 

#动态库名称
ifndef RELEASE
	SHARED_LIB_NAME = liblog-1.0.0.so
else
	SHARED_LIB_NAME = liblog-1.0.0.so
endif

#编译目标
ifdef COMPILE_SHARED_LIB
	TARGET = $(addprefix $(DIR_LIBS)/, $(SHARED_LIB_NAME))
else
	TARGET = $(addprefix $(DIR_EXES)/, $(EXE))
endif

#依赖的头文件搜索目录
INCLUDE_DIR = $(shell find $(DIR_SRCS) -type d)
INCLUDE_DIRS =$(addprefix -I, $(INCLUDE_DIR))
#INCLUDE=$(INCLUDE_DIRS)
INCLUDE=

#依赖的动态库
LIBS=-lpthread

#依赖的静态库
STATICLIBS =

#编译程序
CC = gcc
CXX = g++

#调试参数
DEBUG_OPTION = -g3 -Wall

#预定义宏
DEFINE_MACRO =

#编译动态库选项
ifdef COMPILE_SHARED_LIB
	SHARED_LIB_COMPILE_FLAGS = -fPIC
	SHARED_LIB_LDFLAGS = -shared -Wl,-soname,$(SHARED_LIB_NAME)
endif

#编译选项
CFLAGS = $(DEFINE_MACRO)
CXXFLAGS = $(DEFINE_MACRO)

#连接选项
LDFLAGS = $(DEFINE_MACRO) $(LIBS)

ifndef RELEASE
	CFLAGS += $(DEBUG_OPTION)
	CXXFLAGS += $(DEBUG_OPTION)
	LDFLAGS += $(DEBUG_OPTION)
endif

ifdef COMPILE_SHARED_LIB
	CFLAGS += $(SHARED_LIB_COMPILE_FLAGS)
	CXXFLAGS += $(SHARED_LIB_COMPILE_FLAGS)
	LDFLAGS += $(SHARED_LIB_LDFLAGS)
endif

#临时目标文件目录
DIR_OBJS = ./tmp

#make搜索路径
vpath %.c $(shell find $(DIR_SRCS) -type d)
vpath %.cpp $(shell find $(DIR_SRCS) -type d)

#创建,删除目录命令
MKDIR = mkdir -p
RM = rm -fr

#需要创建的目录
DIRS = $(DIR_OBJS) $(DIR_LIBS)

#源文件,包含路径
CSRCS = $(shell find $(DIR_SRCS) -type f -name "*.c")
CXXSRCS = $(shell find $(DIR_SRCS) -type f -name "*.cpp")

#剔除目录后的源文件名
CSRCS_NO_DIR = $(notdir $(CSRCS))
CXXSRCS_NO_DIR = $(notdir $(CXXSRCS))

#目标文件
OBJS = $(CSRCS_NO_DIR:.c=.o)
OBJS += $(CXXSRCS_NO_DIR:.cpp=.o)
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))

#创建目标
all:$(DIRS) $(TARGET)

#创建目录
$(DIRS):
	$(MKDIR) $@

#生成目标程序
$(TARGET):$(OBJS)
	@echo "source files:" $(CSRCS) $(CXXSRCS)
	@echo "obj files:" $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^ $(STATICLIBS)

#清理目标
clean:
	$(RM) $(RMFLAGS) $(TARGET) $(OBJS) $(DIR_OBJS) $(DIR_LIBS)

#生成目标文件模式
$(DIR_OBJS)/%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $^

$(DIR_OBJS)/%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $^

