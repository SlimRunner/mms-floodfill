ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(detected_OS),Windows)
	RM = del /Q /S
	FixPath = $(subst /,\,$1)
	MD = if not exist $1 mkdir $1
	FixQuotes = $(subst /,\,$(subst ",,$1))
	EXT = .exe
endif
ifeq ($(detected_OS),Linux)
	RM = rm -rf
	FixPath = $1
	MD = mkdir -p $1
	FixQuotes = $1
endif

CC         := gcc
CCVERSION  := -std=c11
CCSTRICT   := -pedantic-errors -Werror
CCWARN     := -Wall -Wextra
CCFLAGS    := $(CCVERSION) $(CCWARN) $(CCSTRICT)
DEBUG_DIR   := build/debug
RELEASE_DIR := build/release
DBG_OBJ_DIR := $(DEBUG_DIR)/objects
DBG_APP_DIR := $(DEBUG_DIR)/app
REL_OBJ_DIR := $(RELEASE_DIR)/objects
REL_APP_DIR := $(RELEASE_DIR)/app
TARGET      := maze-solver$(EXT)
INCLUDE     := -I include/
SRC         :=                           \
  $(wildcard src/*.c)

REL_OBJECTS      := $(SRC:%.c=$(REL_OBJ_DIR)/%.o)
REL_DEPENDENCIES := $(REL_OBJECTS:.o=.d)

DBG_OBJECTS      := $(SRC:%.c=$(DBG_OBJ_DIR)/%.o)
DBG_DEPENDENCIES := $(DBG_OBJECTS:.o=.d)

# default build
all: build

# Include dependencies
-include $(REL_DEPENDENCIES) $(DBG_DEPENDENCIES)

# Note to self: DO NOT ever try to change / for backslash ever again
# breaks dependecies, targets, and overcomplicates things

# release objects TODO add makefile
$(REL_OBJ_DIR)/%.o: %.c Makefile
	$(call MD,$(call FixPath,$(@D)))
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -MMD -MP -o $@

# release binary
$(REL_APP_DIR)/$(TARGET): $(REL_OBJECTS)
	$(call MD,$(call FixPath,$(@D)))
	$(CC) $(CCFLAGS) -o $(call FixPath,$(REL_APP_DIR)/$(TARGET)) $(call FixPath,$^)

# debug objects
$(DBG_OBJ_DIR)/%.o: %.c Makefile
	$(call MD,$(call FixPath,$(@D)))
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -MMD -MP -o $@

# debug binary
$(DBG_APP_DIR)/$(TARGET): $(DBG_OBJECTS)
	$(call MD,$(call FixPath,$(@D)))
	$(CC) $(CCFLAGS) -o $(call FixPath,$@) $(call FixPath,$^)

# do not interpret these names as files
.PHONY:
	all build clean info
	build-debug clean-debug info-debug

build: DEPS = $(REL_DEPENDENCIES)
build: CCFLAGS += -O2
build: $(REL_APP_DIR)/$(TARGET)

build-debug: DEPS = $(DBG_DEPENDENCIES)
build-debug: CCFLAGS += -g
build-debug: $(DBG_APP_DIR)/$(TARGET)

clean:
	$(RM) $(call FixPath,$(REL_OBJ_DIR)/*)
	$(RM) $(call FixPath,$(REL_APP_DIR)/*)

clean-debug:
	$(RM) $(call FixPath,$(DBG_OBJ_DIR)/*)
	$(RM) $(call FixPath,$(DBG_APP_DIR)/*)

info:
	@echo $(call FixQuotes,"[*] Target:          ${TARGET}")
	@echo $(call FixQuotes,"[*] Application dir: ${REL_APP_DIR}")
	@echo $(call FixQuotes,"[*] Objects dir:     ${REL_OBJ_DIR}")
	@echo $(call FixQuotes,"[*] Sources:         ${SRC}")
	@echo $(call FixQuotes,"[*] Objects:         ${REL_OBJECTS}")
	@echo $(call FixQuotes,"[*] Dependencies:    ${REL_DEPENDENCIES}")
	@echo $(call FixQuotes,"[*] Detected OS:     ${detected_OS}")

info-debug:
	@echo $(call FixQuotes,"Debug Information")
	@echo $(call FixQuotes,"[*] Target:          ${TARGET}")
	@echo $(call FixQuotes,"[*] Application dir: ${DBG_APP_DIR}")
	@echo $(call FixQuotes,"[*] Objects dir:     ${DBG_OBJ_DIR}")
	@echo $(call FixQuotes,"[*] Sources:         ${SRC}")
	@echo $(call FixQuotes,"[*] Objects:         ${DBG_OBJECTS}")
	@echo $(call FixQuotes,"[*] Dependencies:    ${DBG_DEPENDENCIES}")
	@echo $(call FixQuotes,"[*] Detected OS:     ${detected_OS}")
