
# -------------------------------------------------------
# |					Sanitizer Flags						|
# -------------------------------------------------------

# Address sanitizers; all can be used concurrently.
# Use 'make sanitize-address' to use address sanitizers
ADDR_SANS	:=	-fno-sanitize-recover=all			\
				-fsanitize=undefined				\
				-fsanitize=address					\
				-fsanitize-address-use-after-scope	\
				-fsanitize=bounds					\
				-fsanitize=integer-divide-by-zero	\
				-fsanitize=float-divide-by-zero		\
				-fsanitize=float-cast-overflow		\
				-fsanitize=unreachable				\
				-fsanitize=vla-bound				\
				-fsanitize=null						\
				-fsanitize=signed-integer-overflow	\
				-fsanitize=bool						\
				-fsanitize=enum						\
				-fsanitize=return					\
				-fsanitize=shift					\
				-fsanitize=alignment				\
				-fsanitize=leak

# Thread sanitizers; can not be used with other sanitizers.
# Use 'make sanitize-thread' to use thread sanitizers
THREAD_SANS	:= -fsanitize=thread


# -------------------------------------------------------
# |           Compile-time Build Information            |
# -------------------------------------------------------

# Default build type (can be overridden via call to make)
BUILD ?= DEVELOPMENT

# Geneate build type information based on BUILD variable value
ifeq ($(BUILD), $(filter $(BUILD), 0 RELEASE BUILD_TYPE_RELEASE))
BUILD_TYPE=0
BUILD_NAME=RELEASE
BUILD_DESC=Release
else ifeq ($(BUILD), $(filter $(BUILD), 1 BETA BUILD_TYPE_BETA))
BUILD_TYPE=1
BUILD_NAME=BETA
BUILD_DESC=Public Beta
else ifeq ($(BUILD), $(filter $(BUILD), 2 DEVELOPMENT BUILD_TYPE_DEVELOPMENT))
BUILD_TYPE=2
BUILD_NAME=DEVELOPMENT
BUILD_DESC=Development & Debug
else ifeq ($(BUILD), $(filter $(BUILD), 3 TEST UNIT_TEST BUILD_TYPE_UNIT_TEST))
BUILD_TYPE=3
BUILD_NAME=TEST
BUILD_DESC=Unit Testing
else ifeq ($(BUILD), $(filter $(BUILD), 4 INTERNAL_BETA BUILD_TYPE_INTERNAL_BETA))
BUILD_TYPE=4
BUILD_NAME=INTERNAL_BETA
BUILD_DESC=Internal Beta
else ifeq ($(BUILD), $(filter $(BUILD), 5 DEBUG BUILD_TYPE_DEBUG))
BUILD_TYPE=5
BUILD_NAME=DEBUG
BUILD_DESC=Debug
else
$(error "Invalid BUILD value: '$(BUILD)'")
endif

# The build date (formatted using ISO-8601)
BUILD_DATE		:= $(shell date -u +"%Y-%m-%dT%H:%M:%SZ")

# Build version major
VERSION_MAJOR	?= 1

# Build version minor
VERSION_MINOR	?= 0

# Build version patch
VERSION_PATCH	?= 0

# Binary representation of short version (e.g 1.20 = 0x0114)
VERSION			:= $(shell printf "0x%02X%02X" $(VERSION_MAJOR) $(VERSION_MINOR))

# Binary representation of complete build/version (e.g BUILD_TYPE_DEVELOPMENT, 1.20.5 = 0x02011405)
VERSION_FULL	:= $(shell printf "0x%02X%02X%02X%02X" $($(BUILD_TYPE)) $(VERSION_MAJOR) $(VERSION_MINOR) $(VERSION_PATCH))

# Build version string short (major.minor)
VERSION_STRING	:= $(VERSION_MAJOR).$(VERSION_MINOR)

# Build version string full (major.minor.patch-'BUILD NAME')
VERSION_STRING_FULL := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MINOR)-$(BUILD_NAME)


# -------------------------------------------------------
# |					Target Information					|
# -------------------------------------------------------

# Name of target library
TARGET			:= os

# Full name of binary
PRODUCT_NAME	:= lib$(TARGET).a


# -------------------------------------------------------
# |					Directory Paths						|
# -------------------------------------------------------

# Target filesystem root directory
SYSROOT			?= /

# Toolchain root directory
TOOLCHAIN		?= ../../contrib/llvm-clang-17/bin

# Output directory for lib/bin/header files
BUILD_DIR		?= ../../.build

# Directory for all source files
SRC_DIR			:= ./src

# Staging directory for compiled object files
OBJ_DIR			:= ./obj

# Directory for compiled and linked binaries
BIN_DIR			:= $(BUILD_DIR)/bin

# Directory for shared/static libraries
LIB_DIR			:= $(BUILD_DIR)/lib

# Directory for includable library header files
INC_DIR			:= $(BUILD_DIR)/include

# Directory to install this library's headers
INC_DIR_NAME	:= $(BUILD_DIR)/include/os


# -------------------------------------------------------
# |						Search Paths					|
# -------------------------------------------------------

