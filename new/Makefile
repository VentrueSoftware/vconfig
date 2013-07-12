###############################################
# VConfig Configuration Parser Makefile       #
# Author: Kurt Sassenrath                     #
###############################################

#============ Project Definitions ============#

TOP_DIR  = $(shell pwd)
TEST    ?= test-val

# VConfig Library Directory names #
VCLIB_NAME    = vconfig
VCLIB_DIR     = $(TOP_DIR)/lib
VCLIB_SRC_DIR = $(VCLIB_DIR)/src
VCLIB_INC_DIR = $(VCLIB_DIR)/include

###############################################
# Test: test-val                              #
# Description:                                #
#---------------------------------------------#
# Tests VCVAL allocation/conversion           #
###############################################
ifeq ($(TEST), test-val)
TARGET_NAME      = test-val
TARGET_DIR       = $(TOP_DIR)/tests/$(TARGET_NAME)
TARGET_SRC_DIR   = $(TARGET_DIR)/src
TARGET_INC_DIR   = $(TARGET_DIR)/include
TARGET_OBJ_DIR   = $(TARGET_DIR)/obj
TARGET_DIST_DIR  = $(TARGET_DIR)
TARGET_SRC_FILES = test-val.c

# Library source files needed for this test 
VCLIB_SRC_FILES = vc-val.c
VCLIB_OBJ_DIR   = $(TARGET_OBJ_DIR)/lib
endif


# Create library/target dependency and object names
VCLIB_SRC = $(addprefix $(VCLIB_SRC_DIR)/, $(VCLIB_SRC_FILES))
VCLIB_OBJ = $(addprefix $(VCLIB_OBJ_DIR)/, $(VCLIB_SRC_FILES:.c=.o))

TARGET_SRC = $(addprefix $(TARGET_SRC_DIR)/, $(TARGET_SRC_FILES))
TARGET_OBJ = $(addprefix $(TARGET_OBJ_DIR)/, $(TARGET_SRC_FILES:.c=.o))

#============ Compiler Definitions ============#
CC       = gcc
CFLAGS   = -Wall -g
INCLUDES = $(addprefix -I, $(VCLIB_INC_DIR)) $(addprefix -I, $(TARGET_INC_DIR))
LIBS     =
DEFS     =

#============ Linker Definitions ===========#
LD       = ld
LDFLAGS  = -r

V=@

#============ Build Targets ============#

.PHONY: bin

# Build the binary
bin: bin-intro lib target
	@echo -e "\t* Creating binary $(TARGET_NAME)"
	$(V)$(CC) $(CFLAGS) $(INCLUDES) $(TARGET_OBJ_DIR)/$(VCLIB_NAME).o $(TARGET_OBJ) $(LIBS) $(DEFS) -o $(TARGET_DIST_DIR)/$(TARGET_NAME)

# Build library module
lib: target-intro lib-obj-dir $(VCLIB_OBJ)
	@echo -e "\t* Creating library module $(VCLIB_NAME).o"
	$(V)$(LD) $(LDFLAGS) $(VCLIB_OBJ) -o $(TARGET_OBJ_DIR)/$(VCLIB_NAME).o
	
# Build target module
target: target-intro $(TARGET_OBJ)
	@echo -e "\t* Creating target module $(TARGET_NAME).o"

lib-obj-dir:
	@mkdir -p $(VCLIB_OBJ_DIR)

# Printouts #
lib-intro:
	@echo -e "\t* Compiling library object code."

target-intro:
	@echo -e "\t* Compiling target object code."

bin-intro:
	@echo -e "============[ Building $(TARGET_NAME) ]============="



-include $(VCLIB_OBJ:.o=.d)
-include $(TARGET_OBJ:.o=.d)

#Generate object files with dynamic dependency generation.
$(VCLIB_OBJ_DIR)/%.o: $(VCLIB_SRC_DIR)/%.c
	@echo -e "\t\t* Generating $*.d"
	$(V)$(CC) -MM $(CFLAGS) $(INCLUDES) $(DEFS) $(VCLIB_SRC_DIR)/$*.c > $(VCLIB_OBJ_DIR)/$*.d
	@echo -e "\t\t* Compiling $*.o"
	$(V)$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFS) $(VCLIB_SRC_DIR)/$*.c -o $(VCLIB_OBJ_DIR)/$*.o
	
    #(Sort of) BLACK MAGIC -- Reformat dependency files for make purposes.
	@mv -f $(VCLIB_OBJ_DIR)/$*.d $(VCLIB_OBJ_DIR)/$*.d.tmp
	@sed -e 's|.*:|$(VCLIB_OBJ_DIR)/$*.o:|' < $(VCLIB_OBJ_DIR)/$*.d.tmp > $(VCLIB_OBJ_DIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(VCLIB_OBJ_DIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(VCLIB_OBJ_DIR)/$*.d
	@rm -f $(VCLIB_OBJ_DIR)/$*.d.tmp

$(TARGET_OBJ_DIR)/%.o: $(TARGET_SRC_DIR)/%.c
	@echo -e "\t\t* Generating $*.d"
	$(V)$(CC) -MM $(CFLAGS) $(INCLUDES) $(DEFS) $(TARGET_SRC_DIR)/$*.c > $(TARGET_OBJ_DIR)/$*.d
	@echo -e "\t\t* Compiling $*.o"
	$(V)$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFS) $(TARGET_SRC_DIR)/$*.c -o $(TARGET_OBJ_DIR)/$*.o
	
    #(Sort of) BLACK MAGIC -- Reformat dependency files for make purposes.
	@mv -f $(TARGET_OBJ_DIR)/$*.d $(TARGET_OBJ_DIR)/$*.d.tmp
	@sed -e 's|.*:|$(TARGET_OBJ_DIR)/$*.o:|' < $(TARGET_OBJ_DIR)/$*.d.tmp > $(TARGET_OBJ_DIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(TARGET_OBJ_DIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(TARGET_OBJ_DIR)/$*.d
	@rm -f $(TARGET_OBJ_DIR)/$*.d.tmp
    
clean:
	@rm -rf tests/*/obj
