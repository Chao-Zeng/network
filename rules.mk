OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep
LIB_DIR = $(BUILD_DIR)/lib
TARGET_DIR = $(BUILD_DIR)

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

INCLUDE_DIR += -I.

CFLAGS += -g -Wall $(INCLUDE_DIR)
CXXFLAGS += -g -Wall $(INCLUDE_DIR)
LDFLAGS += -lpthread
LIBFLAGS += -fPIC

ifdef IS_SO_LIB
	CFLAGS += $(LIBFLAGS)
	CXXFLAGS += $(LIBFLAGS)
	LDFLAGS += -shared -Wl,-soname,$(TARGET)
endif

all : $(C_DEPS) $(CPP_DEPS) $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/$(TARGET) : $(C_OBJS) $(CPP_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean :
	$(RM) $(C_DEPS) $(CPP_DEPS)
	$(RM) $(C_OBJS) $(CPP_OBJS)
	$(RM) $(TARGET_DIR)/$(TARGET)

ifneq ($(MAKECMDGOALS), clean)
-include $(C_DEPS) $(CPP_DEPS)
endif

$(DEP_DIR)/%.d : %.c
	@echo GEN $@
	@set -e; rm -f $@; \
	mkdir -p $(dir $@); \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(OBJ_DIR)/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(DEP_DIR)/%.d : %.cpp
	@echo GEN $@
	@set -e; rm -f $@; \
	mkdir -p $(dir $@); \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,$(OBJ_DIR)/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJ_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