# Additional directories to search for libraries
LIB_SEARCH_PATH	:= -L$(LIB_DIR)

# Additional directories to search for header files.
INCLUDES		:= -I./ -I$(INC_DIR)


# -------------------------------------------------------
# |					Source & Header Files				|
# -------------------------------------------------------

# C source files
C_SRCS		= $(shell find $(SRC_DIR) -type f -name "*.c")

# C++ source files
CXX_SRCS	= $(shell find $(SRC_DIR) -type f -name "*.cpp")

# C & CXX header files
HDRS		:= $(shell find ./inc -type f -name "*.h" -or -name "*.hpp")

# Object files from compiled source files
OBJECTS		:= $(C_SRCS:%.c=$(OBJ_DIR)/C/%.o) $(CXX_SRCS:%.cpp=$(OBJ_DIR)/CXX/%.o)

# Depend files for source files
DEPENDS		:= $(C_SRCS:%.c=$(OBJ_DIR)/C/%.d) $(CXX_SRCS:%.cpp=$(OBJ_DIR)/CXX/%.d)


# -------------------------------------------------------
# |					Compiler Information				|
# -------------------------------------------------------

# Full path to the C compiler we use
CC	:= $(TOOLCHAIN)/clang

# Full path to the C++ compiler we use
CXX	:= $(TOOLCHAIN)/clang++

# Full path to the archiver we use
AR	:= $(TOOLCHAIN)/llvm-ar


# -------------------------------------------------------
# |					Compiler Flags						|
# -------------------------------------------------------

# Compiler flags
CFLAGS		= -g -std=gnu99 -fPIC -Wall -Wextra -Wformat-security	\
				-Wfatal-errors -ftrapv -Wdouble-promotion			\
				-Wfloat-conversion --sysroot=$(SYSROOT)

# Flags to pass to the C++ compiler
CXXFLAGS	= -g -std=c++17 -fPIC -Wall -Wextra -Wformat-security	\
				-Wfatal-errors -ftrapv -Wdouble-promotion			\
				-Wfloat-conversion --sysroot=$(SYSROOT)

# Definitions to pass to the compiler
DEFINES		:=  -DVERSION_MAJOR=$(VERSION_MAJOR)			\
				-DVERSION_MINOR=$(VERSION_MINOR)			\
				-DVERSION_PATCH=$(VERSION_PATCH)			\
				-DVERSION=$(VERSION)						\
				-DVERSION_FULL=$(VERSION_FULL)				\
				-DVERSION_STRING="\"$(VERSION_STRING)\""	\
				-DVERSION_STRING_FULL="\"$(VERSION_STRING_FULL) ($(BUILD_DESC))\"" \
				-DBUILD_TYPE=$(BUILD_TYPE)					\
				-DBUILD_NAME="\"$(BUILD_NAME)\""			\
				-DBUILD_DESC="\"$(BUILD_DESC)\""			\
				-DBUILD_DATE="\"$(BUILD_DATE)\""


# -------------------------------------------------------
# |						Linker Flags					|
# -------------------------------------------------------

# Linker flags
LDFLAGS	:= -g

# Libraries to link with
LDLIBS	:=


# -------------------------------------------------------
# |					Helper Functions					|
# -------------------------------------------------------

define COMPILE
	@echo "Compiling: $(<F)"
	@mkdir -p $(@D)

	@if [ $(suffix $(<F)) = ".c" ]; then \
		$(CC) $(CFLAGS) -MMD -c $(INCLUDES) $(DEFINES) $< -o $@; \
	elif [ $(suffix $(<F)) = ".cpp" ]; then \
		$(CXX) $(CXXFLAGS) -MMD -c $(INCLUDES) $(DEFINES) $< -o $@; \
	fi
endef


# -------------------------------------------------------
# |					Makefile Targets					|
# -------------------------------------------------------

default: prebuild $(PRODUCT_NAME)
	@echo "\nBuild has finished\n"

.PHONY: prebuild
prebuild:
	@echo "Compiling $(PRODUCT_NAME) $(VERSION_STRING_FULL)\n"
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(INC_DIR_NAME)
	@cp $(HDRS) $(INC_DIR_NAME)

.PHONY: sanitize-thread
sanitize-thread: CFLAGS   += $(THREAD_SANS)
sanitize-thread: CXXFLAGS += $(THREAD_SANS)
sanitize-thread: default

.PHONY: sanitize-address
sanitize-address: CFLAGS   += $(ADDR_SANS)
sanitize-address: CXXFLAGS += $(ADDR_SANS)
sanitize-address: default

$(PRODUCT_NAME): $(OBJECTS)
	@$(AR) rcs $(LIB_DIR)/$@ $?

$(OBJ_DIR)/C/%.o: %.c
	$(call COMPILE, $<)

$(OBJ_DIR)/CXX/%.o: %.cpp
	$(call COMPILE, $<)

.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR)/*
	$(RM) -r $(LIB_DIR)/$(PRODUCT_NAME)
	$(RM) -r $(INC_DIR_NAME)

-include $(DEPENDS)
