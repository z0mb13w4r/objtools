#
# Define Include Paths
#---------------------------------------------------------------------
LIB_ROOT = ../libs/

# Define Source Directories.
#---------------------------------------------------------------------
DIR_SRC = ../src/
DIR_CXX = ../$(DIR_SRC)

# Name of c source files to be included in build.
#---------------------------------------------------------------------
SRCS_C = \
	example.c

# Name of cpp source files to be included in build.
#---------------------------------------------------------------------
SRCS_CPP =

# Name of target and map file to be built.
#---------------------------------------------------------------------
TARGETBASE = example

ifeq ($(CROSS),ARM32)
else ifeq ($(CROSS),ARM64)
else ifeq ($(CROSS),WIN32)
else ifeq ($(CROSS),WIN64)
else ifeq ($(CROSS),LINUX32)
else
	CROSS = LINUX64
endif

ifeq ($(DEBUG),y)
	DIR_OBJ = debug$(CROSS)/
	TARGET = $(TARGETBASE)d
else
	DIR_OBJ = release$(CROSS)/
	TARGET = $(TARGETBASE)
	DEBUG = n
endif

MAP_FILE = $(TARGET).map
STRIPPED_FILE = $(TARGET)-stripped

#=====================================================================================================================================
# Library Section of Makefile
#
# Section defining the library paths.
#=====================================================================================================================================

# Include Paths List
#---------------------------------------------------------------------
LIB_INCS = \
	-I../inc/

ifeq ($(CROSS),WIN32)
SYS_OBJS =
else
SYS_OBJS = \
	-lrt
endif

LIB_OBJS =

LIB_PATHS = \
	-L../bin/$(DIR_OBJ)

#=====================================================================================================================================
# Command Section of Makefile
#=====================================================================================================================================
# Set compile flags, dependant on DEBUG flag being set.
#---------------------------------------------------------------------

ifeq ($(DEBUG),y)
	CFLAGS = -Og -Wall -Wno-unknown-pragmas -c -fmessage-length=0 -MMD -MP -ggdb -g3
else
	CFLAGS = -O0 -Wall -c -fmessage-length=0 -MMD -MP
	DFLAGS += -DNDEBUG
endif

# Set profile flags, dependant on PROFILE flag being set.

ifeq ($(PROFILE),y)
	CFLAGS += -pg
	LFLAGS += -pg
else
	PROFILE = n
endif

KPROF_FILE = $(TARGET).kprof
PROFILE_FILE = gmon.out

# GNU toolchain definitions
#---------------------------------------------------------------------
ifeq ($(CROSS),ARM32)
	CROSS_COMPILE = arm-linux-gnueabihf-
	DFLAGS += -DENV_LINUX -DLINUX -DTARGET_ARM32
	EFLAGS  = -Wl,--no-enum-size-warning
	LFLAGS +=
else ifeq ($(CROSS),ARM64)
	CROSS_COMPILE = aarch64-linux-gnu-
	DFLAGS += -DENV_LINUX -DLINUX -DTARGET_ARM64
	EFLAGS  = -Wl,--no-enum-size-warning
	LFLAGS +=
else ifeq ($(CROSS),WIN32)
	CROSS_COMPILE = x86_64-w64-mingw32-
	DFLAGS += -DWIN32
	EFLAGS  =
	LFLAGS += -static-libgcc -static-libstdc++
else ifeq ($(CROSS),WIN64)
	CROSS_COMPILE = i686-w64-mingw32-
	DFLAGS += -DWIN64
	EFLAGS  =
	LFLAGS += -static-libgcc -static-libstdc++
else ifeq ($(CROSS),LINUX32)
	CROSS_COMPILE =
	CFLAGS += -m32
	DFLAGS += -DENV_LINUX -DLINUX
	EFLAGS  = -m32
	LFLAGS +=
else
	CROSS_COMPILE =
	CFLAGS +=
	DFLAGS += -DENV_LINUX -DLINUX
	EFLAGS  =
	LFLAGS +=
endif

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CPP		= $(CROSS_COMPILE)g++
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
GDB		= $(CROSS_COMPILE)gdb
STRIP		= $(CROSS_COMPILE)strip
CP		= cp
SCP		= scp
SSH		= ssh
GPROF		= gprof -b
RM		= rm -rf

# Build list of object files and dependants from c and cpp lists.
#---------------------------------------------------------------------
OBJS_CPP = $(addprefix $(DIR_OBJ), $(SRCS_CPP:.cpp=.o))
OBJS_C = $(addprefix $(DIR_OBJ), $(SRCS_C:.c=.o))
OBJS   = $(sort $(OBJS_CPP) $(OBJS_C))
DEPS   = $(OBJS:.o=.d)

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
	@echo ' set CROSS=WIN32 to build Windows version.'
	@echo ' set CROSS=LINUX32 to build 32-bit version.'
	@echo ' set CROSS=LINUX64 to build native version.'
	@echo ' '
	@echo ' e.g. make -f $(TARGETBASE).mk DEBUG=y CROSS=ARM all.'
	@echo '-----------------------------------------------------------------------------------'
	@echo ' '

