OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep
LIB_DIR = $(BUILD_DIR)/lib
TARGET_DIR = $(BUILD_DIR)

DIRS = $(OBJ_DIR) $(DEP_DIR) $(LIB_DIR) $(TARGET_DIR)
#$(DIRS):
#	mkdir -p $@

TARGET_SUFFIX = $(suffix $(TARGET))

ifeq ($(TARGET_SUFFIX), .a)
	IS_STATIC_LIB = 1
else ifeq ($(TARGET_SUFFIX), .so)
	IS_SO_LIB = 1
endif

C_SRCS = $(shell find . -type f -name '*.c')
C_SRCS := $(patsubst ./%,%,$(C_SRCS))
CPP_SRCS = $(shell find . -type f -name '*.cpp')
CPP_SRCS := $(patsubst ./%,%,$(CPP_SRCS))

C_OBJS = $(C_SRCS:.c=.o)
C_OBJS := $(addprefix $(OBJ_DIR)/, $(C_OBJS))
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
CPP_OBJS := $(addprefix $(OBJ_DIR)/, $(CPP_OBJS))

C_DEPS = $(C_SRCS:.c=.d)
C_DEPS := $(addprefix $(DEP_DIR)/, $(C_DEPS))
CPP_DEPS = $(CPP_SRCS:.cpp=.d)
CPP_DEPS := $(addprefix $(DEP_DIR)/, $(CPP_DEPS))

INCLUDE_DIR += .

CFLAGS = -g -Wall -I $(INCLUDE_DIR)
CXXFLAGS = -g -Wall -I $(INCLUDE_DIR)
LDFLAGS = -lpthread
LIBFLAGS = -fPIC

ifdef IS_SO_LIB
	CFLAGS += $(LIBFLAGS)
	CXXFLAGS += $(LIBFLAGS)
	LDFLAGS += -shared -Wl,-soname,$(TARGET)
endif

all : $(C_DEPS) $(CPP_DEPS) $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/$(TARGET) : $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(LIB_DIR)/$(TARGET_LIB) : $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBFLAGS) -o $@ $^

ifneq ($(MAKECMDGOALS), clean)
-include $(C_DEPS) $(CPP_DEPS)
endif

$(DEP_DIR)/%.d : %.c
	@echo GEN $@
	@set -e; rm -f $@; \
	$(CC) -MM $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(OBJ_DIR)/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(DEP_DIR)/%.d : %.cpp
	@echo GEN $@
	@set -e; rm -f $@; \
	$(CXX) -MM $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(OBJ_DIR)/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

