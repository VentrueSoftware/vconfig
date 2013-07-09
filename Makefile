###############################################
# VConfig Configuration Parser Makefile       #
# Author: Kurt Sassenrath                     #
###############################################

#============ Project Definitions ============#

#Module name.
MODULE_NAME = vconfig

#Directories used.
INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
DIST_DIR = dist

#Source files.
SRC_FILES = vconfig.c vcerror.c vcparse.c vctype.c hash.c

#Generate appropriate source and object paths.
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

#Compiler options
CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -I$(INC_DIR)
LIBS = 
DEFS =

#Allow verbose builds (See all lines of the build process)
ifeq ($(VERBOSE), true)
V=
else
V=@
endif

#============ Build Targets ============#
#Building the executable, typically a test executable.
build: DEFS=-DSTANDALONE
build: build-intro module
	@echo -e "\t* Building executable $(MODULE_NAME)"
	$(V)$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $(DEFS) $(DIST_DIR)/$(MODULE_NAME).o -o $(DIST_DIR)/$(MODULE_NAME)

#Building the module, which is just all the source objects linked into one.
module: build-intro objs-intro $(OBJ)
	@echo -e "\t* Creating module $(MODULE_NAME).o"
	$(V)ld -r $(OBJ) -o $(DIST_DIR)/$(MODULE_NAME).o

#Include rule for all object dependency files.
-include $(OBJ:.o=.d)

#Generate object files with dynamic dependency generation.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo -e "\t\t* Generating $*.d"
	$(V)$(CC) -MM $(CFLAGS) $(INCLUDES) $(DEFS) $(SRC_DIR)/$*.c > $(OBJ_DIR)/$*.d
	@echo -e "\t\t* Compiling $*.o"
	$(V)$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFS) $(SRC_DIR)/$*.c -o $(OBJ_DIR)/$*.o
    
    #(Sort of) BLACK MAGIC -- Reformat dependency files for make purposes.
	@mv -f $(OBJ_DIR)/$*.d $(OBJ_DIR)/$*.d.tmp
	@sed -e 's|.*:|$(OBJ_DIR)/$*.o:|' < $(OBJ_DIR)/$*.d.tmp > $(OBJ_DIR)/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJ_DIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJ_DIR)/$*.d
	@rm -f $(OBJ_DIR)/$*.d.tmp


#============ Cleanup Targets ============#
clobber: 
	@echo "===============[ Clobbering $(MODULE_NAME) ]==============="
	$(V)rm -Rf $(DIST_DIR) $(OBJ_DIR)
	
clean: clean-intro module-clean
	@echo -e "\t* Cleaning up executable"
	$(V)rm -f $(DIST_DIR)/$(MODULE_NAME)

module-clean: clean-intro obj-clean
	@echo -e "\t* Cleaning up module"
	$(V)rm -f $(DIST_DIR)/$(MODULE_NAME).o
	
obj-clean: clean-intro
	@echo -e "\t* Cleaning up objects/dependency files"
	$(V)rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d


#============ Rebuild Targets ============#

rebuild: clean build

module-rebuild: module-clean module

clean-intro:
	@echo "===============[ Cleaning up $(MODULE_NAME) ]==============="

build-intro:
	@echo "================[ Building $(MODULE_NAME) ]================="
	@mkdir -p ${OBJ_DIR}
	@mkdir -p ${DIST_DIR}
	
objs-intro:
	@echo -e "\t* (Re)building object files"