# Check makefile configuration.
#---------------------------------------------------------------------
check:
	@echo '-----------------------------------------------------------------------------------'
	@echo '----------- ################# Makefile configuration ################# ------------'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'TARGET    = $(TARGET)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'DIR SRC   = $(DIR_SRC)'
	@echo 'DIR OBJ   = $(DIR_OBJ)'
	@echo 'DIR CXX   = $(DIR_CXX)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'SRC C     = $(SRCS_C)'
	@echo 'SRC CPP   = $(SRCS_CPP)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'OBJ C     = $(OBJS_C)'
	@echo 'OBJ CPP   = $(OBJS_CPP)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'ALL OBJ   = $(OBJS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'DEPS      = $(DEPS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'G++       = $(CPP)'
	@echo 'GCC       = $(CC)'
	@echo 'CFLAGS    = $(CFLAGS)'
	@echo 'DFLAGS    = $(DFLAGS)'
	@echo 'LFLAGS    = $(LFLAGS)'
	@echo '-----------------------------------------------------------------------------------'
	@echo 'LIB_INCS  = $(LIB_INCS)'
	@echo 'LIB_OBJS  = $(LIB_OBJS)'
	@echo 'LIB_PATHS = $(LIB_PATHS)'
	@echo ' '

# All Target
#---------------------------------------------------------------------
all: rm_exe dir_creation build_dependants $(TARGET)

# Remove any previously build executable - to force build.
#---------------------------------------------------------------------
rm_exe:
	@echo 'Removing executable.'
	-$(RM) $(TARGET) $(MAP_FILE) $(PROFILE_FILE)
	@echo 'Removed.'
	@echo ' '

# build rule external module attributes
#---------------------------------------------------------------------
build: $(OBJS_CPP) $(OBJS_C)

# Create directory - Build_Debug
#---------------------------------------------------------------------
dir_creation:
	@echo -n 'Directory' $$PWD"/"$(DIR_OBJ)
	@if test -d $(DIR_OBJ); then echo " exists"; else mkdir -p $(DIR_OBJ); echo "created";  fi
	@echo ' '

# Build target
#---------------------------------------------------------------------
$(TARGET): build
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Linker'
	-$(CPP) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIB_PATHS) -Wl,--start-group $(LIB_OBJS) $(SYS_OBJS) -Wl,--end-group $(EFLAGS) -Wl,-Map=$(MAP_FILE)
	@echo 'Finished building target: $@'
ifeq ($(DEBUG),y)
	-$(STRIP) -o $(STRIPPED_FILE) -s $(TARGET)
	@echo 'Finished stripping target: $@'
else
ifeq ($(CROSS),WIN32)
	-$(CP) $(TARGET).exe ../bin/$(TARGET)-32.exe
else ifeq ($(CROSS),WIN32)
	-$(CP) $(TARGET) ../bin/$(TARGET)-64-arm
else ifeq ($(CROSS),ARM32)
	-$(CP) $(TARGET) ../bin/$(TARGET)-32-arm
else ifeq ($(CROSS),ARM64)
	-$(CP) $(TARGET) ../bin/$(TARGET)-64-arm
else ifeq ($(CROSS),LINUX32)
	-$(CP) $(TARGET) ../bin/$(TARGET)-32
else
	-$(CP) $(TARGET) ../bin/$(TARGET)-64
endif
	@echo 'Finished copying target: $@'
endif

# Clean external module attributes
#---------------------------------------------------------------------
cleanall: clean clean_dependents

# Clean all - called from eclipse
#---------------------------------------------------------------------
clean:
	@echo 'Removing Object Files'
	-$(RM) $(OBJS) $(TARGET) $(MAP_FILE) $(STRIPPED_FILE) $(PROFILE_FILE) $(KPROF_FILE)
	@echo 'Removing Temporary Directories'
	-$(RM) $(DIR_OBJ)
	-@echo ' '

# Clean all dependents
#---------------------------------------------------------------------
checkall:

clean_dependents:

build_dependants:

# Generate profile for KProf
kprof:
	@echo 'Converting for KProf'
	-$(GPROF) $(TARGET) $(PROFILE_FILE) > $(KPROF_FILE)

# Phony command definitions
#---------------------------------------------------------------------
#Phony
.PHONY: dir_creation clean build check checkall rm_exe help

.SECONDARY:

