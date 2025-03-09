#
# Define Include Paths
#---------------------------------------------------------------------
LIB_ROOT = ../libs/
BIN_ROOT = ../bin/

# Define Source Directories.
#---------------------------------------------------------------------
DIR_SRC = ../src/
DIR_CXX = ../$(DIR_SRC)

# Name of c source files to be included in build.
#---------------------------------------------------------------------
SRCS_C = \
	arcode.c \
	buffer.c \
	opcode.c \
	pecode.c \
	printf.c \
	readar.c \
	readpe.c \
	elfcode.c \
	memfind.c \
	memlink.c \
	ocdwarf.c \
	objcopy.c \
	objdump.c \
	objhash.c \
	options.c \
	readelf.c \
	objutils.c \
	opcode-lib.c \
	opcode-capstone.c

# Name of cpp source files to be included in build.
#---------------------------------------------------------------------
SRCS_CPP =

# Name of target and map file to be built.
#---------------------------------------------------------------------
TARGETBASE = objtool

ifeq ($(CROSS),ARM)
else ifeq ($(CROSS),WIN)
else
	CROSS = I386
endif

ifeq ($(DEBUG),y)
	DIR_OBJ = debug$(CROSS)/
else
	DIR_OBJ = release$(CROSS)/
	DEBUG = n
endif

TARGET = $(BIN_ROOT)$(DIR_OBJ)lib$(TARGETBASE).a

#=====================================================================================================================================
# Library Section of Makefile
#
# Section defining the library paths.
#=====================================================================================================================================

# Include Paths List
#---------------------------------------------------------------------
LIB_INCS = \
	-I../inc/ \
	-I../inc/capstone/

#=====================================================================================================================================
# Command Section of Makefile
#=====================================================================================================================================
# Set compile flags, dependant on DEBUG flag being set.
#---------------------------------------------------------------------

ifeq ($(DEBUG),y)
	CFLAGS = -g3 -O0 -Wall -Wno-unknown-pragmas -c -fmessage-length=0 -MMD -MP -ggdb
else
	CFLAGS = -O0 -Wall -c -fmessage-length=0 -MMD -MP
	DFLAGS += -DNDEBUG
endif

# Set profile flags, dependant on PROFILE flag being set.

ifeq ($(PROFILE),y)
	CFLAGS += -pg
else
	PROFILE = n
endif

# GNU toolchain definitions
#---------------------------------------------------------------------
ifeq ($(CROSS),ARM)
	CROSS_COMPILE = /home/WF_3.02/wrlinux-3.0/sysroots/arm-mm6-glibc-small/x86-linux2/arm-wrs-linux-gnueabi-arm_iwmmxt_el-glibc_small-
	DFLAGS += -DENV_LINUX -DLINUX -DTARGET_ARM
else ifeq ($(CROSS),WIN)
	CROSS_COMPILE = x86_64-w64-mingw32-
	DFLAGS += -DWIN32
else
	CROSS_COMPILE =
	DFLAGS += -DENV_LINUX -DLINUX
endif

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CPP		= $(CROSS_COMPILE)g++
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
GDB		= $(CROSS_COMPILE)gdb
SCP		= scp
SSH		= ssh
GPROF		= gprof -b
RM		= rm -rf

# Build list of object files and dependants from c and cpp lists.
#---------------------------------------------------------------------
OBJS_CPP = $(addprefix $(DIR_OBJ), $(SRCS_CPP:.cpp=.o))
OBJS_C = $(addprefix $(DIR_OBJ), $(SRCS_C:.c=.o))
OBJS   = $(sort $(OBJS_CPP) $(OBJS_C))
DEPS   = $(OBJS_LIB:.o=.d)

#
#=====================================================================================================================================
# BUILD Command Section of Makefile
#=====================================================================================================================================
#
# compile c files.
#---------------------------------------------------------------------
%.o: $(DIR_CXX)%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(CC) $(DFLAGS) $(LIB_INCS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# compile cpp files.
#---------------------------------------------------------------------
%.o: $(DIR_CXX)%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CPP) $(DFLAGS) $(LIB_INCS) $(CFLAGS) -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Help - list of commands in makefile.
#---------------------------------------------------------------------
help:
	@echo '-----------------------------------------------------------------------------------'
	@echo '----------- ################# Makefile configuration ################# ------------'
	@echo '-----------------------------------------------------------------------------------'
	@echo ' checkall Function to call check external make files.'
	@echo '-----------------------------------------------------------------------------------'
	@echo ' clean    clean all - called from eclipse.'
	@echo ' build    build rule external module attributes.'
	@echo '-----------------------------------------------------------------------------------'
	@echo ' NOTE'
	@echo ' ****'
	@echo ' '
	@echo ' set DEBUG=y to build debug information.'
	@echo ' set DEBUG=n to build release version.'
	@echo ' '
	@echo ' set CROSS=ARM to build ARM version.'
	@echo ' set CROSS=WIN to build Windows version.'
	@echo ' set CROSS=I386 to build native version.'
	@echo ' '
	@echo ' e.g. make -f $(TARGETBASE)lib.mk DEBUG=y CROSS=ARM all.'
	@echo '-----------------------------------------------------------------------------------'
	@echo ' '

# Check makefile configuration.
#---------------------------------------------------------------------
check:
	@echo '-----------------------------------------------------------------------------------'
	@echo '----------- ################# Makefile configuration ################# ------------'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'TARGET  = $(TARGET)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'DIR SRC = $(DIR_SRC)'
	@echo 'DIR OBJ = $(DIR_OBJ)'
	@echo 'DIR CXX = $(DIR_CXX)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'SRC C   = $(SRCS_C)'
	@echo 'SRC CPP = $(SRCS_CPP)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'OBJ C   = $(OBJS_C)'
	@echo 'OBJ CPP = $(OBJS_CPP)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'ALL OBJ = $(OBJS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'DEPS    = $(DEPS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'G++     = $(CPP)'
	@echo 'GCC     = $(CC)'
	@echo 'CFLAGS  = $(CFLAGS)'
	@echo 'DFLAGS  = $(DFLAGS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'INCLUDE = $(LIB_INCS)'
	@echo ' '

# All Target
#---------------------------------------------------------------------
all: rm_lib dir_creation $(TARGET)

# Remove any previously build executable - to force build.
#---------------------------------------------------------------------
rm_lib:
	@echo 'Removing library.'
	-$(RM) $(TARGET)
	@echo 'Removed.'
	@echo ' '

# build rule external module attributes
#---------------------------------------------------------------------
build: $(OBJS_CPP) $(OBJS_C)

# Create directory - Build_Debug
#---------------------------------------------------------------------
dir_creation:
	@echo -n 'Directory' $$PWD"/"$(DIR_OBJ)
	@if test -d $(DIR_OBJ); then echo " exists"; else mkdir $(DIR_OBJ); echo "created";  fi
	@if test -d $(BIN_ROOT)$(DIR_OBJ); then echo " exists"; else mkdir -p $(BIN_ROOT)$(DIR_OBJ); echo "created";  fi
	@echo ' '

# Build target
#---------------------------------------------------------------------
$(TARGET): build
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Linker'
	-$(AR) -r $(TARGET) $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Clean external module attributes
#---------------------------------------------------------------------
cleanall: clean

# Clean all - called from eclipse
#---------------------------------------------------------------------
clean:
	@echo 'Removing Object Files'
	-$(RM) $(OBJS) $(TARGET)
	@echo 'Removing Temporary Directories'
	-$(RM) $(DIR_OBJ)
	-@echo ' '

# Phony command definitions
#---------------------------------------------------------------------
#Phony
.PHONY: dir_creation clean build check checkall rm_lib help

.SECONDARY:

