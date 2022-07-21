######################TRUE########################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O3 -ggdb -fomit-frame-pointer -falign-functions=16 -lm
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT =
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT =
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
# idle state with no fpu three phase: 70 us; with fpu: 32 us;
# more explanation here: https://stabl.atlassian.net/wiki/spaces/TECHWIKI/pages/529236020/2021-03-01+FPU-hard
ifeq ($(USE_FPU),)
  USE_FPU = hard
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv5-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = ch

# Target settings.
MCU  = cortex-m4

# Imported source files and paths.
CHIBIOS  := stablChibiOS
BUILDDIR := ./build
DEPDIR   := ./.dep
# Path for functional submodules
FUNCPATH := ./functional


# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F76xxI.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC =
	   $(ALLCSRC) \
       $(TESTSRC) \
	   src/main.c \
	   src/mainloops.c \
	   cmake-build-debug/CMakeFiles/3.22.3/CompilerIdC/CMakeCCompilerId.c \
	   test/test_hal_led.c \
	   test/test_statemachine.c \
	   test/test_usart.c \
	   lib/hal/hal_powerpath.c \
	   lib/hal/hal_usart.c \
	   lib/hal/delay.c \
	   lib/hal/hal_display.c \
	   lib/hal/hal_adc.c \
	   lib/hal/hal_led.c \
	   lib/hal/hal_button.c \
	   lib/hal/hal.c \
	   lib/hal/hal_rtc.c \
	   lib/flags/flags.c \
	   lib/statemachine/statemachine.c \
	   lib/usart/usart.c \
	   lib/logging/logging.c


# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(ALLCPPSRC)

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(TESTINC)

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DMFS_CFG_MEMORY_ALIGNMENT=8 \
        -D'LWIP_IPADDR(p)=IP4_ADDR(p,192,168,30,10)' \
        -D'LWIP_GATEWAY(p)=IP4_ADDR(p,192,168,30,1)'

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR = ./include \
		  ./lib/flags   \
		  ./lib/hal \
		  ./lib/logging \
		  ./lib/statemachine \
		  ./lib/usart


# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =
